#pragma once
// thread.hpp — Named thread wrapper.
//
// Wraps std::thread, sets threadSafeCout prefix from thread name,
// runs a user callback in the managed thread.

#include <atomic>
#include <functional>
#include <string>
#include <thread>

namespace libcpp {
namespace async {

class Thread {
public:
    Thread();
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;
    ~Thread();

    // Set the name of this thread (used as cout prefix)
    void setName(const std::string& name);
    const std::string& name() const;

    // Set the callback to execute
    void setCallback(std::function<void()> callback);

    // Launch the thread (throws if already running or no callback set)
    void start();

    // Request stop and join the thread
    void stop();

    bool isRunning() const;

private:
    std::string           _name;
    std::function<void()> _callback;
    std::thread           _thread;
    std::atomic<bool>     _running{false};
};

} // namespace async
} // namespace libcpp
