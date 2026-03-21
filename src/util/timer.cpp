// timer.cpp — Callback-based Timer implementation.

#include "libcpp/util/timer.hpp"

namespace libcpp {
namespace util {

Timer::Timer() = default;

Timer::~Timer() { stop(); }

void Timer::setTimeout(std::function<void()> fn,
                       std::chrono::milliseconds delay)
{
    stop(); // cancel any existing timer
    _running = true;
    _thread  = std::thread([this, fn = std::move(fn), delay]() {
        auto end = std::chrono::steady_clock::now() + delay;
        while (_running && std::chrono::steady_clock::now() < end) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        if (_running) {
            fn();
            _running = false;
        }
    });
}

void Timer::setInterval(std::function<void()> fn,
                        std::chrono::milliseconds interval)
{
    stop();
    _running = true;
    _thread  = std::thread([this, fn = std::move(fn), interval]() {
        while (_running) {
            auto wakeAt = std::chrono::steady_clock::now() + interval;
            while (_running && std::chrono::steady_clock::now() < wakeAt) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            if (_running)
                fn();
        }
    });
}

void Timer::stop()
{
    _running = false;
    if (_thread.joinable())
        _thread.join();
}

bool Timer::isRunning() const { return _running; }

} // namespace util
} // namespace libcpp
