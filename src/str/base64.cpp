/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   base64.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/24 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/str/base64.hpp"

namespace libcpp {
namespace str {

namespace {

const char kAlphabet[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* Sextet value of an alphabet byte, or -1. Not a table: 64 comparisons at
** worst, and building a 256-entry static costs more than it saves here. */
int sextet(char c) {
  if (c >= 'A' && c <= 'Z') return c - 'A';
  if (c >= 'a' && c <= 'z') return c - 'a' + 26;
  if (c >= '0' && c <= '9') return c - '0' + 52;
  if (c == '+') return 62;
  if (c == '/') return 63;
  return -1;
}

/* Trailing '=' count, only meaningful once length%4==0 has been checked. */
std::size_t pad_count(const std::string& s) {
  std::size_t n = s.size();
  if (n == 0) return 0;
  if (s[n - 1] != '=') return 0;
  if (n >= 2 && s[n - 2] == '=') return 2;
  return 1;
}

}  // namespace

bool is_base64(const std::string& s) {
  const std::size_t n = s.size();
  if (n % 4 != 0) return false;
  if (n == 0) return true;

  const std::size_t pad = pad_count(s);
  /* Everything before the padding must be alphabet; the padding itself was
  ** already established to be a run of '=' anchored at the very end, so a
  ** '=' anywhere earlier fails here as a non-alphabet byte. */
  for (std::size_t i = 0; i < n - pad; ++i)
    if (sextet(s[i]) < 0) return false;
  return true;
}

std::size_t base64_decoded_size(const std::string& s) {
  const std::size_t n = s.size();
  if (n < 4) return 0;
  return (n / 4) * 3 - pad_count(s);
}

std::string base64_encode(const std::string& raw) {
  std::string out;
  const std::size_t n = raw.size();
  out.reserve(((n + 2) / 3) * 4);

  std::size_t i = 0;
  for (; i + 3 <= n; i += 3) {
    unsigned long v = (static_cast<unsigned char>(raw[i]) << 16) |
                      (static_cast<unsigned char>(raw[i + 1]) << 8) |
                      static_cast<unsigned char>(raw[i + 2]);
    out += kAlphabet[(v >> 18) & 63];
    out += kAlphabet[(v >> 12) & 63];
    out += kAlphabet[(v >> 6) & 63];
    out += kAlphabet[v & 63];
  }

  const std::size_t rest = n - i;
  if (rest == 1) {
    unsigned long v = static_cast<unsigned long>(
        static_cast<unsigned char>(raw[i]) << 16);
    out += kAlphabet[(v >> 18) & 63];
    out += kAlphabet[(v >> 12) & 63];
    out += '=';
    out += '=';
  } else if (rest == 2) {
    unsigned long v = (static_cast<unsigned char>(raw[i]) << 16) |
                      (static_cast<unsigned char>(raw[i + 1]) << 8);
    out += kAlphabet[(v >> 18) & 63];
    out += kAlphabet[(v >> 12) & 63];
    out += kAlphabet[(v >> 6) & 63];
    out += '=';
  }
  return out;
}

bool base64_decode(const std::string& s, std::string& out) {
  if (!is_base64(s)) return false;

  const std::size_t n = s.size();
  const std::size_t pad = pad_count(s);
  std::string decoded;
  decoded.reserve(base64_decoded_size(s));

  for (std::size_t i = 0; i + 4 <= n; i += 4) {
    const bool last = (i + 4 == n);
    /* Padding sextets contribute zero bits; the bytes they would have
    ** produced are dropped by the emit count below. */
    const int c0 = sextet(s[i]);
    const int c1 = sextet(s[i + 1]);
    const int c2 = (last && pad == 2) ? 0 : sextet(s[i + 2]);
    const int c3 = (last && pad >= 1) ? 0 : sextet(s[i + 3]);

    unsigned long v = (static_cast<unsigned long>(c0) << 18) |
                      (static_cast<unsigned long>(c1) << 12) |
                      (static_cast<unsigned long>(c2) << 6) |
                      static_cast<unsigned long>(c3);

    const std::size_t emit = last ? 3 - pad : 3;
    if (emit > 0) decoded += static_cast<char>((v >> 16) & 0xFF);
    if (emit > 1) decoded += static_cast<char>((v >> 8) & 0xFF);
    if (emit > 2) decoded += static_cast<char>(v & 0xFF);
  }

  out.swap(decoded);
  return true;
}

} /* namespace str */
} /* namespace libcpp */
