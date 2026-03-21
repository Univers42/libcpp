/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 20:54:36 by dlesieur         ###   ########.fr       */
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

/* ── StopWatch ─────────────────────────────────────────────────────────── */

StopWatch::StopWatch() : _start(0), _accumulated(0.0), _running(false) {}

StopWatch::StopWatch(const StopWatch& o)
	: _start(o._start), _accumulated(o._accumulated), _running(o._running) {}

StopWatch& StopWatch::operator=(const StopWatch& o)
{
	if (this != &o)
	{
		_start = o._start;
		_accumulated = o._accumulated;
		_running = o._running;
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
}

bool StopWatch::running() const { return _running; }

} /* namespace bench */
} /* namespace libcpp */
