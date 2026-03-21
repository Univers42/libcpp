/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   profiler.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 20:56:08 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/bench/profiler.hpp"
#include "libcpp/term/color.hpp"
#include "libcpp/term/style.hpp"
#include <sstream>
#include <iomanip>

namespace libcpp
{
namespace bench
{

/* ── ProfileEntry ──────────────────────────────────────────────────────── */

ProfileEntry::ProfileEntry() : call_count(0), total_ms(0), min_ms(1e18), max_ms(0) {}
ProfileEntry::ProfileEntry(const std::string& n)
	: name(n), call_count(0), total_ms(0), min_ms(1e18), max_ms(0) {}
ProfileEntry::ProfileEntry(const ProfileEntry& o)
	: name(o.name), call_count(o.call_count), total_ms(o.total_ms),
	  min_ms(o.min_ms), max_ms(o.max_ms) {}
ProfileEntry& ProfileEntry::operator=(const ProfileEntry& o)
{
	if (this != &o)
	{
		name = o.name; call_count = o.call_count; total_ms = o.total_ms;
		min_ms = o.min_ms; max_ms = o.max_ms;
	}
	return *this;
}
ProfileEntry::~ProfileEntry() {}
double ProfileEntry::mean_ms() const
{
	return (call_count > 0) ? total_ms / call_count : 0;
}

/* ── Profiler ──────────────────────────────────────────────────────────── */

Profiler::Profiler() : _count(0) {}
Profiler::Profiler(const Profiler&) : _count(0) {}
Profiler& Profiler::operator=(const Profiler&) { return *this; }
Profiler::~Profiler() {}

Profiler& Profiler::instance()
{
	static Profiler inst;
	return inst;
}

int Profiler::_find(const std::string& name) const
{
	for (int i = 0; i < _count; ++i)
		if (_entries[i].name == name) return i;
	return -1;
}

void Profiler::record(const std::string& name, double elapsed_ms)
{
	int idx = _find(name);
	if (idx < 0)
	{
		if (_count >= MAX_ENTRIES) return;
		idx = _count++;
		_entries[idx] = ProfileEntry(name);
	}
	ProfileEntry& e = _entries[idx];
	e.call_count++;
	e.total_ms += elapsed_ms;
	if (elapsed_ms < e.min_ms) e.min_ms = elapsed_ms;
	if (elapsed_ms > e.max_ms) e.max_ms = elapsed_ms;
}

void Profiler::report() const
{
	Srgb cyan(0, 200, 255);
	Srgb green(0, 220, 0);
	Srgb dim(128, 128, 128);

	std::cout << "\n" << cyan.to_ansi_fg()
			  << "━━━ Profiler Report ━━━"
			  << TermUtils::reset() << "\n" << std::endl;

	for (int i = 0; i < _count; ++i)
	{
		const ProfileEntry& e = _entries[i];
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(3);
		oss << "  " << green.to_ansi_fg() << e.name << TermUtils::reset()
			<< "  calls=" << e.call_count
			<< "  total=" << e.total_ms << "ms"
			<< "  mean=" << e.mean_ms() << "ms"
			<< "  min=" << e.min_ms << "ms"
			<< "  max=" << e.max_ms << "ms";
		std::cout << oss.str() << std::endl;
	}

	std::cout << "\n" << dim.to_ansi_fg()
			  << "───────────────────────────────────"
			  << TermUtils::reset() << std::endl;
	std::cout << "  " << _count << " profiled section(s)\n" << std::endl;
}

void Profiler::reset() { _count = 0; }
int Profiler::entry_count() const { return _count; }
const ProfileEntry& Profiler::entry(int i) const { return _entries[i]; }

/* ── ProfileScope ──────────────────────────────────────────────────────── */

ProfileScope::ProfileScope(const std::string& name)
	: _name(name), _start(std::clock()) {}

ProfileScope::ProfileScope(const ProfileScope& o)
	: _name(o._name), _start(o._start) {}

ProfileScope& ProfileScope::operator=(const ProfileScope& o)
{
	if (this != &o) { _name = o._name; _start = o._start; }
	return *this;
}

ProfileScope::~ProfileScope()
{
	std::clock_t now = std::clock();
	double ms = 1000.0 * static_cast<double>(now - _start) / CLOCKS_PER_SEC;
	Profiler::instance().record(_name, ms);
}

} /* namespace bench */
} /* namespace libcpp */
