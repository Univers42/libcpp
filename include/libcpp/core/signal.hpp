/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 20:39:28 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_CORE_SIGNAL_HPP
# define LIBCPP_CORE_SIGNAL_HPP

# include <cstddef>
# include <map>

namespace libcpp
{

/*
** ISlot<A> — abstract base for callable slots (C++98)
** Allows both function pointers and member function pointers.
*/
template<typename A>
class ISlot
{
public:
	virtual ~ISlot() {}
	virtual void call(const A& arg) = 0;
	virtual ISlot* clone() const = 0;
};

/*
** FnSlot<A> — wraps a free function pointer
*/
template<typename A>
class FnSlot : public ISlot<A>
{
public:
	typedef void (*FnPtr)(const A&);

	FnSlot() : _fn(0) {}
	explicit FnSlot(FnPtr fn) : _fn(fn) {}
	FnSlot(const FnSlot& o) : ISlot<A>(), _fn(o._fn) {}
	FnSlot& operator=(const FnSlot& o) { _fn = o._fn; return *this; }
	~FnSlot() {}

	void call(const A& arg) { if (_fn) _fn(arg); }
	ISlot<A>* clone() const { return new FnSlot(*this); }

private:
	FnPtr _fn;
};

/*
** MethodSlot<T, A> — wraps a member function pointer
*/
template<typename T, typename A>
class MethodSlot : public ISlot<A>
{
public:
	typedef void (T::*MemFn)(const A&);

	MethodSlot() : _obj(0), _fn(0) {}
	MethodSlot(T* obj, MemFn fn) : _obj(obj), _fn(fn) {}
	MethodSlot(const MethodSlot& o) : ISlot<A>(), _obj(o._obj), _fn(o._fn) {}
	MethodSlot& operator=(const MethodSlot& o)
	{ _obj = o._obj; _fn = o._fn; return *this; }
	~MethodSlot() {}

	void call(const A& arg) { if (_obj && _fn) (_obj->*_fn)(arg); }
	ISlot<A>* clone() const { return new MethodSlot(*this); }

private:
	T*		_obj;
	MemFn	_fn;
};

/*
** Signal<A> — typed synchronous signal with one argument
**
** Orthodox Canonical Form compliant.
** For C++98 we fix arity at 1 argument. Overloads for 0 and 2 args below.
*/
template<typename A>
class Signal
{
public:
	typedef std::size_t ConnectionId;

	/* ── Orthodox Canonical Form ───────────────────────────────── */
	Signal() : _next_id(0) {}

	Signal(const Signal& other) : _next_id(other._next_id)
	{
		typedef typename std::map<ConnectionId, ISlot<A>*>::const_iterator It;
		for (It it = other._slots.begin(); it != other._slots.end(); ++it)
			_slots[it->first] = it->second->clone();
	}

	Signal& operator=(const Signal& other)
	{
		if (this != &other)
		{
			_clear();
			_next_id = other._next_id;
			typedef typename std::map<ConnectionId, ISlot<A>*>::const_iterator It;
			for (It it = other._slots.begin(); it != other._slots.end(); ++it)
				_slots[it->first] = it->second->clone();
		}
		return *this;
	}

	~Signal() { _clear(); }

	/* ── connect ───────────────────────────────────────────────── */
	ConnectionId connect(void (*fn)(const A&))
	{
		ConnectionId id = _next_id++;
		_slots[id] = new FnSlot<A>(fn);
		return id;
	}

	template<typename T>
	ConnectionId connect(T* obj, void (T::*fn)(const A&))
	{
		ConnectionId id = _next_id++;
		_slots[id] = new MethodSlot<T, A>(obj, fn);
		return id;
	}

	/* ── disconnect ────────────────────────────────────────────── */
	void disconnect(ConnectionId id)
	{
		typedef typename std::map<ConnectionId, ISlot<A>*>::iterator It;
		It it = _slots.find(id);
		if (it != _slots.end())
		{
			delete it->second;
			_slots.erase(it);
		}
	}

	/* ── emit ──────────────────────────────────────────────────── */
	void emit(const A& arg) const
	{
		typedef typename std::map<ConnectionId, ISlot<A>*>::const_iterator It;
		for (It it = _slots.begin(); it != _slots.end(); ++it)
			it->second->call(arg);
	}

	/* ── utilities ─────────────────────────────────────────────── */
	std::size_t slot_count() const { return _slots.size(); }

	void clear() { _clear(); }

private:
	std::map<ConnectionId, ISlot<A>*>	_slots;
	ConnectionId						_next_id;

	void _clear()
	{
		typedef typename std::map<ConnectionId, ISlot<A>*>::iterator It;
		for (It it = _slots.begin(); it != _slots.end(); ++it)
			delete it->second;
		_slots.clear();
	}
};

/*
** Signal0 — signal with zero arguments
*/
class Signal0
{
public:
	typedef std::size_t ConnectionId;
	typedef void (*SlotFn)();

	Signal0() : _next_id(0) {}
	Signal0(const Signal0& o) : _fns(o._fns), _next_id(o._next_id) {}
	Signal0& operator=(const Signal0& o)
	{
		if (this != &o) { _fns = o._fns; _next_id = o._next_id; }
		return *this;
	}
	~Signal0() {}

	ConnectionId connect(SlotFn fn)
	{
		ConnectionId id = _next_id++;
		_fns[id] = fn;
		return id;
	}

	void disconnect(ConnectionId id) { _fns.erase(id); }

	void emit() const
	{
		typedef std::map<ConnectionId, SlotFn>::const_iterator It;
		for (It it = _fns.begin(); it != _fns.end(); ++it)
			it->second();
	}

	std::size_t slot_count() const { return _fns.size(); }
	void clear() { _fns.clear(); }

private:
	std::map<ConnectionId, SlotFn>	_fns;
	ConnectionId					_next_id;
};

} /* namespace libcpp */

#endif /* LIBCPP_CORE_SIGNAL_HPP */
