/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:44:07 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/bench/timer.hpp"
#include "libcpp/term/color.hpp"
#include "libcpp/term/style.hpp"
#include <sstream>

namespace libcpp
{
namespace bench
{

/* ── ScopeTimer ────────────────────────────────────────────────────────── */

ScopeTimer::ScopeTimer() : _label("timer"), _start(std::clock()) {}

ScopeTimer::ScopeTimer(const std::string& label)
	: _label(label), _start(std::clock()) {}

ScopeTimer::ScopeTimer(const ScopeTimer& o)
	: _label(o._label), _start(o._start) {}

ScopeTimer& ScopeTimer::operator=(const ScopeTimer& o)
{
	if (this != &o) { _label = o._label; _start = o._start; }
	return *this;
}

ScopeTimer::~ScopeTimer()
{
	double ms = elapsed_ms();
	Srgb dim(128, 128, 128);
	std::ostringstream oss;
	oss << ms;
	std::cerr << dim.to_ansi_fg() << "[timer] " << _label
			  << ": " << oss.str() << " ms" << TermUtils::reset() << std::endl;
}

double ScopeTimer::elapsed_ms() const
{
	std::clock_t now = std::clock();
	return 1000.0 * static_cast<double>(now - _start) / CLOCKS_PER_SEC;
}

void ScopeTimer::reset() { _start = std::clock(); }
const std::string& ScopeTimer::label() const { return _label; }

/* ── StopWatch ─────────────────────────────────────────────────────────── */

StopWatch::StopWatch() : _start(0), _accumulated(0.0), _running(false), _lap_count(0), _lap_start(0.0) {}

StopWatch::StopWatch(const StopWatch& o)
	: _start(o._start), _accumulated(o._accumulated), _running(o._running),
	  _lap_count(o._lap_count), _lap_start(o._lap_start)
{
	for (int i = 0; i < _lap_count; ++i)
		_laps[i] = o._laps[i];
}

StopWatch& StopWatch::operator=(const StopWatch& o)
{
	if (this != &o)
	{
		_start = o._start;
		_accumulated = o._accumulated;
		_running = o._running;
		_lap_count = o._lap_count;
		_lap_start = o._lap_start;
		for (int i = 0; i < _lap_count; ++i)
			_laps[i] = o._laps[i];
	}
	return *this;
}

StopWatch::~StopWatch() {}

void StopWatch::start()
{
	if (!_running)
	{
		_start = std::clock();
		_running = true;
		_lap_start = _accumulated;
	}
}

void StopWatch::stop()
{
	if (_running)
	{
		std::clock_t now = std::clock();
		_accumulated += 1000.0 * static_cast<double>(now - _start) / CLOCKS_PER_SEC;
		_running = false;
	}
}

double StopWatch::elapsed_ms() const
{
	if (_running)
	{
		std::clock_t now = std::clock();
		return _accumulated + 1000.0 * static_cast<double>(now - _start) / CLOCKS_PER_SEC;
	}
	return _accumulated;
}

void StopWatch::reset()
{
	_accumulated = 0.0;
	_running = false;
	_start = 0;
	_lap_count = 0;
	_lap_start = 0.0;
}

bool StopWatch::running() const { return _running; }

double StopWatch::lap()
{
	double now = elapsed_ms();
	double dt = now - _lap_start;
	if (_lap_count < MAX_LAPS)
		_laps[_lap_count++] = dt;
	_lap_start = now;
	return dt;
}

int StopWatch::lap_count() const { return _lap_count; }

double StopWatch::lap_time(int i) const
{
	if (i < 0 || i >= _lap_count) return 0.0;
	return _laps[i];
}

} /* namespace bench */
} /* namespace libcpp */
