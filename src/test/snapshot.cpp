/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   snapshot.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:28:32 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/test/snapshot.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <algorithm>
#include <cstdlib>
#include <cstring>

namespace libcpp {
namespace test {

/* ── DiffLine ──────────────────────────────────────────────────────────── */

DiffLine::DiffLine() : kind(KEEP) {}
DiffLine::DiffLine(Kind k, const std::string& t) : kind(k), text(t) {}
DiffLine::DiffLine(const DiffLine& o) : kind(o.kind), text(o.text) {}
DiffLine& DiffLine::operator=(const DiffLine& o) {
  if (this != &o) {
    kind = o.kind;
    text = o.text;
  }
  return *this;
}
DiffLine::~DiffLine() {}

/* ── Diff ──────────────────────────────────────────────────────────────── */

Diff::Diff() : count(0), equal(true) {}
Diff::Diff(const Diff& o) : count(o.count), equal(o.equal) {
  for (int i = 0; i < count; ++i) lines[i] = o.lines[i];
}
Diff& Diff::operator=(const Diff& o) {
  if (this != &o) {
    count = o.count;
    equal = o.equal;
    for (int i = 0; i < count; ++i) lines[i] = o.lines[i];
  }
  return *this;
}
Diff::~Diff() {}

std::string Diff::to_string() const {
  std::ostringstream oss;
  for (int i = 0; i < count; ++i) {
    switch (lines[i].kind) {
      case DiffLine::KEEP:
        oss << "  ";
        break;
      case DiffLine::ADD:
        oss << "+ ";
        break;
      case DiffLine::DEL:
        oss << "- ";
        break;
    }
    oss << lines[i].text << "\n";
  }
  return oss.str();
}

/* ── Split helper ──────────────────────────────────────────────────────── */

static int _split_lines(const std::string& text, std::string* out, int max) {
  int n = 0;
  std::istringstream iss(text);
  std::string line;
  while (n < max && std::getline(iss, line)) out[n++] = line;
  return n;
}

/* ── Myers diff(simplified O(ND) algorithm) ──────────────────────────── */

Diff myers_diff(const std::string& old_text, const std::string& new_text) {
  static const int MAX_SRC = 2048;

  std::string old_lines[MAX_SRC];
  std::string new_lines[MAX_SRC];

  int N = _split_lines(old_text, old_lines, MAX_SRC);
  int M = _split_lines(new_text, new_lines, MAX_SRC);

  Diff result;

  /* Simple LCS-based diff for C++98(no VLA, fixed alloc) */
  /* For very large inputs, fall back to line-by-line comparison */
  if (N + M > MAX_SRC) {
    /* fallback: show all old as DEL, all new as ADD */
    for (int i = 0; i < N && result.count < Diff::MAX_LINES; ++i)
      result.lines[result.count++] = DiffLine(DiffLine::DEL, old_lines[i]);
    for (int i = 0; i < M && result.count < Diff::MAX_LINES; ++i)
      result.lines[result.count++] = DiffLine(DiffLine::ADD, new_lines[i]);
    result.equal = (N == 0 && M == 0);
    return result;
  }

  /* LCS table using dynamic allocation(C++98 new[]) */
  int rows = N + 1;
  int cols = M + 1;
  int* dp = new int[rows * cols];
  std::memset(dp, 0, sizeof(int) * rows * cols);

  for (int i = 1; i <= N; ++i) {
    for (int j = 1; j <= M; ++j) {
      if (old_lines[i - 1] == new_lines[j - 1])
        dp[i * cols + j] = dp[(i - 1) * cols + (j - 1)] + 1;
      else {
        int a = dp[(i - 1) * cols + j];
        int b = dp[i * cols + (j - 1)];
        dp[i * cols + j] = (a > b) ? a : b;
      }
    }
  }

  /* Backtrack to build diff */
  static const int MAX_DIFF = Diff::MAX_LINES;
  DiffLine tmp[MAX_DIFF];
  int tc = 0;

  int i = N;
  int j = M;
  while ((i > 0 || j > 0) && tc < MAX_DIFF) {
    if (i > 0 && j > 0 && old_lines[i - 1] == new_lines[j - 1]) {
      tmp[tc++] = DiffLine(DiffLine::KEEP, old_lines[i - 1]);
      --i;
      --j;
    } else if (j > 0 &&
               (i == 0 || dp[i * cols + (j - 1)] >= dp[(i - 1) * cols + j])) {
      tmp[tc++] = DiffLine(DiffLine::ADD, new_lines[j - 1]);
      --j;
    } else {
      tmp[tc++] = DiffLine(DiffLine::DEL, old_lines[i - 1]);
      --i;
    }
  }

  delete[] dp;

  /* Reverse into result */
  result.equal = true;
  for (int k = tc - 1; k >= 0 && result.count < MAX_DIFF; --k) {
    result.lines[result.count++] = tmp[k];
    if (tmp[k].kind != DiffLine::KEEP) result.equal = false;
  }

  return result;
}

/* ── Snapshot ──────────────────────────────────────────────────────────── */

Snapshot::Snapshot()
    : _snap_dir("__snapshots__"), _update(false), _strip_ansi(false) {}
Snapshot::Snapshot(const std::string& snap_dir)
    : _snap_dir(snap_dir), _update(false), _strip_ansi(false) {}
Snapshot::Snapshot(const Snapshot& o)
    : _snap_dir(o._snap_dir),
      _last_diff(o._last_diff),
      _update(o._update),
      _strip_ansi(o._strip_ansi) {}
Snapshot& Snapshot::operator=(const Snapshot& o) {
  if (this != &o) {
    _snap_dir = o._snap_dir;
    _last_diff = o._last_diff;
    _update = o._update;
    _strip_ansi = o._strip_ansi;
  }
  return *this;
}
Snapshot::~Snapshot() {}

static std::string _strip_ansi_codes(const std::string& s) {
  std::string out;
  bool in_esc = false;
  for (std::size_t i = 0; i < s.size(); ++i) {
    if (in_esc) {
      if (s[i] == 'm') in_esc = false;
      continue;
    }
    if (static_cast<unsigned char>(s[i]) == 0x1B && i + 1 < s.size() &&
        s[i + 1] == '[') {
      in_esc = true;
      ++i;
      continue;
    }
    out += s[i];
  }
  return out;
}

bool Snapshot::match(const std::string& name, const std::string& actual) {
  std::string p = _path(name);
  std::string existing = _read_file(p);
  std::string cmp_actual = _strip_ansi ? _strip_ansi_codes(actual) : actual;
  std::string cmp_existing =
      _strip_ansi ? _strip_ansi_codes(existing) : existing;

  if (cmp_existing.empty()) {
    /* No snapshot — create it */
    _write_file(p, cmp_actual);
    _last_diff = Diff();
    return true;
  }

  if (cmp_existing == cmp_actual) {
    _last_diff = Diff();
    return true;
  }

  /* Mismatch */
  _last_diff = myers_diff(cmp_existing, cmp_actual);

  if (_update) {
    _write_file(p, cmp_actual);
    return true;
  }
  return false;
}

const Diff& Snapshot::last_diff() const { return _last_diff; }
void Snapshot::set_update(bool on) { _update = on; }
bool Snapshot::update_mode() const { return _update; }
void Snapshot::set_strip_ansi(bool on) { _strip_ansi = on; }
bool Snapshot::strip_ansi_mode() const { return _strip_ansi; }

std::string Snapshot::_path(const std::string& name) const {
  return _snap_dir + "/" + name + ".snap";
}

std::string Snapshot::_read_file(const std::string& path) const {
  std::ifstream f(path.c_str());
  if (!f.is_open()) return "";
  std::ostringstream oss;
  oss << f.rdbuf();
  return oss.str();
}

void Snapshot::_write_file(const std::string& path,
                           const std::string& data) const {
  /* Ensure directory exists */
  mkdir(_snap_dir.c_str(), 0755);

  std::ofstream f(path.c_str());
  if (f.is_open()) f << data;
}

} /* namespace test */
} /* namespace libcpp */
