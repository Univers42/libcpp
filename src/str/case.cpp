/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   case.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/str/case.hpp"
#include "libcpp/str/utf8.hpp"
#include <cctype>
#include <cwctype>
#include <stdint.h>

namespace libcpp
{
namespace str
{

/* ── Latin supplement case mapping tables ──────────────────────────────── */

struct MapPair { uint32_t from; uint32_t to; };

static const MapPair LATIN_UPPER[] = {
	{0x00E0u, 0x00C0u}, {0x00E1u, 0x00C1u}, {0x00E2u, 0x00C2u},
	{0x00E3u, 0x00C3u}, {0x00E4u, 0x00C4u}, {0x00E5u, 0x00C5u},
	{0x00E6u, 0x00C6u}, {0x00E7u, 0x00C7u}, {0x00E8u, 0x00C8u},
	{0x00E9u, 0x00C9u}, {0x00EAu, 0x00CAu}, {0x00EBu, 0x00CBu},
	{0x00ECu, 0x00CCu}, {0x00EDu, 0x00CDu}, {0x00EEu, 0x00CEu},
	{0x00EFu, 0x00CFu}, {0x00F0u, 0x00D0u}, {0x00F1u, 0x00D1u},
	{0x00F2u, 0x00D2u}, {0x00F3u, 0x00D3u}, {0x00F4u, 0x00D4u},
	{0x00F5u, 0x00D5u}, {0x00F6u, 0x00D6u}, {0x00F8u, 0x00D8u},
	{0x00F9u, 0x00D9u}, {0x00FAu, 0x00DAu}, {0x00FBu, 0x00DBu},
	{0x00FCu, 0x00DCu}, {0x00FDu, 0x00DDu}, {0x00FEu, 0x00DEu}
};
static const int LATIN_UPPER_N = sizeof(LATIN_UPPER) / sizeof(LATIN_UPPER[0]);

static const MapPair LATIN_LOWER[] = {
	{0x00C0u, 0x00E0u}, {0x00C1u, 0x00E1u}, {0x00C2u, 0x00E2u},
	{0x00C3u, 0x00E3u}, {0x00C4u, 0x00E4u}, {0x00C5u, 0x00E5u},
	{0x00C6u, 0x00E6u}, {0x00C7u, 0x00E7u}, {0x00C8u, 0x00E8u},
	{0x00C9u, 0x00E9u}, {0x00CAu, 0x00EAu}, {0x00CBu, 0x00EBu},
	{0x00CCu, 0x00ECu}, {0x00CDu, 0x00EDu}, {0x00CEu, 0x00EEu},
	{0x00CFu, 0x00EFu}, {0x00D0u, 0x00F0u}, {0x00D1u, 0x00F1u},
	{0x00D2u, 0x00F2u}, {0x00D3u, 0x00F3u}, {0x00D4u, 0x00F4u},
	{0x00D5u, 0x00F5u}, {0x00D6u, 0x00F6u}, {0x00D8u, 0x00F8u},
	{0x00D9u, 0x00F9u}, {0x00DAu, 0x00FAu}, {0x00DBu, 0x00FBu},
	{0x00DCu, 0x00FCu}, {0x00DDu, 0x00FDu}, {0x00DEu, 0x00FEu}
};
static const int LATIN_LOWER_N = sizeof(LATIN_LOWER) / sizeof(LATIN_LOWER[0]);

static uint32_t _lookup(const MapPair* table, int n, uint32_t cp)
{
	for (int i = 0; i < n; ++i)
		if (table[i].from == cp) return table[i].to;
	return cp;
}

static uint32_t _to_upper_cp(uint32_t cp)
{
	if (cp <= 0x7F)
		return static_cast<uint32_t>(std::toupper(static_cast<unsigned char>(cp)));
	uint32_t mapped = _lookup(LATIN_UPPER, LATIN_UPPER_N, cp);
	if (mapped != cp) return mapped;
	wint_t up = std::towupper(static_cast<wint_t>(cp));
	if (up != static_cast<wint_t>(cp)) return static_cast<uint32_t>(up);
	return cp;
}

static uint32_t _to_lower_cp(uint32_t cp)
{
	if (cp <= 0x7F)
		return static_cast<uint32_t>(std::tolower(static_cast<unsigned char>(cp)));
	uint32_t mapped = _lookup(LATIN_LOWER, LATIN_LOWER_N, cp);
	if (mapped != cp) return mapped;
	wint_t lo = std::towlower(static_cast<wint_t>(cp));
	if (lo != static_cast<wint_t>(cp)) return static_cast<uint32_t>(lo);
	return cp;
}

static bool _is_lower_cp(uint32_t cp)
{
	if (cp <= 0x7F) return std::islower(static_cast<unsigned char>(cp)) != 0;
	return std::iswlower(static_cast<wint_t>(cp)) != 0;
}

static bool _is_upper_cp(uint32_t cp)
{
	if (cp <= 0x7F) return std::isupper(static_cast<unsigned char>(cp)) != 0;
	return std::iswupper(static_cast<wint_t>(cp)) != 0;
}

/* ── Public API ────────────────────────────────────────────────────────── */

std::string to_upper(const std::string& s)
{
	std::string out;
	out.reserve(s.size());
	std::size_t idx = 0;
	while (idx < s.size())
	{
		uint32_t cp = decode_utf8(s, idx);
		append_utf8(out, _to_upper_cp(cp));
	}
	return out;
}

std::string to_lower(const std::string& s)
{
	std::string out;
	out.reserve(s.size());
	std::size_t idx = 0;
	while (idx < s.size())
	{
		uint32_t cp = decode_utf8(s, idx);
		append_utf8(out, _to_lower_cp(cp));
	}
	return out;
}

std::string to_title(const std::string& s)
{
	std::string out;
	out.reserve(s.size());
	bool word_start = true;
	std::size_t idx = 0;
	while (idx < s.size())
	{
		uint32_t cp = decode_utf8(s, idx);
		bool is_letter = _is_lower_cp(cp) || _is_upper_cp(cp);
		if (word_start && is_letter)
		{
			append_utf8(out, _to_upper_cp(cp));
			word_start = false;
		}
		else
		{
			append_utf8(out, _to_lower_cp(cp));
			if (!is_letter) word_start = true;
		}
	}
	return out;
}

std::string toggle_case(const std::string& s)
{
	std::string out;
	out.reserve(s.size());
	std::size_t idx = 0;
	while (idx < s.size())
	{
		uint32_t cp = decode_utf8(s, idx);
		if (_is_lower_cp(cp))
			append_utf8(out, _to_upper_cp(cp));
		else if (_is_upper_cp(cp))
			append_utf8(out, _to_lower_cp(cp));
		else
			append_utf8(out, cp);
	}
	return out;
}

bool eq_nocase(const std::string& a, const std::string& b)
{
	return to_lower(a) == to_lower(b);
}

/* ── Case style conversions ────────────────────────────────────────────── */

static bool _is_sep(char c)
{
	return c == '_' || c == '-' || c == ' ' || c == '\t';
}

/* Break string into words based on separators and camelCase boundaries */
static int _tokenize(const std::string& s, std::string* words, int max)
{
	int count = 0;
	std::string cur;

	for (std::size_t i = 0; i < s.size() && count < max; ++i)
	{
		char c = s[i];
		if (_is_sep(c))
		{
			if (!cur.empty()) { words[count++] = cur; cur.clear(); }
			continue;
		}
		/* detect camelCase boundary */
		if (std::isupper(static_cast<unsigned char>(c)) && !cur.empty() &&
			std::islower(static_cast<unsigned char>(cur[cur.size() - 1])))
		{
			words[count++] = cur;
			cur.clear();
		}
		cur += c;
	}
	if (!cur.empty() && count < max)
		words[count++] = cur;
	return count;
}

std::string to_snake_case(const std::string& s)
{
	std::string words[64];
	int n = _tokenize(s, words, 64);
	for (int i = 0; i < n; ++i) words[i] = to_lower(words[i]);
	std::string sep = "_";
	std::string r;
	for (int i = 0; i < n; ++i) { if (i > 0) r += sep; r += words[i]; }
	return r;
}

std::string to_camel_case(const std::string& s)
{
	std::string words[64];
	int n = _tokenize(s, words, 64);
	std::string r;
	for (int i = 0; i < n; ++i)
	{
		if (i == 0)
			r += to_lower(words[i]);
		else
		{
			std::string w = to_lower(words[i]);
			if (!w.empty())
				w[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(w[0])));
			r += w;
		}
	}
	return r;
}

std::string to_pascal_case(const std::string& s)
{
	std::string words[64];
	int n = _tokenize(s, words, 64);
	std::string r;
	for (int i = 0; i < n; ++i)
	{
		std::string w = to_lower(words[i]);
		if (!w.empty())
			w[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(w[0])));
		r += w;
	}
	return r;
}

std::string to_kebab_case(const std::string& s)
{
	std::string words[64];
	int n = _tokenize(s, words, 64);
	for (int i = 0; i < n; ++i) words[i] = to_lower(words[i]);
	std::string sep = "-";
	std::string r;
	for (int i = 0; i < n; ++i) { if (i > 0) r += sep; r += words[i]; }
	return r;
}

} /* namespace str */
} /* namespace libcpp */
