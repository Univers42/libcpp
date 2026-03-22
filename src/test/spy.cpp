/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spy.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:40:18 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/test/spy.hpp"

namespace libcpp {
namespace test {

/* ── CallRecord ────────────────────────────────────────────────────────── */

CallRecord::CallRecord() : arg_int(0) {}
CallRecord::CallRecord(const std::string& t, long a) : tag(t), arg_int(a) {}
CallRecord::CallRecord(const CallRecord& o) : tag(o.tag), arg_int(o.arg_int) {}
CallRecord& CallRecord::operator=(const CallRecord& o) {
  if (this != &o) {
    tag = o.tag;
    arg_int = o.arg_int;
  }
  return *this;
}
CallRecord::~CallRecord() {}

/* ── Spy ───────────────────────────────────────────────────────────────── */

Spy::Spy() : _count(0) {}
Spy::Spy(const Spy& o) : _count(o._count) {
  for (int i = 0; i < _count; ++i) _calls[i] = o._calls[i];
}
Spy& Spy::operator=(const Spy& o) {
  if (this != &o) {
    _count = o._count;
    for (int i = 0; i < _count; ++i) _calls[i] = o._calls[i];
  }
  return *this;
}
Spy::~Spy() {}

void Spy::call() {
  if (_count < MAX_CALLS) _calls[_count++] = CallRecord("", 0);
}

void Spy::call(const std::string& tag) {
  if (_count < MAX_CALLS) _calls[_count++] = CallRecord(tag, 0);
}

void Spy::call(const std::string& tag, long arg) {
  if (_count < MAX_CALLS) _calls[_count++] = CallRecord(tag, arg);
}

int Spy::count() const { return _count; }
bool Spy::was_called() const { return _count > 0; }

bool Spy::was_called_with(const std::string& tag) const {
  for (int i = 0; i < _count; ++i)
    if (_calls[i].tag == tag) return true;
  return false;
}

bool Spy::was_called_times(int n) const { return _count == n; }

std::string Spy::last_tag() const {
  if (_count > 0) return _calls[_count - 1].tag;
  return std::string();
}

const CallRecord& Spy::at(int index) const {
  static CallRecord empty;
  if (index >= 0 && index < _count) return _calls[index];
  return empty;
}

const CallRecord& Spy::last() const {
  static CallRecord empty;
  if (_count > 0) return _calls[_count - 1];
  return empty;
}

const CallRecord& Spy::first() const {
  static CallRecord empty;
  if (_count > 0) return _calls[0];
  return empty;
}

void Spy::reset() { _count = 0; }

/* ── StubInt ───────────────────────────────────────────────────────────── */

StubInt::StubInt() : _value(0), _calls(0) {}
StubInt::StubInt(long value) : _value(value), _calls(0) {}
StubInt::StubInt(const StubInt& o) : _value(o._value), _calls(o._calls) {}
StubInt& StubInt::operator=(const StubInt& o) {
  if (this != &o) {
    _value = o._value;
    _calls = o._calls;
  }
  return *this;
}
StubInt::~StubInt() {}

long StubInt::get() {
  ++_calls;
  return _value;
}
void StubInt::set(long v) { _value = v; }
int StubInt::call_count() const { return _calls; }
void StubInt::reset() { _calls = 0; }

/* ── StubStr ───────────────────────────────────────────────────────────── */

StubStr::StubStr() : _calls(0) {}
StubStr::StubStr(const std::string& value) : _value(value), _calls(0) {}
StubStr::StubStr(const StubStr& o) : _value(o._value), _calls(o._calls) {}
StubStr& StubStr::operator=(const StubStr& o) {
  if (this != &o) {
    _value = o._value;
    _calls = o._calls;
  }
  return *this;
}
StubStr::~StubStr() {}

std::string StubStr::get() {
  ++_calls;
  return _value;
}
void StubStr::set(const std::string& v) { _value = v; }
int StubStr::call_count() const { return _calls; }
void StubStr::reset() { _calls = 0; }

} /* namespace test */
} /* namespace libcpp */
