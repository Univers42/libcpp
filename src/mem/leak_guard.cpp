/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   leak_guard.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 22:23:30 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/mem/leak_guard.hpp"
#include <cstdio>
#include <cstdlib>
#include <new>

namespace libcpp {
namespace mem {

/* ── Static member initialisation ──────────────────────────────────────── */

LgRecord LeakGuard::_ledger[LG_MAX_ALLOCS];
int LeakGuard::_count = 0;
int LeakGuard::_seq_counter = 0;
int LeakGuard::_snapshot_seq = 0;
int LeakGuard::_snapshot_new = 0;
int LeakGuard::_snapshot_del = 0;
bool LeakGuard::_enabled = false;
bool LeakGuard::_overflow_warned = false;
int LeakGuard::_total_new = 0;
int LeakGuard::_total_delete = 0;
int LeakGuard::_double_frees = 0;
int LeakGuard::_wild_frees = 0;
std::size_t LeakGuard::_current_mem = 0;
std::size_t LeakGuard::_peak_mem = 0;

/* ── Lifecycle ─────────────────────────────────────────────────────────── */

void LeakGuard::enable() { _enabled = true; }
void LeakGuard::disable() { _enabled = false; }
bool LeakGuard::is_enabled() { return _enabled; }

void LeakGuard::reset() {
  for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
    _ledger[i].ptr = 0;
    _ledger[i].size = 0;
    _ledger[i].seq_id = 0;
    _ledger[i].active = false;
  }
  _count = 0;
  _seq_counter = 0;
  _snapshot_seq = 0;
  _snapshot_new = 0;
  _snapshot_del = 0;
  _overflow_warned = false;
  _total_new = 0;
  _total_delete = 0;
  _double_frees = 0;
  _wild_frees = 0;
  _current_mem = 0;
  _peak_mem = 0;
}

/* ── Ledger helpers ────────────────────────────────────────────────────── */

int LeakGuard::find_slot(void* ptr) {
  for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
    if (_ledger[i].ptr == ptr && _ledger[i].active) return i;
  }
  for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
    if (_ledger[i].ptr == ptr && !_ledger[i].active && _ledger[i].seq_id > 0)
      return -(i + 1);
  }
  return -1;
}

int LeakGuard::free_slot() {
  for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
    if (!_ledger[i].active && _ledger[i].seq_id == 0) return i;
  }
  for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
    if (!_ledger[i].active) return i;
  }
  return -1;
}

/* ── Record new / delete ───────────────────────────────────────────────── */

void LeakGuard::record_new(void* ptr, std::size_t size) {
  if (!_enabled || !ptr) return;
  ++_total_new;
  ++_seq_counter;

  int idx = free_slot();
  if (idx < 0) {
    if (!_overflow_warned) {
      std::fprintf(stderr,
                   "\033[38;5;210m[LeakGuard]\033[0m ledger full(%d slots).\n",
                   LG_MAX_ALLOCS);
      _overflow_warned = true;
    }
    return;
  }
  _ledger[idx].ptr = ptr;
  _ledger[idx].size = size;
  _ledger[idx].seq_id = _seq_counter;
  _ledger[idx].active = true;
  ++_count;
  _current_mem += size;
  if (_current_mem > _peak_mem) _peak_mem = _current_mem;
}

int LeakGuard::record_delete(void* ptr) {
  if (!_enabled || !ptr) return 0;
  ++_total_delete;

  int raw = find_slot(ptr);

  if (raw >= 0) {
    _current_mem -= _ledger[raw].size;
    _ledger[raw].active = false;
    --_count;
    return 0;
  }

  if (raw < -1) {
    ++_double_frees;
    std::fprintf(stderr,
                 "\033[38;5;210m[LeakGuard]\033[0m \033[1mDOUBLE FREE\033[0m "
                 "ptr=%p(alloc #%d, %zu bytes)\n",
                 ptr, _ledger[-(raw + 1)].seq_id, _ledger[-(raw + 1)].size);
    return 1;
  }

  ++_wild_frees;
  return 2;
}

/* ── Snapshot / diff ───────────────────────────────────────────────────── */

void LeakGuard::snapshot() {
  _snapshot_seq = _seq_counter;
  _snapshot_new = _total_new;
  _snapshot_del = _total_delete;
}

int LeakGuard::count_leaks_since_snapshot() {
  int n = 0;
  for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
    if (_ledger[i].active && _ledger[i].seq_id > _snapshot_seq) ++n;
  }
  return n;
}

int LeakGuard::net_balance_since_snapshot() {
  return (_total_new - _snapshot_new) - (_total_delete - _snapshot_del);
}

