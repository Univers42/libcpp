/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utf8.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:04:25 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/str/utf8.hpp"

namespace libcpp
{
namespace str
{

void append_utf8(std::string& out, uint32_t cp)
{
	if (cp <= 0x7F)
	{
		out += static_cast<char>(cp);
	}
	else if (cp <= 0x7FF)
	{
		out += static_cast<char>(0xC0 | ((cp >> 6) & 0x1F));
		out += static_cast<char>(0x80 | (cp & 0x3F));
	}
	else if (cp <= 0xFFFF)
	{
		out += static_cast<char>(0xE0 | ((cp >> 12) & 0x0F));
		out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
		out += static_cast<char>(0x80 | (cp & 0x3F));
	}
	else
	{
		out += static_cast<char>(0xF0 | ((cp >> 18) & 0x07));
		out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
		out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
		out += static_cast<char>(0x80 | (cp & 0x3F));
	}
}

uint32_t decode_utf8(const std::string& s, std::size_t& idx)
{
	const unsigned char* data = reinterpret_cast<const unsigned char*>(s.data());
	std::size_t n = s.size();

	if (idx >= n) return 0;

	unsigned char c = data[idx];
	if (c < 0x80)
	{
		++idx;
		return c;
	}

	std::size_t len = 0;
	uint32_t cp = 0;

	if ((c & 0xE0) == 0xC0)      { cp = c & 0x1F; len = 2; }
	else if ((c & 0xF0) == 0xE0) { cp = c & 0x0F; len = 3; }
	else if ((c & 0xF8) == 0xF0) { cp = c & 0x07; len = 4; }
	else { ++idx; return 0xFFFD; }

	if (idx + len > n) { ++idx; return 0xFFFD; }

	for (std::size_t k = 1; k < len; ++k)
	{
		unsigned char cc = data[idx + k];
		if ((cc & 0xC0) != 0x80) { ++idx; return 0xFFFD; }
		cp = (cp << 6) | (cc & 0x3F);
	}

	idx += len;
	return cp;
}

int utf8_len(const std::string& s)
{
	int count = 0;
	std::size_t idx = 0;
	while (idx < s.size())
	{
		decode_utf8(s, idx);
		++count;
	}
	return count;
}

std::size_t utf8_offset(const std::string& s, int n)
{
	std::size_t idx = 0;
	for (int i = 0; i < n && idx < s.size(); ++i)
		decode_utf8(s, idx);
	return idx;
}

std::string utf8_substr(const std::string& s, int start, int count)
{
	std::size_t begin = utf8_offset(s, start);
	std::size_t idx = begin;
	for (int i = 0; i < count && idx < s.size(); ++i)
		decode_utf8(s, idx);
	return s.substr(begin, idx - begin);
}

bool utf8_valid(const std::string& s)
{
	std::size_t idx = 0;
	while (idx < s.size())
	{
		if (decode_utf8(s, idx) == 0xFFFD)
			return false;
	}
	return true;
}

std::wstring utf8_to_wstr(const std::string& s)
{
	std::wstring ws;
	std::size_t idx = 0;
	while (idx < s.size())
	{
		uint32_t cp = decode_utf8(s, idx);
		if (cp <= 0xFFFF)
			ws += static_cast<wchar_t>(cp);
		else
		{
			cp -= 0x10000;
			ws += static_cast<wchar_t>((cp >> 10) + 0xD800);
			ws += static_cast<wchar_t>((cp & 0x3FF) + 0xDC00);
		}
	}
	return ws;
}

} /* namespace str */
} /* namespace libcpp */
