/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traffic_stats.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/24 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp98/traffic_stats.hpp"

namespace libcpp98 {

TrafficCounters::TrafficCounters()
	: linesIn(0), linesOut(0), bytesIn(0), bytesOut(0)
{
}

TrafficStats::TrafficStats() : _live(), _totals(), _sessions(0)
{
}

void TrafficStats::open(int key)
{
	_live[key] = TrafficCounters();
	++_sessions;
}

void TrafficStats::countIn(int key, std::size_t bytes)
{
	TrafficCounters &c = _live[key];
	++c.linesIn;
	c.bytesIn += bytes;
	++_totals.linesIn;
	_totals.bytesIn += bytes;
}

void TrafficStats::countOut(int key, std::size_t bytes)
{
	TrafficCounters &c = _live[key];
	++c.linesOut;
	c.bytesOut += bytes;
	++_totals.linesOut;
	_totals.bytesOut += bytes;
}

const TrafficCounters *TrafficStats::get(int key) const
{
	std::map<int, TrafficCounters>::const_iterator it = _live.find(key);
	return it == _live.end() ? 0 : &it->second;
}

bool TrafficStats::close(int key)
{
	return _live.erase(key) != 0;
}

const TrafficCounters &TrafficStats::totals() const
{
	return _totals;
}

unsigned long TrafficStats::sessionCount() const
{
	return _sessions;
}

std::size_t TrafficStats::liveCount() const
{
	return _live.size();
}

void TrafficStats::clear()
{
	_live.clear();
	_totals = TrafficCounters();
	_sessions = 0;
}

} /* namespace libcpp98 */
