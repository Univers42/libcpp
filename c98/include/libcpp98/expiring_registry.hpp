/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expiring_registry.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/24 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP98_EXPIRING_REGISTRY_HPP
#define LIBCPP98_EXPIRING_REGISTRY_HPP

#include <cstddef>
#include <ctime>
#include <map>
#include <vector>

namespace libcpp98 {

/* ── Id-keyed table of things that go stale ────────────────────────────────
**
** A registry of in-flight operations — pending transfers, outstanding
** requests, unacknowledged invitations — each stamped with the moment it
** last saw activity, and each reachable by a small integer handed to a
** remote peer.
**
** Ids are allocated monotonically and never reused within one registry's
** lifetime, so a peer replaying a stale id gets "no such entry" rather than
** somebody else's entry.  Id 0 is never allocated and is free to mean "none".
**
** The reason this is a class and not a std::map used directly is
** collectExpired().  Sweeping a map while erasing from it inside the loop
** body is a standing invitation to iterator invalidation — and a sweep whose
** action is "tell both peers, then erase" cannot easily be written any other
** way.  collectExpired() therefore only *reports*: it takes no action and
** erases nothing, so the caller is free to do arbitrary work per id,
** including erasing, with no live iterator to invalidate.
**
** Timeouts are compared against a caller-supplied `now` rather than read
** from the clock internally.  A poll loop already knows the time, and
** passing it in makes expiry deterministic under test.
*/
template <class T>
class ExpiringRegistry {
public:
	typedef long				Id;

	struct Entry {
		T				value;
		std::time_t			lastActivity;

		Entry() : value(), lastActivity(0) {}
		Entry(const T &v, std::time_t t) : value(v), lastActivity(t) {}
	};

	typedef std::map<Id, Entry>			Map;
	typedef typename Map::const_iterator		const_iterator;
	typedef typename Map::iterator			iterator;

	explicit ExpiringRegistry(Id firstId = 1) : _entries(), _nextId(firstId) {}

	/* Returns the id assigned. Never 0 unless the registry was built with
	** firstId == 0, which callers using 0 as a sentinel must not do. */
	Id add(const T &value, std::time_t now)
	{
		const Id id = _nextId++;
		_entries[id] = Entry(value, now);
		return id;
	}

	/* NULL when there is no such id. The pointer stays valid until the
	** next erase() or clear() of that id (std::map node stability). */
	T *find(Id id)
	{
		iterator it = _entries.find(id);
		return it == _entries.end() ? 0 : &it->second.value;
	}

	const T *find(Id id) const
	{
		const_iterator it = _entries.find(id);
		return it == _entries.end() ? 0 : &it->second.value;
	}

	/* False when there is no such id, so a caller can treat "touch a
	** vanished entry" as the error it usually is. */
	bool touch(Id id, std::time_t now)
	{
		iterator it = _entries.find(id);
		if (it == _entries.end())
			return false;
		it->second.lastActivity = now;
		return true;
	}

	bool erase(Id id) { return _entries.erase(id) != 0; }

	/* Appends — does not clear — the ids idle for strictly more than
	** timeout, in ascending id order. Nothing is erased; see the class
	** comment for why that separation exists. */
	void collectExpired(std::time_t now, std::time_t timeout,
			    std::vector<Id> &out) const
	{
		for (const_iterator it = _entries.begin(); it != _entries.end(); ++it)
			if (now - it->second.lastActivity > timeout)
				out.push_back(it->first);
	}

	/* Iteration, for the searches a C++98 function pointer cannot express:
	** a predicate over two captured values has nowhere to keep them. */
	const_iterator	begin() const { return _entries.begin(); }
	const_iterator	end() const { return _entries.end(); }

	std::size_t	size() const { return _entries.size(); }
	bool		empty() const { return _entries.empty(); }
	void		clear() { _entries.clear(); }

private:
	Map	_entries;
	Id	_nextId;
};

} /* namespace libcpp98 */

#endif /* LIBCPP98_EXPIRING_REGISTRY_HPP */
