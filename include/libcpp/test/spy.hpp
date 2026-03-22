/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spy.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:40:20 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_TEST_SPY_HPP
#define LIBCPP_TEST_SPY_HPP

#include <sstream>
#include <string>

namespace libcpp {
namespace test {

/* ── CallRecord — stores info about a single call ──────────────────────── */

struct CallRecord {
  std::string tag; /* optional label for the call */
  long arg_int;    /* generic integer argument snapshot */

  CallRecord();
  CallRecord(const std::string& t, long a);
  CallRecord(const CallRecord& o);
  CallRecord& operator=(const CallRecord& o);
  ~CallRecord();
};

/* ── Spy — tracks function calls(C++98 compatible) ────────────────────── */
/*
** Usage:
**   Spy spy;
**   spy.call("doSomething", 42);
**   assert(spy.count() == 1);
**   assert(spy.was_called());
**   assert(spy.last().tag == "doSomething");
**   assert(spy.at(0).arg_int == 42);
*/

class Spy {
 public:
  static const int MAX_CALLS = 256;

  Spy();
  Spy(const Spy& o);
  Spy& operator=(const Spy& o);
  ~Spy();

  /* Record a call */
  void call();
  void call(const std::string& tag);
  void call(const std::string& tag, long arg);

  /* Query */
  int count() const;
  bool was_called() const;
  bool was_called_with(const std::string& tag) const;
  bool was_called_times(int n) const;
  std::string last_tag() const;
  const CallRecord& at(int index) const;
  const CallRecord& last() const;
  const CallRecord& first() const;

  /* Reset */
  void reset();

 private:
  CallRecord _calls[MAX_CALLS];
  int _count;
};

/* ── Stub — simple return-value override ───────────────────────────────── */
/*
** Usage:
**   StubInt stub(42);
**   int val = stub.get();   // returns 42
**   stub.set(99);
**   val = stub.get();       // returns 99
**   assert(stub.call_count() == 2);
*/

class StubInt {
 public:
  StubInt();
  StubInt(long value);
  StubInt(const StubInt& o);
  StubInt& operator=(const StubInt& o);
  ~StubInt();

  long get();
  void set(long value);
  int call_count() const;
  void reset();

 private:
  long _value;
  int _calls;
};

class StubStr {
 public:
  StubStr();
  StubStr(const std::string& value);
  StubStr(const StubStr& o);
  StubStr& operator=(const StubStr& o);
  ~StubStr();

  std::string get();
  void set(const std::string& value);
  int call_count() const;
  void reset();

 private:
  std::string _value;
  int _calls;
};

/* ── Spy assertion macros ──────────────────────────────────────────────── */

#define ASSERT_CALLED(suite, spy)                                       \
  (suite).assert_true((spy).was_called(), "spy.was_called()", __FILE__, \
                      __LINE__)

#define ASSERT_NOT_CALLED(suite, spy)                                    \
  (suite).assert_false((spy).was_called(), "spy.was_called()", __FILE__, \
                       __LINE__)

#define ASSERT_CALL_COUNT(suite, spy, n) \
  (suite).assert_eq_int((spy).count(), (n), __FILE__, __LINE__)

#define ASSERT_CALLED_WITH(suite, spy, tag)       \
  (suite).assert_true((spy).was_called_with(tag), \
                      "spy.was_called_with(" #tag ")", __FILE__, __LINE__)

} /* namespace test */
} /* namespace libcpp */

#endif /* LIBCPP_TEST_SPY_HPP */
