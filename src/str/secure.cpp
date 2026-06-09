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

} /* namespace str */
} /* namespace libcpp */
