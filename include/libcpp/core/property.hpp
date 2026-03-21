/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   property.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:30:05 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_CORE_PROPERTY_HPP
# define LIBCPP_CORE_PROPERTY_HPP

# include "signal.hpp"

namespace libcpp
{

/*
** ChangePair<T> — holds old and new value for on_change signal
*/
template<typename T>
struct ChangePair
{
	T old_val;
	T new_val;

	ChangePair() : old_val(), new_val() {}
	ChangePair(const T& o, const T& n) : old_val(o), new_val(n) {}
	ChangePair(const ChangePair& cp) : old_val(cp.old_val), new_val(cp.new_val) {}
	ChangePair& operator=(const ChangePair& cp)
	{
		old_val = cp.old_val;
		new_val = cp.new_val;
		return *this;
	}
	~ChangePair() {}
};

/*
** Property<T> — observable value that fires on_change signal
**
** Orthodox Canonical Form compliant.
*/
template<typename T>
class Property
{
public:
	Signal< ChangePair<T> > on_change;

	/* ── Orthodox Canonical Form ───────────────────────────────── */
	Property() : _val() {}

	explicit Property(const T& initial) : _val(initial) {}

	Property(const Property& other) : _val(other._val), on_change(other.on_change) {}

	Property& operator=(const Property& other)
	{
		if (this != &other)
		{
			set(other._val);
			on_change = other.on_change;
		}
		return *this;
	}

	~Property() {}

	/* ── read ──────────────────────────────────────────────────── */
	const T& get() const { return _val; }
	operator const T&() const { return _val; }

	/* ── write (fires on_change if value differs) ──────────────── */
	Property& set(const T& val)
	{
		if (!(val == _val))
		{
			T old = _val;
			_val = val;
			ChangePair<T> cp(old, _val);
			on_change.emit(cp);
		}
		return *this;
	}

	/* ── force_set (always fires on_change, even if the same) ──── */
	Property& force_set(const T& val)
	{
		T old = _val;
		_val = val;
		ChangePair<T> cp(old, _val);
		on_change.emit(cp);
		return *this;
	}

private:
	T _val;
};

} /* namespace libcpp */

#endif /* LIBCPP_CORE_PROPERTY_HPP */
