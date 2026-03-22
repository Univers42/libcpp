/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   suite.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:56:48 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_TEST_SUITE_HPP
#define LIBCPP_TEST_SUITE_HPP

#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include "libcpp/term/color.hpp"
#include "libcpp/term/style.hpp"

namespace libcpp {
namespace test {

/* ── Forward declarations ──────────────────────────────────────────────── */

class TestSuite;

/* ── Test function pointer type ────────────────────────────────────────── */

typedef void (*TestFn)(TestSuite&);

/* ── AssertError — thrown on assertion failure ──────────────────────────── */

struct AssertError {
  std::string message;
  std::string file;
  int line;

  AssertError();
  AssertError(const std::string& msg, const std::string& f, int l);
  AssertError(const AssertError& o);
  AssertError& operator=(const AssertError& o);
  ~AssertError();
};

/* ── TestCase — single registered test ─────────────────────────────────── */

struct TestCase {
  std::string name;
  TestFn fn;
  bool skip;

  TestCase();
  TestCase(const std::string& n, TestFn f, bool s);
  TestCase(const TestCase& o);
  TestCase& operator=(const TestCase& o);
  ~TestCase();
};

/* ── TestResult — outcome of a single test ─────────────────────────────── */

struct TestResult {
  std::string name;
  bool passed;
  bool skipped;
  std::string error_msg;
  double elapsed_ms;

  TestResult();
  TestResult(const TestResult& o);
  TestResult& operator=(const TestResult& o);
  ~TestResult();
};

/* ── TestSuite ─────────────────────────────────────────────────────────── */

class TestSuite {
 public:
  static const int MAX_TESTS = 512;
  static const int MAX_RESULTS = 512;

  TestSuite();
  TestSuite(const std::string& name);
  TestSuite(const TestSuite& o);
  TestSuite& operator=(const TestSuite& o);
  ~TestSuite();

  /* Registration */
  TestSuite& test(const std::string& name, TestFn fn);
  TestSuite& skip(const std::string& name, TestFn fn);
  TestSuite& before_each(TestFn fn);
  TestSuite& after_each(TestFn fn);
  TestSuite& before_all(TestFn fn);
  TestSuite& after_all(TestFn fn);

  /* Run */
  int run();
  void run_test(int index);

  /* Assertions — throw AssertError on failure */
  void assert_true(bool cond, const char* expr, const char* file, int line);
  void assert_false(bool cond, const char* expr, const char* file, int line);
  void assert_eq_str(const std::string& a, const std::string& b,
                     const char* file, int line);
  void assert_eq_int(long a, long b, const char* file, int line);
  void assert_eq_double(double a, double b, double eps, const char* file,
                        int line);
  void assert_neq_int(long a, long b, const char* file, int line);
  void assert_lt(long a, long b, const char* file, int line);
  void assert_gt(long a, long b, const char* file, int line);
  void assert_le(long a, long b, const char* file, int line);
  void assert_ge(long a, long b, const char* file, int line);
  void assert_null(const void* p, const char* file, int line);
  void assert_not_null(const void* p, const char* file, int line);
  void assert_str_contains(const std::string& haystack,
                           const std::string& needle, const char* file,
                           int line);

  /* Accessors */
  int test_count() const;
  int total() const;
  int passed() const;
  int failed() const;
  int skipped() const;
  const TestResult& result(int i) const;

 private:
  std::string _name;
  TestCase _tests[MAX_TESTS];
  int _test_count;
  TestResult _results[MAX_RESULTS];
  int _result_count;
  TestFn _before_each;
  TestFn _after_each;
  TestFn _before_all;
  TestFn _after_all;
  int _passed;
  int _failed;
  int _skipped;

  void _print_header() const;
  void _print_result(const TestResult& r) const;
  void _print_summary() const;
};

/* ── Assertion macros ──────────────────────────────────────────────────── */

#define ASSERT_TRUE(suite, expr) \
  (suite).assert_true((expr), #expr, __FILE__, __LINE__)

#define ASSERT_FALSE(suite, expr) \
  (suite).assert_false((expr), #expr, __FILE__, __LINE__)

#define ASSERT_EQ(suite, a, b)                                                \
  (suite).assert_eq_int(static_cast<long>(a), static_cast<long>(b), __FILE__, \
                        __LINE__)

#define ASSERT_EQ_STR(suite, a, b) \
  (suite).assert_eq_str((a), (b), __FILE__, __LINE__)

#define ASSERT_EQ_DBL(suite, a, b, eps) \
  (suite).assert_eq_double((a), (b), (eps), __FILE__, __LINE__)

#define ASSERT_NEQ(suite, a, b)                                                \
  (suite).assert_neq_int(static_cast<long>(a), static_cast<long>(b), __FILE__, \
                         __LINE__)

#define ASSERT_LT(suite, a, b)                                            \
  (suite).assert_lt(static_cast<long>(a), static_cast<long>(b), __FILE__, \
                    __LINE__)

#define ASSERT_GT(suite, a, b)                                            \
  (suite).assert_gt(static_cast<long>(a), static_cast<long>(b), __FILE__, \
                    __LINE__)

#define ASSERT_LE(suite, a, b)                                            \
  (suite).assert_le(static_cast<long>(a), static_cast<long>(b), __FILE__, \
                    __LINE__)

#define ASSERT_GE(suite, a, b)                                            \
  (suite).assert_ge(static_cast<long>(a), static_cast<long>(b), __FILE__, \
                    __LINE__)

#define ASSERT_NULL(suite, p) \
  (suite).assert_null(static_cast<const void*>(p), __FILE__, __LINE__)

#define ASSERT_NOT_NULL(suite, p) \
  (suite).assert_not_null(static_cast<const void*>(p), __FILE__, __LINE__)

#define ASSERT_STR_CONTAINS(suite, haystack, needle) \
  (suite).assert_str_contains((haystack), (needle), __FILE__, __LINE__)

} /* namespace test */
} /* namespace libcpp */

#endif /* LIBCPP_TEST_SUITE_HPP */
