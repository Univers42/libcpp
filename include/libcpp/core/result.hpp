/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   result.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:33:18 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_CORE_RESULT_HPP
# define LIBCPP_CORE_RESULT_HPP

# include <string>
# include <cstdlib>
# include <iostream>
# include <sstream>

namespace libcpp
{

/*
** Result<T, E> — Rust-inspired error propagation for C++98
**
** Orthodox Canonical Form: default ctor, copy ctor, operator=, destructor.
** Uses placement new / manual destructor calls to hold either T or E.
*/
template<typename T, typename E = std::string>
class Result
{
public:
	/* ── static factories ──────────────────────────────────────── */
	static Result ok(const T& val)
	{
		Result r;
		r._is_ok = true;
		new (&r._storage.val) T(val);
		return r;
	}

	static Result err(const E& error)
	{
		Result r;
		r._is_ok = false;
		new (&r._storage.error) E(error);
		return r;
	}

	/* ── Orthodox Canonical Form ───────────────────────────────── */
	Result() : _is_ok(false)
	{
		new (&_storage.error) E();
	}

	Result(const Result& other) : _is_ok(other._is_ok)
	{
		if (_is_ok)
			new (&_storage.val) T(other._storage.val);
		else
			new (&_storage.error) E(other._storage.error);
	}

	Result& operator=(const Result& other)
	{
		if (this != &other)
		{
			_destroy();
			_is_ok = other._is_ok;
			if (_is_ok)
				new (&_storage.val) T(other._storage.val);
			else
				new (&_storage.error) E(other._storage.error);
		}
		return *this;
	}

	~Result()
	{
		_destroy();
	}

	/* ── queries ───────────────────────────────────────────────── */
	bool is_ok() const  { return _is_ok; }
	bool is_err() const { return !_is_ok; }

	/* ── accessors ─────────────────────────────────────────────── */
	T& unwrap()
	{
		if (!_is_ok)
		{
			std::cerr << "Result::unwrap() called on Err value" << std::endl;
			std::abort();
		}
		return _storage.val;
	}

	const T& unwrap() const
	{
		if (!_is_ok)
		{
			std::cerr << "Result::unwrap() called on Err value" << std::endl;
			std::abort();
		}
		return _storage.val;
	}

	T& unwrap_or(T& fallback)
	{
		if (_is_ok)
			return _storage.val;
		return fallback;
	}

	const T& unwrap_or(const T& fallback) const
	{
		if (_is_ok)
			return _storage.val;
		return fallback;
	}

	E& error()
	{
		if (_is_ok)
		{
			std::cerr << "Result::error() called on Ok value" << std::endl;
			std::abort();
		}
		return _storage.error;
	}

	const E& error() const
	{
		if (_is_ok)
		{
			std::cerr << "Result::error() called on Ok value" << std::endl;
			std::abort();
		}
		return _storage.error;
	}

	/* ── map: transform the Ok value ──────────────────────────── */
	template<typename U>
	Result<U, E> map(U (*fn)(const T&)) const
	{
		if (_is_ok)
			return Result<U, E>::ok(fn(_storage.val));
		return Result<U, E>::err(_storage.error);
	}

	/* ── map_err: transform the Err value ─────────────────────── */
	template<typename F>
	Result<T, F> map_err(F (*fn)(const E&)) const
	{
		if (_is_ok)
			return Result<T, F>::ok(_storage.val);
		return Result<T, F>::err(fn(_storage.error));
	}

	/* ── and_then: flatMap — chain operations that may fail ──── */
	template<typename U>
	Result<U, E> and_then(Result<U, E> (*fn)(const T&)) const
	{
		if (_is_ok)
			return fn(_storage.val);
		return Result<U, E>::err(_storage.error);
	}

	/* ── or_else: recover from an error ───────────────────────── */
	Result or_else(Result (*fn)(const E&)) const
	{
		if (_is_ok)
			return *this;
		return fn(_storage.error);
	}

private:
	union Storage
	{
		T val;
		E error;
		Storage() {}
		~Storage() {}
	};

	Storage	_storage;
	bool	_is_ok;

	void _destroy()
	{
		if (_is_ok)
			_storage.val.~T();
		else
			_storage.error.~E();
	}
};

/*
** Specialization for void Ok type (Result<void, E>)
*/
template<typename E>
class Result<void, E>
{
public:
	static Result ok()
	{
		Result r;
		r._is_ok = true;
		return r;
	}

	static Result err(const E& error)
	{
		Result r;
		r._is_ok = false;
		r._error = error;
		return r;
	}

	/* ── Orthodox Canonical Form ───────────────────────────────── */
	Result() : _is_ok(false), _error() {}

	Result(const Result& other) : _is_ok(other._is_ok), _error(other._error) {}

	Result& operator=(const Result& other)
	{
		if (this != &other)
		{
			_is_ok = other._is_ok;
			_error = other._error;
		}
		return *this;
	}

	~Result() {}

	/* ── queries ───────────────────────────────────────────────── */
	bool is_ok() const  { return _is_ok; }
	bool is_err() const { return !_is_ok; }

	E& error()
	{
		if (_is_ok)
		{
			std::cerr << "Result<void>::error() called on Ok" << std::endl;
			std::abort();
		}
		return _error;
	}

	const E& error() const
	{
		if (_is_ok)
		{
			std::cerr << "Result<void>::error() called on Ok" << std::endl;
			std::abort();
		}
		return _error;
	}

private:
	bool	_is_ok;
	E		_error;
};

} /* namespace libcpp */

#endif /* LIBCPP_CORE_RESULT_HPP */
