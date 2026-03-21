/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   option.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_CORE_OPTION_HPP
# define LIBCPP_CORE_OPTION_HPP

# include <cstdlib>
# include <iostream>

namespace libcpp
{

/*
** Option<T> — nullable value without raw pointers (C++98)
**
** Orthodox Canonical Form compliant.
*/
template<typename T>
class Option
{
public:
	/* ── static factories ──────────────────────────────────────── */
	static Option some(const T& val)
	{
		Option o;
		o._has_value = true;
		new (&o._storage.val) T(val);
		return o;
	}

	static Option none()
	{
		Option o;
		return o;
	}

	/* ── Orthodox Canonical Form ───────────────────────────────── */
	Option() : _has_value(false) {}

	Option(const Option& other) : _has_value(other._has_value)
	{
		if (_has_value)
			new (&_storage.val) T(other._storage.val);
	}

	Option& operator=(const Option& other)
	{
		if (this != &other)
		{
			_destroy();
			_has_value = other._has_value;
			if (_has_value)
				new (&_storage.val) T(other._storage.val);
		}
		return *this;
	}

	~Option()
	{
		_destroy();
	}

	/* ── queries ───────────────────────────────────────────────── */
	bool is_some() const { return _has_value; }
	bool is_none() const { return !_has_value; }

	/* ── accessors ─────────────────────────────────────────────── */
	T& unwrap()
	{
		if (!_has_value)
		{
			std::cerr << "Option::unwrap() called on None" << std::endl;
			std::abort();
		}
		return _storage.val;
	}

	const T& unwrap() const
	{
		if (!_has_value)
		{
			std::cerr << "Option::unwrap() called on None" << std::endl;
			std::abort();
		}
		return _storage.val;
	}

	T& unwrap_or(T& fallback)
	{
		return _has_value ? _storage.val : fallback;
	}

	const T& unwrap_or(const T& fallback) const
	{
		return _has_value ? _storage.val : fallback;
	}

	/* ── map: transform the contained value ───────────────────── */
	template<typename U>
	Option<U> map(U (*fn)(const T&)) const
	{
		if (_has_value)
			return Option<U>::some(fn(_storage.val));
		return Option<U>::none();
	}

	/* ── or_else: return this if Some, otherwise call fn ──────── */
	Option or_else(Option (*fn)()) const
	{
		if (_has_value)
			return *this;
		return fn();
	}

private:
	union Storage
	{
		T		val;
		char	_dummy;
		Storage() : _dummy(0) {}
		~Storage() {}
	};

	Storage	_storage;
	bool	_has_value;

	void _destroy()
	{
		if (_has_value)
			_storage.val.~T();
		_has_value = false;
	}
};

} /* namespace libcpp */

#endif /* LIBCPP_CORE_OPTION_HPP */
