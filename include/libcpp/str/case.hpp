/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   case.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_STR_CASE_HPP
# define LIBCPP_STR_CASE_HPP

# include <string>

namespace libcpp
{
namespace str
{

/* ── Case conversion (UTF-8 aware) ─────────────────────────────────────── */

std::string to_upper(const std::string& s);
std::string to_lower(const std::string& s);
std::string to_title(const std::string& s);
std::string toggle_case(const std::string& s);

/* ── Case-insensitive comparison ───────────────────────────────────────── */

bool eq_nocase(const std::string& a, const std::string& b);

/* ── Case style conversions ────────────────────────────────────────────── */

std::string to_snake_case(const std::string& s);
std::string to_camel_case(const std::string& s);
std::string to_pascal_case(const std::string& s);
std::string to_kebab_case(const std::string& s);

} /* namespace str */
} /* namespace libcpp */

#endif /* LIBCPP_STR_CASE_HPP */
