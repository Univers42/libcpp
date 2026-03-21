/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   format.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:47:01 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/str/format.hpp"
#include <cctype>

namespace libcpp
{
namespace str
{

/* ── to_string ─────────────────────────────────────────────────────────── */

std::string to_string(int v)          { std::ostringstream o; o << v; return o.str(); }
std::string to_string(long v)         { std::ostringstream o; o << v; return o.str(); }
std::string to_string(unsigned int v) { std::ostringstream o; o << v; return o.str(); }
std::string to_string(unsigned long v){ std::ostringstream o; o << v; return o.str(); }
std::string to_string(double v)       { std::ostringstream o; o << v; return o.str(); }
std::string to_string(const char* v)  { return v ? std::string(v) : std::string("(null)"); }
std::string to_string(const std::string& v) { return v; }
std::string to_string(bool v)         { return v ? "true" : "false"; }

/* ── fmt — positional format helper ────────────────────────────────────── */

static std::string _replace_placeholder(const std::string& s, int idx,
										const std::string& val)
{
	std::string placeholder = "{";
	placeholder += static_cast<char>('0' + idx);
	placeholder += '}';

	std::string result = s;
	std::string::size_type pos = 0;
	while ((pos = result.find(placeholder, pos)) != std::string::npos)
	{
		result.replace(pos, 3, val);
		pos += val.size();
	}
	return result;
}

std::string fmt(const std::string& pattern) { return pattern; }

std::string fmt(const std::string& pattern, const std::string& a0)
{
	return _replace_placeholder(pattern, 0, a0);
}

std::string fmt(const std::string& pattern, const std::string& a0,
				const std::string& a1)
{
	std::string r = _replace_placeholder(pattern, 0, a0);
	return _replace_placeholder(r, 1, a1);
}

std::string fmt(const std::string& pattern, const std::string& a0,
				const std::string& a1, const std::string& a2)
{
	std::string r = _replace_placeholder(pattern, 0, a0);
	r = _replace_placeholder(r, 1, a1);
	return _replace_placeholder(r, 2, a2);
}

std::string fmt(const std::string& pattern, const std::string& a0,
				const std::string& a1, const std::string& a2,
				const std::string& a3)
{
	std::string r = _replace_placeholder(pattern, 0, a0);
	r = _replace_placeholder(r, 1, a1);
	r = _replace_placeholder(r, 2, a2);
	return _replace_placeholder(r, 3, a3);
}

std::string fmt(const std::string& pattern, const std::string& a0,
				const std::string& a1, const std::string& a2,
				const std::string& a3, const std::string& a4)
{
	std::string r = _replace_placeholder(pattern, 0, a0);
	r = _replace_placeholder(r, 1, a1);
	r = _replace_placeholder(r, 2, a2);
	r = _replace_placeholder(r, 3, a3);
	return _replace_placeholder(r, 4, a4);
}

/* ── String manipulation ───────────────────────────────────────────────── */

std::string trim(const std::string& s)
{
	std::string::size_type start = 0;
	while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start])))
		++start;
	std::string::size_type end = s.size();
	while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1])))
		--end;
	return s.substr(start, end - start);
}

std::string trim_left(const std::string& s)
{
	std::string::size_type start = 0;
	while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start])))
		++start;
	return s.substr(start);
}

std::string trim_right(const std::string& s)
{
	std::string::size_type end = s.size();
	while (end > 0 && std::isspace(static_cast<unsigned char>(s[end - 1])))
		--end;
	return s.substr(0, end);
}

std::string pad_left(const std::string& s, int width, char pad)
{
	if (static_cast<int>(s.size()) >= width) return s;
	return std::string(width - s.size(), pad) + s;
}

std::string pad_right(const std::string& s, int width, char pad)
{
	if (static_cast<int>(s.size()) >= width) return s;
	return s + std::string(width - s.size(), pad);
}

std::string repeat(const std::string& s, int n)
{
	std::string r;
	r.reserve(s.size() * n);
	for (int i = 0; i < n; ++i) r += s;
	return r;
}

std::string join(const std::string* arr, int count, const std::string& sep)
{
	std::string r;
	for (int i = 0; i < count; ++i)
	{
		if (i > 0) r += sep;
		r += arr[i];
	}
	return r;
}

int split(const std::string& s, char delim, std::string* out, int max)
{
	int count = 0;
	std::string::size_type start = 0;
	std::string::size_type pos;

	while (count < max && (pos = s.find(delim, start)) != std::string::npos)
	{
		out[count++] = s.substr(start, pos - start);
		start = pos + 1;
	}
	if (count < max)
		out[count++] = s.substr(start);
	return count;
}

bool starts_with(const std::string& s, const std::string& prefix)
{
	if (prefix.size() > s.size()) return false;
	return s.compare(0, prefix.size(), prefix) == 0;
}

bool ends_with(const std::string& s, const std::string& suffix)
{
	if (suffix.size() > s.size()) return false;
	return s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string replace_all(const std::string& s, const std::string& from,
						const std::string& to)
{
	if (from.empty()) return s;
	std::string result;
	std::string::size_type pos = 0;
	std::string::size_type prev = 0;
	while ((pos = s.find(from, prev)) != std::string::npos)
	{
		result += s.substr(prev, pos - prev);
		result += to;
		prev = pos + from.size();
	}
	result += s.substr(prev);
	return result;
}

bool contains(const std::string& s, const std::string& sub)
{
	return s.find(sub) != std::string::npos;
}

int count(const std::string& s, const std::string& sub)
{
	if (sub.empty()) return 0;
	int n = 0;
	std::string::size_type pos = 0;
	while ((pos = s.find(sub, pos)) != std::string::npos)
	{
		++n;
		pos += sub.size();
	}
	return n;
}

std::string reverse(const std::string& s)
{
	std::string r(s.rbegin(), s.rend());
	return r;
}

std::string center(const std::string& s, int width, char pad)
{
	int len = static_cast<int>(s.size());
	if (len >= width) return s;
	int total = width - len;
	int left = total / 2;
	int right = total - left;
	return std::string(left, pad) + s + std::string(right, pad);
}

std::string truncate(const std::string& s, int max_len,
					 const std::string& suffix)
{
	if (static_cast<int>(s.size()) <= max_len) return s;
	int suf_len = static_cast<int>(suffix.size());
	if (max_len <= suf_len) return suffix.substr(0, max_len);
	return s.substr(0, max_len - suf_len) + suffix;
}

bool is_empty(const std::string& s) { return s.empty(); }

bool is_blank(const std::string& s)
{
	for (std::size_t i = 0; i < s.size(); ++i)
	{
		if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n'
			&& s[i] != '\r' && s[i] != '\f' && s[i] != '\v')
			return false;
	}
	return true;
}

/* ── Message ───────────────────────────────────────────────────────────── */

Message::Message() {}
Message::Message(const Message& o) { _ss.str(o._ss.str()); }
Message& Message::operator=(const Message& o)
{
	if (this != &o) _ss.str(o._ss.str());
	return *this;
}
Message::~Message() {}

std::string Message::str() const { return _ss.str(); }

} /* namespace str */
} /* namespace libcpp */
