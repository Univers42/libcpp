#pragma once
// pool.hpp — libftpp Pool<TType> + Pool<TType>::Object<TType>
//
// Resource pool: pre-allocates TType storage, hands out RAII Object handles.
// Pool::Object calls the destructor (not delete) on release, recycling the slot.

#include <cstddef>
#include <cstring>
#include <mutex>
#include <new>
#include <stdexcept>
#include <vector>

namespace libcpp {
namespace data {

/// @brief Fixed-size object pool with RAII handle (move-only).
/// Acquires objects via placement-new and returns them automatically.
template <typename TType>
class Pool {
public:
    /// @brief RAII handle to a pool-managed object. Move-only.
    template <typename T>
    class Object {
    public:
        Object(const Object&) = delete;
        Object& operator=(const Object&) = delete;

        Object(Object&& other) noexcept
            : _ptr(other._ptr), _pool(other._pool), _index(other._index)
        {
            other._ptr = nullptr;
            other._pool = nullptr;
        }

        Object& operator=(Object&& other) noexcept
        {
            if (this != &other) {
                _release();
                _ptr = other._ptr;
                _pool = other._pool;
                _index = other._index;
                other._ptr = nullptr;
                other._pool = nullptr;
            }
            return *this;
        }

        ~Object() { _release(); }

        T* operator->() { return _ptr; }
        const T* operator->() const { return _ptr; }
        T& operator*() { return *_ptr; }
        const T& operator*() const { return *_ptr; }

    private:
        friend class Pool<T>;

        Object(T* ptr, Pool<T>* pool, size_t index)
            : _ptr(ptr), _pool(pool), _index(index) {}

        void _release()
        {
            if (_ptr && _pool) {
                _ptr->~T();
                std::lock_guard<std::mutex> lock(_pool->_mtx);
                _pool->_slots[_index].inUse = false;
                _ptr = nullptr;
                _pool = nullptr;
            }
        }

        T*       _ptr;
        Pool<T>* _pool;
        size_t   _index;
    };

    // ── Pool interface ──────────────────────────────────────────────────
    Pool() = default;
    ~Pool() { _destroyAll(); }

    Pool(const Pool&) = delete;
    Pool& operator=(const Pool&) = delete;

    void resize(const size_t& numberOfObjectStored)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _destroyAll();
        _slots.resize(numberOfObjectStored);
        for (auto& s : _slots) {
            s.inUse = false;
        }
    }

    template <typename... TArgs>
    [[nodiscard]] Object<TType> acquire(TArgs&&... p_args)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        for (size_t i = 0; i < _slots.size(); ++i) {
            if (!_slots[i].inUse) {
                TType* ptr = reinterpret_cast<TType*>(&_slots[i].storage);
                new (ptr) TType(std::forward<TArgs>(p_args)...);
                _slots[i].inUse = true;
                return Object<TType>(ptr, this, i);
            }
        }
        throw std::runtime_error("Pool: no available slots");
    }

    [[nodiscard]] size_t capacity() const
    {
        std::lock_guard<std::mutex> lock(_mtx);
        return _slots.size();
    }

    [[nodiscard]] size_t available() const
    {
        std::lock_guard<std::mutex> lock(_mtx);
        size_t count = 0;
        for (const auto& s : _slots)
            if (!s.inUse) ++count;
        return count;
    }

    [[nodiscard]] bool full() const
    {
        std::lock_guard<std::mutex> lock(_mtx);
        for (const auto& s : _slots)
            if (!s.inUse) return false;
        return true;
    }

private:
    struct Slot {
        typename std::aligned_storage<sizeof(TType), alignof(TType)>::type storage;
        bool inUse = false;
    };

    std::vector<Slot> _slots;
    mutable std::mutex _mtx;

    void _destroyAll()
    {
        for (size_t i = 0; i < _slots.size(); ++i) {
            if (_slots[i].inUse) {
                TType* ptr = reinterpret_cast<TType*>(&_slots[i].storage);
                ptr->~TType();
                _slots[i].inUse = false;
            }
        }
        _slots.clear();
    }
};

} // namespace data
} // namespace libcpp
