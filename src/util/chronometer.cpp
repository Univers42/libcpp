// chronometer.cpp — Chronometer implementation.

#include "libcpp/util/chronometer.hpp"

namespace libcpp {
namespace util {

Chronometer::Chronometer() : _accumulated(Duration::zero()), _running(false) {}

void Chronometer::start()
{
    if (!_running) {
        _start   = Clock::now();
        _running = true;
    }
}

void Chronometer::stop()
{
    if (_running) {
        _accumulated += Clock::now() - _start;
        _running = false;
    }
}

void Chronometer::reset()
{
    _accumulated = Duration::zero();
    if (_running)
        _start = Clock::now();
}

double Chronometer::elapsedSeconds() const
{
    Duration d = _accumulated;
    if (_running)
        d += Clock::now() - _start;
    return d.count();
}

double Chronometer::elapsedMilliseconds() const { return elapsedSeconds() * 1000.0; }
double Chronometer::elapsedMicroseconds() const { return elapsedSeconds() * 1000000.0; }

bool Chronometer::isRunning() const { return _running; }

} // namespace util
} // namespace libcpp
