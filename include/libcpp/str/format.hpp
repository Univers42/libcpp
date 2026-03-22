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
int split(const std::string& s, char delim, std::string* out, int max);
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
