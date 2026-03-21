#pragma once
// chronometer.hpp — High-resolution chronometer with pause/resume.
//
// Uses std::chrono::steady_clock for monotonic timing.

#include <chrono>

namespace libcpp {
namespace util {

/// @brief High-resolution chronometer with pause/resume accumulation.
class Chronometer {
public:
    Chronometer();
    ~Chronometer() = default;

    void start();
    void stop();
    void reset();

    // Elapsed since start (or between start/stop), in various units
    [[nodiscard]] double elapsedSeconds() const;
    [[nodiscard]] double elapsedMilliseconds() const;
    [[nodiscard]] double elapsedMicroseconds() const;

    [[nodiscard]] bool isRunning() const;

private:
    using Clock     = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration  = std::chrono::duration<double>;

    TimePoint _start;
    Duration  _accumulated;
    bool      _running;
};

} // namespace util
} // namespace libcpp
