/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:38:28 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_BENCH_TIMER_HPP
# define LIBCPP_BENCH_TIMER_HPP

# include <string>
# include <ctime>
# include <iostream>

namespace libcpp
{
namespace bench
{

/* ── ScopeTimer — RAII timer that prints elapsed on destruction ─────────── */

class ScopeTimer
{
public:
	ScopeTimer();
	ScopeTimer(const std::string& label);
	ScopeTimer(const ScopeTimer& o);
	ScopeTimer& operator=(const ScopeTimer& o);
	~ScopeTimer();

	/* Manual query */
	double elapsed_ms() const;
	void   reset();
	const std::string& label() const;

private:
	std::string _label;
	std::clock_t _start;
};

/* ── StopWatch — manual start/stop timer ───────────────────────────────── */

class StopWatch
{
public:
	static const int MAX_LAPS = 128;

	StopWatch();
	StopWatch(const StopWatch& o);
	StopWatch& operator=(const StopWatch& o);
	~StopWatch();

	void   start();
	void   stop();
	double elapsed_ms() const;
	void   reset();
	bool   running() const;
	double lap();
	int    lap_count() const;
	double lap_time(int i) const;

private:
	std::clock_t _start;
	double       _accumulated;
	bool         _running;
	double       _laps[MAX_LAPS];
	int          _lap_count;
	double       _lap_start;
};

} /* namespace bench */
} /* namespace libcpp */

#endif /* LIBCPP_BENCH_TIMER_HPP */
