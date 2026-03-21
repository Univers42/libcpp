/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fuzzer.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 20:53:31 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_TEST_FUZZER_HPP
# define LIBCPP_TEST_FUZZER_HPP

# include <string>
# include <cstdlib>
# include <ctime>
# include <sstream>

namespace libcpp
{
namespace test
{

/* ── Rng — simple linear congruential PRNG ─────────────────────────────── */

class Rng
{
public:
	Rng();
	Rng(unsigned long seed);
	Rng(const Rng& o);
	Rng& operator=(const Rng& o);
	~Rng();

	unsigned long next();
	int           next_int(int lo, int hi);
	double        next_double();
	bool          next_bool();
	char          next_char(char lo, char hi);

	void          seed(unsigned long s);

private:
	unsigned long _state;
};

/* ── Generator — produces random test data ─────────────────────────────── */

class Generator
{
public:
	Generator();
	Generator(unsigned long seed);
	Generator(const Generator& o);
	Generator& operator=(const Generator& o);
	~Generator();

	/* Primitives */
	int         gen_int(int lo, int hi);
	double      gen_double(double lo, double hi);
	bool        gen_bool();
	char        gen_char();
	char        gen_alpha();
	char        gen_digit();

	/* Strings */
	std::string gen_string(int min_len, int max_len);
	std::string gen_alpha_string(int min_len, int max_len);
	std::string gen_numeric_string(int len);
	std::string gen_hex_string(int len);

	/* Containers (writes into caller's buffer) */
	int         gen_int_array(int* out, int count, int lo, int hi);

	/* Access to the underlying RNG */
	Rng& rng();

private:
	Rng _rng;
};

/* ── FuzzRunner — runs a property test multiple iterations ─────────────── */

typedef bool (*FuzzPropertyFn)(Generator&);

struct FuzzResult
{
	int         iterations;
	int         failures;
	std::string first_failure;

	FuzzResult();
	FuzzResult(const FuzzResult& o);
	FuzzResult& operator=(const FuzzResult& o);
	~FuzzResult();
};

class FuzzRunner
{
public:
	FuzzRunner();
	FuzzRunner(const FuzzRunner& o);
	FuzzRunner& operator=(const FuzzRunner& o);
	~FuzzRunner();

	FuzzRunner& iterations(int n);
	FuzzRunner& seed(unsigned long s);
	FuzzResult  run(FuzzPropertyFn prop);

private:
	int           _iterations;
	unsigned long _seed;
};

/* ── Fuzzer assertion macro ────────────────────────────────────────────── */

# define ASSERT_FUZZ(suite, runner, prop) \
	do { \
		FuzzResult _fr = (runner).run(prop); \
		if (_fr.failures > 0) { \
			std::ostringstream _oss; \
			_oss << "fuzzer failed after " << _fr.failures << " failure(s): " \
				 << _fr.first_failure; \
			(suite).assert_true(false, _oss.str().c_str(), __FILE__, __LINE__); \
		} \
	} while (0)

} /* namespace test */
} /* namespace libcpp */

#endif /* LIBCPP_TEST_FUZZER_HPP */
