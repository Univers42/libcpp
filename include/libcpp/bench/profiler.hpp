/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   profiler.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:45:01 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_BENCH_PROFILER_HPP
#define LIBCPP_BENCH_PROFILER_HPP

#include <ctime>
#include <iostream>
#include <string>

namespace libcpp {
namespace bench {

/* ── ProfileEntry — accumulated stats for a named section ──────────────── */

struct ProfileEntry {
  std::string name;
  int call_count;
  double total_ms;
  double min_ms;
  double max_ms;

  ProfileEntry();
  ProfileEntry(const std::string& n);
  ProfileEntry(const ProfileEntry& o);
  ProfileEntry& operator=(const ProfileEntry& o);
  ~ProfileEntry();

  double mean_ms() const;
};

/* ── Profiler — global function-level profiler ─────────────────────────── */

class Profiler {
 public:
  static const int MAX_ENTRIES = 256;

  /* Singleton access */
  static Profiler& instance();

  /* Record a measurement */
  void record(const std::string& name, double elapsed_ms);

  /* Print report to stdout */
  void report() const;

  /* Reset all data */
  void reset();

  /* Access entries */
  int entry_count() const;
  const ProfileEntry& entry(int i) const;
  const ProfileEntry* find(const std::string& name) const;

 private:
  ProfileEntry _entries[MAX_ENTRIES];
  int _count;

  Profiler();
  Profiler(const Profiler&);
  Profiler& operator=(const Profiler&);
  ~Profiler();

  int _find(const std::string& name) const;
};

/* ── ProfileScope — RAII profiling, records on destruction ─────────────── */

class ProfileScope {
 public:
  ProfileScope(const std::string& name);
  ProfileScope(const ProfileScope& o);
  ProfileScope& operator=(const ProfileScope& o);
  ~ProfileScope();

 private:
  std::string _name;
  std::clock_t _start;
};

/* ── Convenience macros ────────────────────────────────────────────────── */

#define PROFILE_SCOPE(name) \
  libcpp::bench::ProfileScope _libcpp_ps_##__LINE__(name)

#define PROFILE_FUNCTION() libcpp::bench::ProfileScope _libcpp_pf_(__FUNCTION__)

} /* namespace bench */
} /* namespace libcpp */

#endif /* LIBCPP_BENCH_PROFILER_HPP */
