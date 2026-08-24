/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   secure.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/09 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_STR_SECURE_HPP
#define LIBCPP_STR_SECURE_HPP

#include <string>

namespace libcpp {
namespace str {

/* ── Timing-safe comparison ────────────────────────────────────────────── */

/* Compares the full length of both strings regardless of where the first
** mismatch lies, so response timing leaks nothing about a secret's prefix.
** Use for passwords/tokens instead of operator== (which short-circuits). */
bool eq_consttime(const std::string& a, const std::string& b);

/* ── Path-component safety ─────────────────────────────────────────────── */

/*
** True when s is safe to use as a SINGLE path component -- a filename with
** no directory part.  Rejects, in order: the empty string, "." and "..",
** any '/' or '\\', and any control byte (0x00-0x1F) or DEL (0x7F).
**
** This is the check that stands between an attacker-supplied name and a
** path-traversal write.  It is deliberately about the FILESYSTEM only: a
** space or a comma is a perfectly legal filename byte, and rejecting them
** is a wire-format decision belonging to whatever protocol carries the
** name.  The second form takes those extra bytes explicitly -- pass " ,"
** for a space-delimited, comma-list protocol such as IRC.
**
** Note it does not consult the filesystem, so it says nothing about whether
** the name exists, and it does not fold case, so on a case-insensitive
** filesystem two safe names can still collide.
*/
bool is_safe_path_component(const std::string& s);
bool is_safe_path_component(const std::string& s, const char* also_reject);

} /* namespace str */
} /* namespace libcpp */

#endif /* LIBCPP_STR_SECURE_HPP */