int LeakGuard::count_all_leaks() {
  int n = 0;
  for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
    if (_ledger[i].active) ++n;
  }
  return n;
}

std::size_t LeakGuard::leaked_bytes() {
  std::size_t total = 0;
  for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
    if (_ledger[i].active) total += _ledger[i].size;
  }
  return total;
}

std::size_t LeakGuard::leaked_bytes_since_snapshot() {
  std::size_t total = 0;
  for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
    if (_ledger[i].active && _ledger[i].seq_id > _snapshot_seq)
      total += _ledger[i].size;
  }
  return total;
}

/* ── Diagnostics ───────────────────────────────────────────────────────── */

int LeakGuard::total_new_calls() { return _total_new; }
int LeakGuard::total_delete_calls() { return _total_delete; }
int LeakGuard::double_free_count() { return _double_frees; }
int LeakGuard::wild_free_count() { return _wild_frees; }
std::size_t LeakGuard::peak_memory() { return _peak_mem; }
std::size_t LeakGuard::current_memory() { return _current_mem; }

static void _print_record(const LgRecord& r) {
  std::fprintf(stderr,
               "    alloc \033[1m#%d\033[0m  ptr=\033[38;5;141m%p\033[0m  "
               "size=\033[38;5;183m%zu\033[0m bytes\n",
               r.seq_id, r.ptr, r.size);
}

void LeakGuard::report_leaks() {
  int leaks = count_all_leaks();
  if (leaks == 0) {
    std::fprintf(stderr,
                 "\033[38;5;114m[LeakGuard]\033[0m "
                 "\033[1mNo leaks detected.\033[0m  "
                 "(new=%d  delete=%d)\n",
                 _total_new, _total_delete);
    return;
  }
  std::fprintf(stderr,
               "\n\033[38;5;210m[LeakGuard]\033[0m "
               "\033[1m%d leaked allocation(s)\033[0m  "
               "(%zu bytes):\n",
               leaks, leaked_bytes());
  for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
    if (_ledger[i].active) _print_record(_ledger[i]);
  }
  std::fprintf(
      stderr,
      "  \033[2m(new=%d  delete=%d  double-free=%d  wild-free=%d)\033[0m\n\n",
      _total_new, _total_delete, _double_frees, _wild_frees);
}

void LeakGuard::report_leaks_since_snapshot() {
  int leaks = count_leaks_since_snapshot();
  if (leaks == 0) {
    std::fprintf(stderr,
                 "\033[38;5;114m[LeakGuard]\033[0m "
                 "\033[1mNo leaks since snapshot.\033[0m  "
                 "(new=%d  delete=%d  snapshot@#%d)\n",
                 _total_new, _total_delete, _snapshot_seq);
    return;
  }
  std::fprintf(stderr,
               "\n\033[38;5;210m[LeakGuard]\033[0m "
               "\033[1m%d leaked allocation(s) since snapshot #%d\033[0m  "
               "(%zu bytes):\n",
               leaks, _snapshot_seq, leaked_bytes_since_snapshot());
  for (int i = 0; i < LG_MAX_ALLOCS; ++i) {
    if (_ledger[i].active && _ledger[i].seq_id > _snapshot_seq)
      _print_record(_ledger[i]);
  }
  std::fprintf(
      stderr,
      "  \033[2m(new=%d  delete=%d  double-free=%d  wild-free=%d)\033[0m\n\n",
      _total_new, _total_delete, _double_frees, _wild_frees);
}

} /* namespace mem */
} /* namespace libcpp */

/* ── Global operator new/delete overrides ──────────────────────────────── */

void* operator new(std::size_t size) {
  void* ptr = std::malloc(size);
  if (!ptr) throw std::bad_alloc();
  libcpp::mem::LeakGuard::record_new(ptr, size);
  return ptr;
}

void operator delete(void* ptr) noexcept {
  if (!ptr) return;
  libcpp::mem::LeakGuard::record_delete(ptr);
  std::free(ptr);
}

void operator delete(void* ptr, std::size_t) noexcept {
  if (!ptr) return;
  libcpp::mem::LeakGuard::record_delete(ptr);
  std::free(ptr);
}

void* operator new[](std::size_t size) {
  void* ptr = std::malloc(size);
  if (!ptr) throw std::bad_alloc();
  libcpp::mem::LeakGuard::record_new(ptr, size);
  return ptr;
}

void operator delete[](void* ptr) noexcept {
  if (!ptr) return;
  libcpp::mem::LeakGuard::record_delete(ptr);
  std::free(ptr);
}

void operator delete[](void* ptr, std::size_t) noexcept {
  if (!ptr) return;
  libcpp::mem::LeakGuard::record_delete(ptr);
  std::free(ptr);
}
