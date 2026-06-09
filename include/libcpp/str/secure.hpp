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

} /* namespace str */
} /* namespace libcpp */

#endif /* LIBCPP_STR_SECURE_HPP */
