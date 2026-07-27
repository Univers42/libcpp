/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   case.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:47:01 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_STR_CASE_HPP
#define LIBCPP_STR_CASE_HPP

#include <string>

namespace libcpp {
namespace str {

/* ── Case conversion(UTF-8 aware) ─────────────────────────────────────── */

std::string to_upper(const std::string& s);
std::string to_lower(const std::string& s);
std::string to_title(const std::string& s);
std::string capitalize(const std::string& s);
std::string toggle_case(const std::string& s);

/* ── Case-insensitive comparison ───────────────────────────────────────── */

/* Unicode-aware: decodes UTF-8 and folds accented letters too.  Allocates
** two lowercase copies per call — see eq_ascii_nocase below when that is
** both wrong and expensive. */
bool eq_nocase(const std::string& a, const std::string& b);

/* ── ASCII-only case operations ────────────────────────────────────────── */
/*
** Fold A-Z / a-z and pass every other byte through untouched, UTF-8
** continuation bytes included.
**
** Reach for these — NOT to_lower/to_upper/eq_nocase — whenever a
** specification defines case folding over ASCII and only ASCII: IRC's
** CASEMAPPING=ascii, HTTP header field names, DNS labels, URI schemes.  The
** Unicode-aware versions would treat letters those protocols require to stay
** distinct as equal, which is a correctness bug and, where names are
** security-relevant, an impersonation vector.
**
** eq_ascii_nocase also compares in place and stops at the first difference,
** so it allocates nothing at all.
*/
std::string ascii_to_lower(const std::string& s);
std::string ascii_to_upper(const std::string& s);
bool eq_ascii_nocase(const std::string& a, const std::string& b);

/* ── Case style conversions ────────────────────────────────────────────── */

std::string to_snake_case(const std::string& s);
std::string to_camel_case(const std::string& s);
std::string to_pascal_case(const std::string& s);
std::string to_kebab_case(const std::string& s);

} /* namespace str */
} /* namespace libcpp */

#endif /* LIBCPP_STR_CASE_HPP */
