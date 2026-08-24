/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   secure.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/09 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/str/secure.hpp"

namespace libcpp {
namespace str {

bool eq_consttime(const std::string& a, const std::string& b)
{
	/* c_str() guarantees a readable NUL at index size(), so indexing with
	** i % (len + 1) is always in bounds and the loop shape is identical
	** whatever the contents. */
	const char* pa = a.c_str();
	const char* pb = b.c_str();
	const std::string::size_type la = a.size();
	const std::string::size_type lb = b.size();
	std::string::size_type n = la > lb ? la : lb;

	volatile unsigned char acc = (la == lb) ? 0 : 1;
	for (std::string::size_type i = 0; i < n; ++i)
	{
		unsigned char ca = static_cast<unsigned char>(pa[i % (la + 1)]);
		unsigned char cb = static_cast<unsigned char>(pb[i % (lb + 1)]);
		acc = static_cast<unsigned char>(acc | (ca ^ cb));
	}
	return acc == 0;
}

bool is_safe_path_component(const std::string& s)
{
	return is_safe_path_component(s, "");
}

bool is_safe_path_component(const std::string& s, const char* also_reject)
{
	if (s.empty() || s == "." || s == "..")
		return false;
	for (std::string::size_type i = 0; i < s.size(); ++i)
	{
		const unsigned char c = static_cast<unsigned char>(s[i]);
		if (c < 0x20 || c == 0x7F || c == '/' || c == '\\')
			return false;
		if (also_reject != 0)
			for (const char *p = also_reject; *p != '\0'; ++p)
				if (s[i] == *p)
					return false;
	}
	return true;
}

} /* namespace str */
} /* namespace libcpp */
