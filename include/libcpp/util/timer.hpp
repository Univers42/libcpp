#pragma once
// timer.hpp — Callback-based timer (single-shot or repeating).
//
// Fires a callback after a specified delay.  Can be one-shot or repeating.
// Uses a background thread to wait.

#include <atomic>
#include <chrono>
#include <functional>
#include <thread>

namespace libcpp {
namespace util {

/// @brief Callback-based timer (setTimeout / setInterval) with background thread.
class Timer {
public:
    Timer();
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    ~Timer();

    // Single-shot: calls fn once after delay
    void setTimeout(std::function<void()> fn,
                    std::chrono::milliseconds delay);

    // Repeating: calls fn every interval until stop()
    void setInterval(std::function<void()> fn,
                     std::chrono::milliseconds interval);

    // Cancel a pending or repeating timer
    void stop();

    [[nodiscard]] bool isRunning() const;

private:
    std::atomic<bool> _running{false};
    std::thread       _thread;
};

} // namespace util
} // namespace libcpp
