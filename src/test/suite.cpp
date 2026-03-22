/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   suite.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:56:48 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/test/suite.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>

namespace libcpp {
namespace test {

/* ── AssertError ───────────────────────────────────────────────────────── */

AssertError::AssertError() : line(0) {}
AssertError::AssertError(const std::string& msg, const std::string& f, int l)
    : message(msg), file(f), line(l) {}
AssertError::AssertError(const AssertError& o)
    : message(o.message), file(o.file), line(o.line) {}
AssertError& AssertError::operator=(const AssertError& o) {
  if (this != &o) {
    message = o.message;
    file = o.file;
    line = o.line;
  }
  return *this;
}
AssertError::~AssertError() {}

/* ── TestCase ──────────────────────────────────────────────────────────── */

TestCase::TestCase() : fn(0), skip(false) {}
TestCase::TestCase(const std::string& n, TestFn f, bool s)
    : name(n), fn(f), skip(s) {}
TestCase::TestCase(const TestCase& o) : name(o.name), fn(o.fn), skip(o.skip) {}
TestCase& TestCase::operator=(const TestCase& o) {
  if (this != &o) {
    name = o.name;
    fn = o.fn;
    skip = o.skip;
  }
  return *this;
}
TestCase::~TestCase() {}

/* ── TestResult ────────────────────────────────────────────────────────── */

TestResult::TestResult() : passed(false), skipped(false), elapsed_ms(0.0) {}
TestResult::TestResult(const TestResult& o)
    : name(o.name),
      passed(o.passed),
      skipped(o.skipped),
      error_msg(o.error_msg),
      elapsed_ms(o.elapsed_ms) {}
TestResult& TestResult::operator=(const TestResult& o) {
  if (this != &o) {
    name = o.name;
    passed = o.passed;
    skipped = o.skipped;
    error_msg = o.error_msg;
    elapsed_ms = o.elapsed_ms;
  }
  return *this;
}
TestResult::~TestResult() {}

/* ── TestSuite ─────────────────────────────────────────────────────────── */

TestSuite::TestSuite()
    : _name("TestSuite"),
      _test_count(0),
      _result_count(0),
      _before_each(0),
      _after_each(0),
      _before_all(0),
      _after_all(0),
      _passed(0),
      _failed(0),
      _skipped(0) {}

TestSuite::TestSuite(const std::string& name)
    : _name(name),
      _test_count(0),
      _result_count(0),
      _before_each(0),
      _after_each(0),
      _before_all(0),
      _after_all(0),
      _passed(0),
      _failed(0),
      _skipped(0) {}

TestSuite::TestSuite(const TestSuite& o)
    : _name(o._name),
      _test_count(o._test_count),
      _result_count(o._result_count),
      _before_each(o._before_each),
      _after_each(o._after_each),
      _before_all(o._before_all),
      _after_all(o._after_all),
      _passed(o._passed),
      _failed(o._failed),
      _skipped(o._skipped) {
  for (int i = 0; i < _test_count; ++i) _tests[i] = o._tests[i];
  for (int i = 0; i < _result_count; ++i) _results[i] = o._results[i];
}

TestSuite& TestSuite::operator=(const TestSuite& o) {
  if (this != &o) {
    _name = o._name;
    _test_count = o._test_count;
    _result_count = o._result_count;
    _before_each = o._before_each;
    _after_each = o._after_each;
    _before_all = o._before_all;
    _after_all = o._after_all;
    _passed = o._passed;
    _failed = o._failed;
    _skipped = o._skipped;
    for (int i = 0; i < _test_count; ++i) _tests[i] = o._tests[i];
    for (int i = 0; i < _result_count; ++i) _results[i] = o._results[i];
  }
  return *this;
}

TestSuite::~TestSuite() {}

/* ── Registration ──────────────────────────────────────────────────────── */

TestSuite& TestSuite::test(const std::string& name, TestFn fn) {
  if (_test_count < MAX_TESTS)
    _tests[_test_count++] = TestCase(name, fn, false);
  return *this;
}

TestSuite& TestSuite::skip(const std::string& name, TestFn fn) {
  if (_test_count < MAX_TESTS) _tests[_test_count++] = TestCase(name, fn, true);
  return *this;
}

TestSuite& TestSuite::before_each(TestFn fn) {
  _before_each = fn;
  return *this;
}
TestSuite& TestSuite::after_each(TestFn fn) {
  _after_each = fn;
  return *this;
}
TestSuite& TestSuite::before_all(TestFn fn) {
  _before_all = fn;
  return *this;
}
TestSuite& TestSuite::after_all(TestFn fn) {
  _after_all = fn;
  return *this;
}

/* ── Run ───────────────────────────────────────────────────────────────── */

int TestSuite::run() {
  _passed = 0;
  _failed = 0;
  _skipped = 0;
  _result_count = 0;

  _print_header();

  if (_before_all) _before_all(*this);

  for (int i = 0; i < _test_count; ++i) run_test(i);

  if (_after_all) _after_all(*this);

  _print_summary();
  return _failed;
}

void TestSuite::run_test(int index) {
  if (index < 0 || index >= _test_count) return;

  TestCase& tc = _tests[index];
  TestResult r;
  r.name = tc.name;

  if (tc.skip) {
    r.skipped = true;
    r.passed = false;
    ++_skipped;
    _print_result(r);
    if (_result_count < MAX_RESULTS) _results[_result_count++] = r;
    return;
  }

  if (_before_each) _before_each(*this);

  std::clock_t start = std::clock();
  try {
    tc.fn(*this);
    r.passed = true;
    ++_passed;
  } catch (const AssertError& e) {
    r.passed = false;
    std::ostringstream oss;
    oss << e.message << " (" << e.file << ":" << e.line << ")";
    r.error_msg = oss.str();
    ++_failed;
  } catch (...) {
    r.passed = false;
    r.error_msg = "unexpected exception";
    ++_failed;
  }
  std::clock_t end = std::clock();
  r.elapsed_ms = 1000.0 * static_cast<double>(end - start) / CLOCKS_PER_SEC;

  if (_after_each) _after_each(*this);

  _print_result(r);
  if (_result_count < MAX_RESULTS) _results[_result_count++] = r;
}

/* ── Assertions ────────────────────────────────────────────────────────── */

void TestSuite::assert_true(bool cond, const char* expr, const char* file,
                            int line) {
  if (!cond) {
    std::string msg = std::string("expected true: ") + expr;
    throw AssertError(msg, file, line);
  }
}

void TestSuite::assert_false(bool cond, const char* expr, const char* file,
                             int line) {
  if (cond) {
    std::string msg = std::string("expected false: ") + expr;
    throw AssertError(msg, file, line);
  }
}

void TestSuite::assert_eq_str(const std::string& a, const std::string& b,
                              const char* file, int line) {
  if (a != b) {
    std::string msg = "expected \"" + a + "\" == \"" + b + "\"";
    throw AssertError(msg, file, line);
  }
}

void TestSuite::assert_eq_int(long a, long b, const char* file, int line) {
  if (a != b) {
    std::ostringstream oss;
    oss << "expected " << a << " == " << b;
    throw AssertError(oss.str(), file, line);
  }
}

void TestSuite::assert_eq_double(double a, double b, double eps,
                                 const char* file, int line) {
  if (std::fabs(a - b) > eps) {
    std::ostringstream oss;
    oss << "expected " << a << " ~= " << b << " (eps=" << eps << ")";
    throw AssertError(oss.str(), file, line);
  }
}

void TestSuite::assert_neq_int(long a, long b, const char* file, int line) {
  if (a == b) {
    std::ostringstream oss;
    oss << "expected " << a << " != " << b;
    throw AssertError(oss.str(), file, line);
  }
}

void TestSuite::assert_lt(long a, long b, const char* file, int line) {
  if (!(a < b)) {
    std::ostringstream oss;
    oss << "expected " << a << " < " << b;
    throw AssertError(oss.str(), file, line);
  }
}

void TestSuite::assert_gt(long a, long b, const char* file, int line) {
  if (!(a > b)) {
    std::ostringstream oss;
    oss << "expected " << a << " > " << b;
    throw AssertError(oss.str(), file, line);
  }
}

void TestSuite::assert_le(long a, long b, const char* file, int line) {
  if (!(a <= b)) {
    std::ostringstream oss;
    oss << "expected " << a << " <= " << b;
    throw AssertError(oss.str(), file, line);
  }
}

void TestSuite::assert_ge(long a, long b, const char* file, int line) {
  if (!(a >= b)) {
    std::ostringstream oss;
    oss << "expected " << a << " >= " << b;
    throw AssertError(oss.str(), file, line);
  }
}

void TestSuite::assert_null(const void* p, const char* file, int line) {
  if (p != 0) throw AssertError("expected NULL pointer", file, line);
}

void TestSuite::assert_not_null(const void* p, const char* file, int line) {
  if (p == 0) throw AssertError("expected non-NULL pointer", file, line);
}

void TestSuite::assert_str_contains(const std::string& haystack,
                                    const std::string& needle, const char* file,
                                    int line) {
  if (haystack.find(needle) == std::string::npos) {
    std::ostringstream oss;
    oss << "expected \"" << haystack << "\" to contain \"" << needle << "\"";
    throw AssertError(oss.str(), file, line);
  }
}

/* ── Accessors ─────────────────────────────────────────────────────────── */

int TestSuite::test_count() const { return _test_count; }
int TestSuite::total() const { return _test_count; }
int TestSuite::passed() const { return _passed; }
int TestSuite::failed() const { return _failed; }
int TestSuite::skipped() const { return _skipped; }

const TestResult& TestSuite::result(int i) const { return _results[i]; }

/* ── Printing ──────────────────────────────────────────────────────────── */

void TestSuite::_print_header() const {
  Srgb cyan(0, 200, 255);
  std::cout << "\n"
            << cyan.to_ansi_fg() << "━━━ " << _name << " ━━━"
            << TermUtils::reset() << "\n"
            << std::endl;
}

void TestSuite::_print_result(const TestResult& r) const {
  if (r.skipped) {
    Srgb yellow(255, 200, 0);
    std::cout << "  " << yellow.to_ansi_fg() << "⊘ SKIP" << TermUtils::reset()
              << "  " << r.name << std::endl;
    return;
  }
  if (r.passed) {
    Srgb green(0, 220, 0);
    std::ostringstream oss;
    oss << r.elapsed_ms;
    std::cout << "  " << green.to_ansi_fg() << "✓ PASS" << TermUtils::reset()
              << "  " << r.name << "  (" << oss.str() << " ms)" << std::endl;
  } else {
    Srgb red(255, 60, 60);
    std::cout << "  " << red.to_ansi_fg() << "✗ FAIL" << TermUtils::reset()
              << "  " << r.name << std::endl;
    if (!r.error_msg.empty())
      std::cout << "         " << r.error_msg << std::endl;
  }
}

void TestSuite::_print_summary() const {
  Srgb green(0, 220, 0);
  Srgb red(255, 60, 60);
  Srgb yellow(255, 200, 0);
  Srgb dim(128, 128, 128);

  std::cout << "\n"
            << dim.to_ansi_fg() << "───────────────────────────────────"
            << TermUtils::reset() << std::endl;

  std::cout << "  " << green.to_ansi_fg() << _passed << " passed"
            << TermUtils::reset();
  if (_failed > 0)
    std::cout << "  " << red.to_ansi_fg() << _failed << " failed"
              << TermUtils::reset();
  if (_skipped > 0)
    std::cout << "  " << yellow.to_ansi_fg() << _skipped << " skipped"
              << TermUtils::reset();
  std::cout << "  " << dim.to_ansi_fg() << "(" << _test_count << " total)"
            << TermUtils::reset() << "\n"
            << std::endl;
}

} /* namespace test */
} /* namespace libcpp */
