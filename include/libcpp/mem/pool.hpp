/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pool.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_MEM_POOL_HPP
# define LIBCPP_MEM_POOL_HPP

# include <cstddef>
# include <new>

namespace libcpp
{
namespace mem
{

/* ── Pool<T, Capacity> — fixed-size object pool ────────────────────────── */
/*
** Pre-allocates storage for Capacity objects of type T.
** alloc() returns a pointer from the free list.
** dealloc() returns the pointer to the free list.
** No dynamic allocation after construction.
*/

template <typename T, int Capacity>
class Pool
{
public:
	Pool()
	{
		/* Build the free list through indices */
		for (int i = 0; i < Capacity - 1; ++i)
			_next[i] = i + 1;
		_next[Capacity - 1] = -1;
		_free_head = 0;
		_used = 0;
		for (int i = 0; i < Capacity; ++i)
			_alive[i] = false;
	}

	Pool(const Pool& o) : _free_head(o._free_head), _used(o._used)
	{
		for (int i = 0; i < Capacity; ++i)
		{
			_next[i] = o._next[i];
			_alive[i] = o._alive[i];
			if (_alive[i])
			{
				T* dst = _ptr(i);
				const T* src = o._cptr(i);
				new (dst) T(*src);
			}
		}
	}

	Pool& operator=(const Pool& o)
	{
		if (this != &o)
		{
			_destroy_all();
			_free_head = o._free_head;
			_used = o._used;
			for (int i = 0; i < Capacity; ++i)
			{
				_next[i] = o._next[i];
				_alive[i] = o._alive[i];
				if (_alive[i])
					new (_ptr(i)) T(*o._cptr(i));
			}
		}
		return *this;
	}

	~Pool() { _destroy_all(); }

	/* Allocate a default-constructed T; returns pointer or 0 if full */
	T* alloc()
	{
		if (_free_head < 0) return 0;
		int idx = _free_head;
		_free_head = _next[idx];
		_alive[idx] = true;
		++_used;
		T* p = _ptr(idx);
		new (p) T();
		return p;
	}

	/* Allocate a copy-constructed T; returns pointer or 0 if full */
	T* alloc(const T& val)
	{
		if (_free_head < 0) return 0;
		int idx = _free_head;
		_free_head = _next[idx];
		_alive[idx] = true;
		++_used;
		T* p = _ptr(idx);
		new (p) T(val);
		return p;
	}

	/* Return an object to the pool */
	void dealloc(T* p)
	{
		int idx = _index_of(p);
		if (idx < 0 || !_alive[idx]) return;
		p->~T();
		_alive[idx] = false;
		_next[idx] = _free_head;
		_free_head = idx;
		--_used;
	}

	/* Reset all — destroy and rebuild free list */
	void reset()
	{
		_destroy_all();
		for (int i = 0; i < Capacity - 1; ++i)
			_next[i] = i + 1;
		_next[Capacity - 1] = -1;
		_free_head = 0;
		_used = 0;
	}

	int  capacity() const { return Capacity; }
	int  used()     const { return _used; }
	int  available() const { return Capacity - _used; }
	bool full()     const { return _used >= Capacity; }
	bool empty()    const { return _used == 0; }

	bool is_alive(const T* p) const
	{
		int idx = _index_of_const(p);
		return (idx >= 0 && _alive[idx]);
	}

private:
	/* Storage: char array avoids default-constructing T objects */
	char _storage[Capacity * sizeof(T)];
	int  _next[Capacity];
	bool _alive[Capacity];
	int  _free_head;
	int  _used;

	T*       _ptr(int i)       { return reinterpret_cast<T*>(_storage + i * sizeof(T)); }
	const T* _cptr(int i) const { return reinterpret_cast<const T*>(_storage + i * sizeof(T)); }

	int _index_of(const T* p) const
	{
		const char* cp = reinterpret_cast<const char*>(p);
		if (cp < _storage || cp >= _storage + Capacity * sizeof(T)) return -1;
		std::ptrdiff_t off = cp - _storage;
		if (off % sizeof(T) != 0) return -1;
		return static_cast<int>(off / sizeof(T));
	}

	int _index_of_const(const T* p) const { return _index_of(p); }

	void _destroy_all()
	{
		for (int i = 0; i < Capacity; ++i)
		{
			if (_alive[i])
			{
				_ptr(i)->~T();
				_alive[i] = false;
			}
		}
	}
};

} /* namespace mem */
} /* namespace libcpp */

#endif /* LIBCPP_MEM_POOL_HPP */
