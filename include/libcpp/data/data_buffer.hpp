#pragma once
// data_buffer.hpp — polymorphic byte container for serialization.
//
// Stores arbitrary trivially-copyable types in a flat byte buffer.
// Used as the foundation for Memento::Snapshot and Message payload.

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace libcpp {
namespace data {

class DataBuffer {
public:
    DataBuffer() = default;
    DataBuffer(const char* data, size_t size);
    DataBuffer(const DataBuffer& other) = default;
    DataBuffer& operator=(const DataBuffer& other) = default;
    DataBuffer(DataBuffer&& other) noexcept = default;
    DataBuffer& operator=(DataBuffer&& other) noexcept = default;
    ~DataBuffer() = default;

    // ── Serialization ───────────────────────────────────────────────
    template <typename T>
    DataBuffer& operator<<(const T& val)
    {
        static_assert(std::is_trivially_copyable<T>::value,
                      "DataBuffer: type must be trivially copyable");
        const char* p = reinterpret_cast<const char*>(&val);
        _data.insert(_data.end(), p, p + sizeof(T));
        return *this;
    }

    // ── Deserialization ─────────────────────────────────────────────
    template <typename T>
    DataBuffer& operator>>(T& val)
    {
        static_assert(std::is_trivially_copyable<T>::value,
                      "DataBuffer: type must be trivially copyable");
        if (_readCursor + sizeof(T) > _data.size())
            throw std::runtime_error("DataBuffer: read past end of buffer");
        std::memcpy(&val, _data.data() + _readCursor, sizeof(T));
        _readCursor += sizeof(T);
        return *this;
    }

    // ── String serialization (length-prefixed) ──────────────────────
    DataBuffer& operator<<(const std::string& str);
    DataBuffer& operator>>(std::string& str);

    // ── Accessors ───────────────────────────────────────────────────
    size_t size() const;
    bool   empty() const;
    void   clear();
    const std::vector<char>& raw() const;

    // ── Raw import (for network deserialization) ────────────────────
    void setRaw(const char* data, size_t size);
    void setRaw(const uint8_t* data, size_t size);

    // ── Read cursor control ─────────────────────────────────────────
    void   resetCursor();
    size_t cursor() const;

private:
    std::vector<char> _data;
    size_t            _readCursor = 0;
};

} // namespace data
} // namespace libcpp
