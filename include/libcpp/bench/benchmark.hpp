/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   benchmark.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:47:02 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_BENCH_BENCHMARK_HPP
# define LIBCPP_BENCH_BENCHMARK_HPP

# include <string>
# include <ctime>
# include <iostream>

namespace libcpp
{
namespace bench
{

/* ── BenchFn — function pointer type for benchmarks ────────────────────── */

typedef void (*BenchFn)();

/* ── BenchResult — stats from a single benchmark ───────────────────────── */

struct BenchResult
{
	std::string name;
	int         iterations;
	double      total_ms;
	double      mean_ms;
	double      min_ms;
	double      max_ms;
	double      ops_per_sec;

	BenchResult();
	BenchResult(const BenchResult& o);
	BenchResult& operator=(const BenchResult& o);
	~BenchResult();
};

/* ── BenchCase — registered benchmark ──────────────────────────────────── */

struct BenchCase
{
	std::string name;
	BenchFn     fn;

	BenchCase();
	BenchCase(const std::string& n, BenchFn f);
	BenchCase(const BenchCase& o);
	BenchCase& operator=(const BenchCase& o);
	~BenchCase();
};

/* ── Benchmark — main runner ───────────────────────────────────────────── */

class Benchmark
{
public:
	static const int MAX_CASES   = 64;
	static const int MAX_RESULTS = 64;

	Benchmark();
	Benchmark(const std::string& name);
	Benchmark(const Benchmark& o);
	Benchmark& operator=(const Benchmark& o);
	~Benchmark();

	/* Registration */
	Benchmark& add(const std::string& name, BenchFn fn);

	/* Configuration */
	Benchmark& warmup(int n);
	Benchmark& iterations(int n);

	/* Run all benchmarks and print results */
	void run();

	/* Access results */
	int                 result_count() const;
	const BenchResult&  result(int i) const;
	const std::string&  name() const;
	const BenchResult*  fastest() const;
	const BenchResult*  slowest() const;

private:
	std::string _name;
	BenchCase   _cases[MAX_CASES];
	int         _case_count;
	BenchResult _results[MAX_RESULTS];
	int         _result_count;
	int         _warmup;
	int         _iterations;

	BenchResult _run_one(const BenchCase& bc);
	void        _print_header() const;
	void        _print_result(const BenchResult& r) const;
	void        _print_summary() const;
};

} /* namespace bench */
} /* namespace libcpp */

#endif /* LIBCPP_BENCH_BENCHMARK_HPP */
