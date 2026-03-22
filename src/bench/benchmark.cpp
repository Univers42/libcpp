/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   benchmark.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:47:02 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/bench/benchmark.hpp"
#include <iomanip>
#include <sstream>
#include "libcpp/term/color.hpp"
#include "libcpp/term/style.hpp"

namespace libcpp {
namespace bench {

/* ── BenchResult ───────────────────────────────────────────────────────── */

BenchResult::BenchResult()
    : iterations(0),
      total_ms(0),
      mean_ms(0),
      min_ms(0),
      max_ms(0),
      ops_per_sec(0) {}

BenchResult::BenchResult(const BenchResult& o)
    : name(o.name),
      iterations(o.iterations),
      total_ms(o.total_ms),
      mean_ms(o.mean_ms),
      min_ms(o.min_ms),
      max_ms(o.max_ms),
      ops_per_sec(o.ops_per_sec) {}

BenchResult& BenchResult::operator=(const BenchResult& o) {
  if (this != &o) {
    name = o.name;
    iterations = o.iterations;
    total_ms = o.total_ms;
    mean_ms = o.mean_ms;
    min_ms = o.min_ms;
    max_ms = o.max_ms;
    ops_per_sec = o.ops_per_sec;
  }
  return *this;
}

BenchResult::~BenchResult() {}

/* ── BenchCase ─────────────────────────────────────────────────────────── */

BenchCase::BenchCase() : fn(0) {}
BenchCase::BenchCase(const std::string& n, BenchFn f) : name(n), fn(f) {}
BenchCase::BenchCase(const BenchCase& o) : name(o.name), fn(o.fn) {}
BenchCase& BenchCase::operator=(const BenchCase& o) {
  if (this != &o) {
    name = o.name;
    fn = o.fn;
  }
  return *this;
}
BenchCase::~BenchCase() {}

/* ── Benchmark ─────────────────────────────────────────────────────────── */

Benchmark::Benchmark()
    : _name("Benchmark"),
      _case_count(0),
      _result_count(0),
      _warmup(3),
      _iterations(100) {}

Benchmark::Benchmark(const std::string& name)
    : _name(name),
      _case_count(0),
      _result_count(0),
      _warmup(3),
      _iterations(100) {}

Benchmark::Benchmark(const Benchmark& o)
    : _name(o._name),
      _case_count(o._case_count),
      _result_count(o._result_count),
      _warmup(o._warmup),
      _iterations(o._iterations) {
  for (int i = 0; i < _case_count; ++i) _cases[i] = o._cases[i];
  for (int i = 0; i < _result_count; ++i) _results[i] = o._results[i];
}

Benchmark& Benchmark::operator=(const Benchmark& o) {
  if (this != &o) {
    _name = o._name;
    _case_count = o._case_count;
    _result_count = o._result_count;
    _warmup = o._warmup;
    _iterations = o._iterations;
    for (int i = 0; i < _case_count; ++i) _cases[i] = o._cases[i];
    for (int i = 0; i < _result_count; ++i) _results[i] = o._results[i];
  }
  return *this;
}

Benchmark::~Benchmark() {}

Benchmark& Benchmark::add(const std::string& name, BenchFn fn) {
  if (_case_count < MAX_CASES) _cases[_case_count++] = BenchCase(name, fn);
  return *this;
}

Benchmark& Benchmark::warmup(int n) {
  _warmup = n;
  return *this;
}
Benchmark& Benchmark::iterations(int n) {
  _iterations = n;
  return *this;
}

void Benchmark::run() {
  _result_count = 0;
  _print_header();

  for (int i = 0; i < _case_count; ++i) {
    BenchResult r = _run_one(_cases[i]);
    _print_result(r);
    if (_result_count < MAX_RESULTS) _results[_result_count++] = r;
  }

  _print_summary();
}

int Benchmark::result_count() const { return _result_count; }
const BenchResult& Benchmark::result(int i) const { return _results[i]; }
const std::string& Benchmark::name() const { return _name; }

const BenchResult* Benchmark::fastest() const {
  if (_result_count == 0) return 0;
  const BenchResult* best = &_results[0];
  for (int i = 1; i < _result_count; ++i)
    if (_results[i].mean_ms < best->mean_ms) best = &_results[i];
  return best;
}

const BenchResult* Benchmark::slowest() const {
  if (_result_count == 0) return 0;
  const BenchResult* worst = &_results[0];
  for (int i = 1; i < _result_count; ++i)
    if (_results[i].mean_ms > worst->mean_ms) worst = &_results[i];
  return worst;
}

BenchResult Benchmark::_run_one(const BenchCase& bc) {
  BenchResult r;
  r.name = bc.name;
  r.iterations = _iterations;

  /* Warmup */
  for (int w = 0; w < _warmup; ++w) bc.fn();

  double total = 0.0;
  double mn = 1e18;
  double mx = 0.0;

  for (int i = 0; i < _iterations; ++i) {
    std::clock_t t0 = std::clock();
    bc.fn();
    std::clock_t t1 = std::clock();
    double ms = 1000.0 * static_cast<double>(t1 - t0) / CLOCKS_PER_SEC;
    total += ms;
    if (ms < mn) mn = ms;
    if (ms > mx) mx = ms;
  }

  r.total_ms = total;
  r.mean_ms = total / _iterations;
  r.min_ms = mn;
  r.max_ms = mx;
  r.ops_per_sec = (r.mean_ms > 0) ? (1000.0 / r.mean_ms) : 0;

  return r;
}

void Benchmark::_print_header() const {
  Srgb cyan(0, 200, 255);
  std::cout << "\n"
            << cyan.to_ansi_fg() << "━━━ " << _name << " ━━━"
            << TermUtils::reset() << "\n"
            << std::endl;
}

void Benchmark::_print_result(const BenchResult& r) const {
  Srgb green(0, 220, 0);
  Srgb dim(128, 128, 128);

  std::ostringstream oss;
  oss << std::fixed << std::setprecision(3);
  oss << "  " << green.to_ansi_fg() << r.name << TermUtils::reset()
      << "  mean=" << r.mean_ms << "ms"
      << "  min=" << r.min_ms << "ms"
      << "  max=" << r.max_ms << "ms"
      << "  " << dim.to_ansi_fg() << "(" << r.iterations << " iters)"
      << TermUtils::reset();
  std::cout << oss.str() << std::endl;
}

void Benchmark::_print_summary() const {
  Srgb dim(128, 128, 128);
  std::cout << "\n"
            << dim.to_ansi_fg() << "───────────────────────────────────"
            << TermUtils::reset() << std::endl;
  std::cout << "  " << _result_count << " benchmark(s) completed\n"
            << std::endl;
}

} /* namespace bench */
} /* namespace libcpp */
