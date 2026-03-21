#pragma once
// observer.hpp — Observer pattern, templated by TEvent.
//
// Subscribe lambdas to named events, notify all subscribers when events fire.
// Thread-safe via std::mutex.

#include <cstddef>
#include <functional>
#include <map>
#include <mutex>

namespace libcpp {
namespace core {

/// @brief Observer pattern — subscribe lambdas to events, notify all.
/// Thread-safe via std::mutex.
template <typename TEvent>
class Observer {
public:
    using SubscriptionId = size_t;

    Observer() = default;
    ~Observer() = default;

    [[nodiscard]] SubscriptionId subscribe(const TEvent& event,
                             const std::function<void()>& lambda)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        SubscriptionId id = _nextId++;
        _listeners[event][id] = lambda;
        return id;
    }

    void unsubscribe(const TEvent& event, SubscriptionId id)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        auto it = _listeners.find(event);
        if (it != _listeners.end())
            it->second.erase(id);
    }

    void unsubscribeAll(const TEvent& event)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _listeners.erase(event);
    }

    void notify(const TEvent& event)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        auto it = _listeners.find(event);
        if (it == _listeners.end())
            return;
        for (auto& pair : it->second)
            pair.second();
    }

    [[nodiscard]] size_t listenerCount(const TEvent& event) const
    {
        std::lock_guard<std::mutex> lock(_mtx);
        auto it = _listeners.find(event);
        if (it == _listeners.end())
            return 0;
        return it->second.size();
    }

private:
    std::map<TEvent, std::map<SubscriptionId, std::function<void()>>> _listeners;
    SubscriptionId _nextId = 0;
    mutable std::mutex _mtx;
};

} // namespace core
} // namespace libcpp
