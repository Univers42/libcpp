/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   format.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 22:06:06 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_STR_FORMAT_HPP
#define LIBCPP_STR_FORMAT_HPP

#include <sstream>
#include <string>
#include <vector>

namespace libcpp {
namespace str {

/* ── to_string helpers(C++98, no std::to_string) ──────────────────────── */

std::string to_string(int v);
std::string to_string(long v);
std::string to_string(unsigned int v);
std::string to_string(unsigned long v);
std::string to_string(double v);
std::string to_string(const char* v);
std::string to_string(const std::string& v);
std::string to_string(bool v);

/* ── Format — printf-style positional formatting ───────────────────────── */
/*
** fmt("{0} has {1} items", name, count)
** Placeholders are {0}, {1}, ... up to {9}
*/

std::string fmt(const std::string& pattern);
std::string fmt(const std::string& pattern, const std::string& a0);
std::string fmt(const std::string& pattern, const std::string& a0,
                const std::string& a1);
std::string fmt(const std::string& pattern, const std::string& a0,
                const std::string& a1, const std::string& a2);
std::string fmt(const std::string& pattern, const std::string& a0,
                const std::string& a1, const std::string& a2,
                const std::string& a3);
std::string fmt(const std::string& pattern, const std::string& a0,
                const std::string& a1, const std::string& a2,
                const std::string& a3, const std::string& a4);

/* ── String manipulation ───────────────────────────────────────────────── */

std::string trim(const std::string& s);
std::string trim_left(const std::string& s);
std::string trim_right(const std::string& s);
std::string pad_left(const std::string& s, int width, char pad);
std::string pad_right(const std::string& s, int width, char pad);
std::string repeat(const std::string& s, int n);
std::string join(const std::string* arr, int count, const std::string& sep);

/* Fixed-array split.  Returns the number of fields written.  When the input
** holds more fields than `max`, the LAST slot receives the whole unsplit
** remainder: a bound may shorten the field list, but it never silently
** discards input.  (It used to drop the tail with no indication at all.) */
int split(const std::string& s, char delim, std::string* out, int max);

/* Unbounded split — no array to size, no bound to guess.  n delimiters give
** n + 1 fields, so "a,,b" yields 3 and "a," yields 2 (the last empty). */
std::vector<std::string> split(const std::string& s, char delim);

/* Same, with empty fields dropped — what a comma-separated protocol list
** ("#a,,#b,") almost always means. */
std::vector<std::string> split_nonempty(const std::string& s, char delim);

bool starts_with(const std::string& s, const std::string& prefix);
bool ends_with(const std::string& s, const std::string& suffix);
std::string replace_all(const std::string& s, const std::string& from,
                        const std::string& to);
bool contains(const std::string& s, const std::string& sub);
int count(const std::string& s, const std::string& sub);
std::string reverse(const std::string& s);
std::string center(const std::string& s, int width, char pad);
std::string truncate(const std::string& s, int max_len,
                     const std::string& suffix);
bool is_empty(const std::string& s);
bool is_blank(const std::string& s);

/* ── Parsing — the inverse of to_string ────────────────────────────────── */
/*
** Strict and total: the WHOLE string must be a number.  These reject an
** empty string, leading whitespace, a leading '+', a lone '-', embedded or
** trailing garbage ("12abc"), and any value outside the requested range.
** `out` is left untouched on failure, and errno is restored, so a caller
** that is careful about errno (or forbidden from branching on it) can use
** these freely.
**
** They exist because the raw strtol/strtoul idiom is easy to get subtly
** wrong: it needs errno cleared beforehand, an end-pointer check AND a range
** check to be correct — and strtoul in particular accepts "-1" happily,
** wrapping it to ULONG_MAX instead of failing.
*/
bool parse_long(const std::string& s, long& out);
bool parse_long(const std::string& s, long lo, long hi, long& out);
bool parse_ulong(const std::string& s, unsigned long& out);
bool parse_ulong(const std::string& s, unsigned long lo, unsigned long hi,
                 unsigned long& out);

/* ── Message — streamable string builder(replaces old message.hpp) ────── */

class Message {
 public:
  Message();
  Message(const Message& o);
  Message& operator=(const Message& o);
  ~Message();

  template <typename T>
  Message& operator<<(const T& val) {
    _ss << val;
    return *this;
  }

  std::string str() const;

 private:
  std::ostringstream _ss;
};

} /* namespace str */
} /* namespace libcpp */

#endif /* LIBCPP_STR_FORMAT_HPP */
