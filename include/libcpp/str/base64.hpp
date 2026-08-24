/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   base64.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/24 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_STR_BASE64_HPP
#define LIBCPP_STR_BASE64_HPP

#include <cstddef>
#include <string>

namespace libcpp {
namespace str {

/* ── Base64 (RFC 4648 §4, the standard alphabet) ───────────────────────── */
/*
** Padded output only, no line wrapping, no URL-safe alternate alphabet.
** Binary-safe in both directions: embedded NUL bytes travel intact, because
** every interface here is std::string carrying a length rather than a
** NUL-terminated char*.
*/

/*
** STRICT validity, which is the point of having this rather than an
** alphabet scan.  Requires all four of:
**   - length a multiple of 4
**   - every byte in [A-Za-z0-9+/] or '='
**   - '=' only in the final quantum, only as the last one or two bytes
**   - nothing after the first '=' except more '='
**
** So "QUJD" and "QQ==" pass; "a=b", "QQ", "QQ=" and "QUJD=" do not.  A
** lenient alphabet-only check accepts strings that decode to garbage, which
** is exactly the case a relay must reject before forwarding bytes it will
** never itself decode.
**
** is_base64("") is true: the empty string is the encoding of no input.
** Callers that need a non-empty payload must say so themselves.
*/
bool is_base64(const std::string& s);

/*
** Number of bytes is_base64-valid input decodes to, accounting for padding.
** Meaningless for input is_base64 rejects -- check first.  Never decodes,
** so it costs O(1) and is the right way to meter a transfer against a
** declared size without buffering the plaintext.
*/
std::size_t base64_decoded_size(const std::string& s);

std::string base64_encode(const std::string& raw);

/* False (with out untouched) when s fails is_base64. */
bool base64_decode(const std::string& s, std::string& out);

} /* namespace str */
} /* namespace libcpp */

#endif /* LIBCPP_STR_BASE64_HPP */
