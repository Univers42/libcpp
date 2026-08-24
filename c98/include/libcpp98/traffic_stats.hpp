/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traffic_stats.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/24 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP98_TRAFFIC_STATS_HPP
#define LIBCPP98_TRAFFIC_STATS_HPP

#include <cstddef>
#include <map>

namespace libcpp98 {

/* ── Per-descriptor I/O counters, plus a running total ─────────────────────
**
** Sits alongside Reactor and BufferedSocket: a server that frames lines on a
** socket almost always also wants to know how many went each way, both for
** the current peer and across the process lifetime.
**
** The lifetime total is deliberately NOT the sum of the live entries.  A
** session's counters are dropped when close() retires its key, but its
** contribution to the total stays — otherwise "lines served since startup"
** would fall every time somebody disconnected.  Keeping both means the
** closing handler can still read that peer's last tally out of get()
** before calling close().
*/
struct TrafficCounters {
	unsigned long	linesIn;
	unsigned long	linesOut;
	unsigned long	bytesIn;
	unsigned long	bytesOut;

	TrafficCounters();
};

class TrafficStats {
public:
	TrafficStats();

	/* Starts (or restarts, at zero) the counters for key, and bumps the
	** lifetime session count. Safe on a key that is already open — a
	** descriptor number reused after a close is the normal case. */
	void			open(int key);

	/* Counting against an unopened key opens it implicitly, so a caller
	** that misses an open() under-reports its session count rather than
	** silently dropping the bytes. */
	void			countIn(int key, std::size_t bytes);
	void			countOut(int key, std::size_t bytes);

	/* NULL when key is not open. */
	const TrafficCounters	*get(int key) const;

	/* False when key was not open. The entry's totals are already folded
	** into totals() and stay there. */
	bool			close(int key);

	const TrafficCounters	&totals() const;
	unsigned long		sessionCount() const;
	std::size_t		liveCount() const;

	/* Drops the live entries AND resets the lifetime figures. */
	void			clear();

private:
	std::map<int, TrafficCounters>	_live;
	TrafficCounters			_totals;
	unsigned long			_sessions;
};

} /* namespace libcpp98 */

#endif /* LIBCPP98_TRAFFIC_STATS_HPP */
