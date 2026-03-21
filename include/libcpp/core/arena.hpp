/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arena.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 22:23:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_CORE_ARENA_HPP
# define LIBCPP_CORE_ARENA_HPP

# include <cstddef>
# include <cstdlib>
# include <cstring>
# include <iostream>

namespace libcpp
{

/*
** Arena<T> — index-based allocator (C++98)
**
** Pre-allocates a buffer of N elements. Returns integer indices, not pointers.
** Orthodox Canonical Form (deep copy on copy-construct/assign).
*/
template<typename T>
class Arena
{
public:
	typedef std::size_t Index;

	/* ── Orthodox Canonical Form ───────────────────────────────── */
	Arena() : _data(0), _alive(0), _capacity(0), _count(0) {}

	explicit Arena(std::size_t cap)
		: _capacity(cap), _count(0)
	{
		_data = static_cast<T*>(std::malloc(sizeof(T) * cap));
		_alive = new bool[cap];
		std::memset(_alive, 0, sizeof(bool) * cap);
	}

	Arena(const Arena& other)
		: _capacity(other._capacity), _count(other._count)
	{
		_data = static_cast<T*>(std::malloc(sizeof(T) * _capacity));
		_alive = new bool[_capacity];
		std::memcpy(_alive, other._alive, sizeof(bool) * _capacity);
		for (std::size_t i = 0; i < _capacity; ++i)
		{
			if (_alive[i])
				new (&_data[i]) T(other._data[i]);
		}
	}

	Arena& operator=(const Arena& other)
	{
		if (this != &other)
		{
			_destroy_all();
			std::free(_data);
			delete[] _alive;
			_capacity = other._capacity;
			_count = other._count;
			_data = static_cast<T*>(std::malloc(sizeof(T) * _capacity));
			_alive = new bool[_capacity];
			std::memcpy(_alive, other._alive, sizeof(bool) * _capacity);
			for (std::size_t i = 0; i < _capacity; ++i)
			{
				if (_alive[i])
					new (&_data[i]) T(other._data[i]);
			}
		}
		return *this;
	}

	~Arena()
	{
		_destroy_all();
		std::free(_data);
		delete[] _alive;
	}

	/* ── alloc: construct a new element, return its index ──────── */
	Index alloc(const T& val)
	{
		for (std::size_t i = 0; i < _capacity; ++i)
		{
			if (!_alive[i])
			{
				new (&_data[i]) T(val);
				_alive[i] = true;
				++_count;
				return i;
			}
		}
		std::cerr << "Arena: out of capacity" << std::endl;
		std::abort();
		return 0;
	}

	/* ── free: destruct element at index ──────────────────────── */
	void free(Index idx)
	{
		if (idx < _capacity && _alive[idx])
		{
			_data[idx].~T();
			_alive[idx] = false;
			--_count;
		}
	}

	/* ── access ────────────────────────────────────────────────── */
	T& get(Index idx)
	{
		if (idx >= _capacity || !_alive[idx])
		{
			std::cerr << "Arena::get: invalid index" << std::endl;
			std::abort();
		}
		return _data[idx];
	}

	const T& get(Index idx) const
	{
		if (idx >= _capacity || !_alive[idx])
		{
			std::cerr << "Arena::get: invalid index" << std::endl;
			std::abort();
		}
		return _data[idx];
	}

	bool is_alive(Index idx) const
	{
		return idx < _capacity && _alive[idx];
	}

	/* ── metrics ───────────────────────────────────────────────── */
	std::size_t capacity() const { return _capacity; }
	std::size_t count() const    { return _count; }
	std::size_t available() const { return _capacity - _count; }
	bool empty() const           { return _count == 0; }
	bool full() const            { return _count == _capacity; }

	/* ── find: returns index of first element equal to val, or capacity if not found ── */
	Index find(const T& val) const
	{
		for (std::size_t i = 0; i < _capacity; ++i)
			if (_alive[i] && _data[i] == val)
				return i;
		return _capacity;
	}

	/* ── iteration (callback receives index and const reference) ── */
	void for_each(void (*fn)(Index, const T&)) const
	{
		for (std::size_t i = 0; i < _capacity; ++i)
			if (_alive[i])
				fn(i, _data[i]);
	}

	/* ── reset: destroy all, reclaim all slots ─────────────────── */
	void reset()
	{
		_destroy_all();
		_count = 0;
	}

private:
	T*				_data;
	bool*			_alive;
	std::size_t		_capacity;
	std::size_t		_count;

	void _destroy_all()
	{
		for (std::size_t i = 0; i < _capacity; ++i)
		{
			if (_alive[i])
			{
				_data[i].~T();
				_alive[i] = false;
			}
		}
	}
};

} /* namespace libcpp */

#endif /* LIBCPP_CORE_ARENA_HPP */
