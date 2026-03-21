#pragma once
// observable_value.hpp — Value wrapper that notifies on change.
//
// Stores a value and fires registered callbacks whenever the value
// is modified.  Leverages the Observer pattern.

#include <functional>
#include <mutex>
#include <vector>

namespace libcpp {
namespace core {

template <typename TType>
class ObservableValue {
public:
    using Callback = std::function<void(const TType& newVal)>;

    ObservableValue() = default;
    explicit ObservableValue(const TType& initial) : _value(initial) {}
    ~ObservableValue() = default;

    // Get the current value
    const TType& get() const { return _value; }
    operator const TType&() const { return _value; }

    // Set a new value (notifies all observers if changed)
    void set(const TType& val)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        if (_value == val)
            return;
        _value = val;
        for (const auto& cb : _callbacks)
            cb(_value);
    }

    ObservableValue& operator=(const TType& val)
    {
        set(val);
        return *this;
    }

    // Subscribe to changes
    void subscribe(Callback cb)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _callbacks.push_back(std::move(cb));
    }

    // Remove all subscribers
    void clearSubscribers()
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _callbacks.clear();
    }

    size_t subscriberCount() const
    {
        std::lock_guard<std::mutex> lock(_mtx);
        return _callbacks.size();
    }

private:
    TType                  _value{};
    std::vector<Callback>  _callbacks;
    mutable std::mutex     _mtx;
};

} // namespace core
} // namespace libcpp
