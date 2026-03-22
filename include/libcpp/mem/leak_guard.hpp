/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   leak_guard.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 22:06:05 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_MEM_LEAK_GUARD_HPP
#define LIBCPP_MEM_LEAK_GUARD_HPP

#include <cstddef>
#include <string>

#define LG_MAX_ALLOCS 4096

namespace libcpp {
namespace mem {

/* ── LgRecord — one slot in the allocation ledger ──────────────────────── */

struct LgRecord {
  void* ptr;
  std::size_t size;
  int seq_id;
  bool active;
};

/* ── LeakGuard — static runtime allocation tracker ─────────────────────── */
/*
** Intercepts global operator new/delete to track heap allocations.
** All state is static; no heap use by LeakGuard itself.
**
** Usage:
**   LeakGuard::enable();
**   LeakGuard::snapshot();
**   // ... code under test ...
**   int leaks = LeakGuard::count_leaks_since_snapshot();
**   LeakGuard::report_leaks();
**   LeakGuard::disable();
*/

class LeakGuard {
 public:
  /* ── lifecycle ─────────────────────────────────────────── */
  static void enable();
  static void disable();
  static bool is_enabled();

  /* ── allocation ledger ─────────────────────────────────── */
  static void record_new(void* ptr, std::size_t size);
  static int record_delete(void* ptr);

  /* ── snapshot / diff ───────────────────────────────────── */
  static void snapshot();
  static int count_leaks_since_snapshot();
  static int net_balance_since_snapshot();
  static int count_all_leaks();
  static std::size_t leaked_bytes();
  static std::size_t leaked_bytes_since_snapshot();

  /* ── diagnostics ───────────────────────────────────────── */
  static int total_new_calls();
  static int total_delete_calls();
  static int double_free_count();
  static int wild_free_count();
  static std::size_t peak_memory();
  static std::size_t current_memory();
  static void report_leaks();
  static void report_leaks_since_snapshot();
  static void reset();

 private:
  LeakGuard();
  ~LeakGuard();
  LeakGuard(const LeakGuard&);
  LeakGuard& operator=(const LeakGuard&);

  static LgRecord _ledger[LG_MAX_ALLOCS];
  static int _count;
  static int _seq_counter;
  static int _snapshot_seq;
  static int _snapshot_new;
  static int _snapshot_del;
  static bool _enabled;
  static bool _overflow_warned;
  static int _total_new;
  static int _total_delete;
  static int _double_frees;
  static int _wild_frees;
  static std::size_t _current_mem;
  static std::size_t _peak_mem;

  static int find_slot(void* ptr);
  static int free_slot();
};

} /* namespace mem */
} /* namespace libcpp */

#endif /* LIBCPP_MEM_LEAK_GUARD_HPP */
