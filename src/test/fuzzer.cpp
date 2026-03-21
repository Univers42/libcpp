/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fuzzer.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:40:19 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/test/fuzzer.hpp"

namespace libcpp
{
namespace test
{

/* ── Rng — Linear Congruential Generator ───────────────────────────────── */

static const unsigned long LCG_A = 6364136223846793005UL;
static const unsigned long LCG_C = 1442695040888963407UL;

Rng::Rng() : _state(static_cast<unsigned long>(std::time(0))) {}
Rng::Rng(unsigned long seed) : _state(seed) {}
Rng::Rng(const Rng& o) : _state(o._state) {}
Rng& Rng::operator=(const Rng& o) { if (this != &o) _state = o._state; return *this; }
Rng::~Rng() {}

unsigned long Rng::next()
{
	_state = _state * LCG_A + LCG_C;
	return _state;
}

int Rng::next_int(int lo, int hi)
{
	if (lo >= hi) return lo;
	unsigned long r = next();
	return lo + static_cast<int>(r % static_cast<unsigned long>(hi - lo + 1));
}

double Rng::next_double()
{
	return static_cast<double>(next()) / static_cast<double>(static_cast<unsigned long>(-1));
}

bool Rng::next_bool() { return (next() & 1) != 0; }

char Rng::next_char(char lo, char hi)
{
	return static_cast<char>(next_int(lo, hi));
}

void Rng::seed(unsigned long s) { _state = s; }

/* ── Generator ─────────────────────────────────────────────────────────── */

Generator::Generator() {}
Generator::Generator(unsigned long seed) : _rng(seed) {}
Generator::Generator(const Generator& o) : _rng(o._rng) {}
Generator& Generator::operator=(const Generator& o) { if (this != &o) _rng = o._rng; return *this; }
Generator::~Generator() {}

int    Generator::gen_int(int lo, int hi) { return _rng.next_int(lo, hi); }
double Generator::gen_double(double lo, double hi) { return lo + _rng.next_double() * (hi - lo); }
bool   Generator::gen_bool() { return _rng.next_bool(); }
char   Generator::gen_char() { return _rng.next_char(32, 126); }
char   Generator::gen_alpha() { return _rng.next_bool() ? _rng.next_char('a', 'z') : _rng.next_char('A', 'Z'); }
char   Generator::gen_digit() { return _rng.next_char('0', '9'); }

std::string Generator::gen_string(int min_len, int max_len)
{
	int len = _rng.next_int(min_len, max_len);
	std::string s;
	s.reserve(len);
	for (int i = 0; i < len; ++i) s += gen_char();
	return s;
}

std::string Generator::gen_alpha_string(int min_len, int max_len)
{
	int len = _rng.next_int(min_len, max_len);
	std::string s;
	s.reserve(len);
	for (int i = 0; i < len; ++i) s += gen_alpha();
	return s;
}

std::string Generator::gen_numeric_string(int len)
{
	std::string s;
	s.reserve(len);
	for (int i = 0; i < len; ++i) s += gen_digit();
	return s;
}

std::string Generator::gen_hex_string(int len)
{
	static const char hex[] = "0123456789abcdef";
	std::string s;
	s.reserve(len);
	for (int i = 0; i < len; ++i) s += hex[_rng.next_int(0, 15)];
	return s;
}

int Generator::gen_int_array(int* out, int count, int lo, int hi)
{
	for (int i = 0; i < count; ++i) out[i] = _rng.next_int(lo, hi);
	return count;
}

std::string Generator::gen_email()
{
	std::string user = gen_alpha_string(4, 10);
	static const char* domains[] = { "example.com", "test.org", "mail.net", "dummy.io" };
	return user + "@" + domains[_rng.next_int(0, 3)];
}

std::string Generator::gen_uuid()
{
	std::string h = gen_hex_string(32);
	/* 8-4-4-4-12 */
	return h.substr(0, 8) + "-" + h.substr(8, 4) + "-" +
		   h.substr(12, 4) + "-" + h.substr(16, 4) + "-" + h.substr(20, 12);
}

Rng& Generator::rng() { return _rng; }

/* ── FuzzResult ────────────────────────────────────────────────────────── */

FuzzResult::FuzzResult() : iterations(0), failures(0) {}
FuzzResult::FuzzResult(const FuzzResult& o)
	: iterations(o.iterations), failures(o.failures), first_failure(o.first_failure) {}
FuzzResult& FuzzResult::operator=(const FuzzResult& o)
{
	if (this != &o)
	{
		iterations = o.iterations; failures = o.failures;
		first_failure = o.first_failure;
	}
	return *this;
}
FuzzResult::~FuzzResult() {}

/* ── FuzzRunner ────────────────────────────────────────────────────────── */

FuzzRunner::FuzzRunner() : _iterations(100), _seed(static_cast<unsigned long>(std::time(0))) {}
FuzzRunner::FuzzRunner(const FuzzRunner& o) : _iterations(o._iterations), _seed(o._seed) {}
FuzzRunner& FuzzRunner::operator=(const FuzzRunner& o)
{
	if (this != &o) { _iterations = o._iterations; _seed = o._seed; }
	return *this;
}
FuzzRunner::~FuzzRunner() {}

FuzzRunner& FuzzRunner::iterations(int n) { _iterations = n; return *this; }
FuzzRunner& FuzzRunner::seed(unsigned long s) { _seed = s; return *this; }

FuzzResult FuzzRunner::run(FuzzPropertyFn prop)
{
	FuzzResult res;
	Generator gen(_seed);

	for (int i = 0; i < _iterations; ++i)
	{
		bool ok = false;
		try { ok = prop(gen); }
		catch (...) { ok = false; }

		++res.iterations;
		if (!ok)
		{
			++res.failures;
			if (res.first_failure.empty())
			{
				std::ostringstream oss;
				oss << "iteration " << (i + 1);
				res.first_failure = oss.str();
			}
		}
	}
	return res;
}

} /* namespace test */
} /* namespace libcpp */
