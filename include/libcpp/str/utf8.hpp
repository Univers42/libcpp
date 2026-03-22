/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utf8.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:04:24 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_STR_UTF8_HPP
#define LIBCPP_STR_UTF8_HPP

#include <stdint.h>
#include <cstddef>
#include <string>

namespace libcpp {
namespace str {

/* ── UTF-8 encoding/decoding ───────────────────────────────────────────── */

/* Append a Unicode codepoint as UTF-8 bytes to *out */
void append_utf8(std::string& out, uint32_t cp);

/* Decode next UTF-8 codepoint from s at *idx; advances *idx.
   Returns 0xFFFD on invalid sequence. */
uint32_t decode_utf8(const std::string& s, std::size_t& idx);

/* Count the number of UTF-8 codepoints in s */
int utf8_len(const std::string& s);

/* Return byte offset of the n-th codepoint(0-indexed), or s.size() */
std::size_t utf8_offset(const std::string& s, int n);

/* Extract a substring by codepoint range [start, start+count) */
std::string utf8_substr(const std::string& s, int start, int count);

/* Check if s is valid UTF-8 */
bool utf8_valid(const std::string& s);

/* Convert UTF-8 string to std::wstring */
std::wstring utf8_to_wstr(const std::string& s);

} /* namespace str */
} /* namespace libcpp */

#endif /* LIBCPP_STR_UTF8_HPP */
