/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_string.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 21:23:49 by marvin            #+#    #+#             */
/*   Updated: 2025/12/23 21:23:49 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_string.hpp"

#include <locale>
#if defined(__cpp_lib_codecvt) || defined(_MSC_VER)
#include <codecvt>
#endif
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <cwctype>
#include <clocale>
#include <vector>
#include <cstring>
#include <wchar.h>  // wcwidth

// initialize global locale from environment once (prefer UTF-8)
static void ensure_locale_initialized()
{
    static bool inited = false;
    if (inited) return;
    inited = true;
    /* Try environment / common UTF-8 locales; fall back silently if none available. */
    const char *candidates[] = { "", "C.UTF-8", "en_US.UTF-8", "UTF-8", NULL };
    const char *chosen = NULL;
    for (const char **p = candidates; *p; ++p) {
        if (setlocale(LC_CTYPE, *p) != NULL) {
            chosen = *p;
            break;
        }
    }
    try {
        if (chosen && chosen[0] != '\0')
            std::locale::global(std::locale(chosen));
        else
            std::locale::global(std::locale(""));
    } catch (...) {
        try { std::locale::global(std::locale("")); } catch(...) {}
    }
}

namespace testing
{
    namespace internal
    {

        const char *String::CloneCString(const char *c_str)
        {
            if (!c_str)
                return NULL;
            size_t len = std::strlen(c_str);
            char *copy = new char[len + 1];
            std::memcpy(copy, c_str, len + 1);
            return copy;
        }

        bool String::CStringEquals(const char *lhs, const char *rhs)
        {
            if (lhs == rhs)
                return true; // covers both nullptr
            if (!lhs || !rhs)
                return false;
            return std::strcmp(lhs, rhs) == 0;
        }

        std::string String::ShowWideCString(const wchar_t *wide_c_str)
        {
            if (!wide_c_str)
                return std::string("(null)");
#if defined(__cpp_lib_codecvt) || defined(_MSC_VER)
            try
            {
                std::wstring ws(wide_c_str);
                // use codecvt to convert wchar_t -> UTF-8 when available
                std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conv;
                return conv.to_bytes(ws);
            }
            catch (...)
            {
                return std::string("(failed to convert from wide string)");
            }
#else
            // Fallback: narrow each wchar via wctomb / wcsrtombs (locale dependent)
            std::string out;
            std::mbstate_t state = std::mbstate_t();
            const wchar_t *src = wide_c_str;
            size_t len = std::wcslen(wide_c_str);
            std::vector<char> buf(len * MB_CUR_MAX + 1);
            const wchar_t *p = src;
            char *q = buf.data();
            size_t ret = std::wcsrtombs(q, &p, buf.size(), &state);
            if (ret == static_cast<size_t>(-1))
                return "(failed to convert from wide string)";
            return std::string(q, ret);
#endif
        }

        bool String::WideCStringEquals(const wchar_t *lhs, const wchar_t *rhs)
        {
            if (lhs == rhs)
                return true;
            if (!lhs || !rhs)
                return false;
            return std::wcscmp(lhs, rhs) == 0;
        }

        bool String::CaseInsensitiveCStringEquals(const char *lhs, const char *rhs)
        {
            if (lhs == rhs)
                return true;
            if (!lhs || !rhs)
                return false;
            while (*lhs && *rhs)
            {
                unsigned char a = static_cast<unsigned char>(*lhs);
                unsigned char b = static_cast<unsigned char>(*rhs);
                if (std::tolower(a) != std::tolower(b))
                    return false;
                ++lhs;
                ++rhs;
            }
            return *lhs == *rhs;
        }

        bool String::CaseInsensitiveWideCStringEquals(const wchar_t *lhs, const wchar_t *rhs)
        {
            if (lhs == rhs)
                return true;
            if (!lhs || !rhs)
                return false;
            while (*lhs && *rhs)
            {
                if (std::towlower(*lhs) != std::towlower(*rhs))
                    return false;
                ++lhs;
                ++rhs;
            }
            return *lhs == *rhs;
        }

        bool String::EndsWithCaseInsensitive(const std::string &str, const std::string &suffix)
        {
            if (suffix.size() > str.size())
                return false;
            size_t offset = str.size() - suffix.size();
            for (size_t i = 0; i < suffix.size(); ++i)
            {
                char a = std::tolower(static_cast<unsigned char>(str[offset + i]));
                char b = std::tolower(static_cast<unsigned char>(suffix[i]));
                if (a != b)
                    return false;
            }
            return true;
        }

        std::string String::FormatIntWidth2(int val)
        {
            std::ostringstream ss;
            ss << std::setw(2) << std::setfill('0') << val;
            return ss.str();
        }

        std::string String::FormatIntWidthN(int val, int width)
        {
            std::ostringstream ss;
            if (width < 0)
                width = 0;
            ss << std::setw(width) << std::setfill('0') << val;
            return ss.str();
        }

        std::string String::FormatHexInt(int val)
        {
            std::ostringstream ss;
            ss << std::uppercase << std::hex << val;
            return ss.str();
        }

        std::string String::FormatHexUint32(uint32_t val)
        {
            std::ostringstream ss;
            ss << std::uppercase << std::hex << val;
            return ss.str();
        }

        std::string String::FormatByte(unsigned char value)
        {
            std::ostringstream ss;
            ss << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value);
            return ss.str();
        }

        std::string StringStreamToString(::std::stringstream *stream)
        {
            if (!stream)
                return std::string();
            return stream->str();
        }

    } // namespace internal
} // namespace testing

std::wstring ft_str_to_wstr(const std::string &s)
{
	std::wstring ws;
	size_t idx = 0;
	while (idx < s.size()) {
		uint32_t cp = decode_utf8_at(s, &idx);
		if (cp == 0xFFFD) {
			ws.push_back(L'\uFFFD');
			continue;
		}
		if (cp <= 0xFFFF) {
			ws.push_back(static_cast<wchar_t>(cp));
		} else {
			// Encode as surrogate pair
			cp -= 0x10000;
			wchar_t high = static_cast<wchar_t>((cp >> 10) + 0xD800);
			wchar_t low = static_cast<wchar_t>((cp & 0x3FF) + 0xDC00);
			ws.push_back(high);
			ws.push_back(low);
		}
	}
	return ws;
}

// Helper: append a Unicode codepoint (U+) as UTF-8 to a string
void	append_utf8(std::string *out, uint32_t cp)
{
    if (cp <= 0x7F) {
        out->push_back(static_cast<char>(cp));
    } else if (cp <= 0x7FF) {
        out->push_back(static_cast<char>(0xC0 |
            ((cp >> 6) & 0x1F)));
        out->push_back(static_cast<char>(0x80 |
            (cp & 0x3F)));
    } else if (cp <= 0xFFFF) {
        out->push_back(static_cast<char>(0xE0 |
            ((cp >> 12) & 0x0F)));
        out->push_back(static_cast<char>(0x80 |
            ((cp >> 6) & 0x3F)));
        out->push_back(static_cast<char>(0x80 |
            (cp & 0x3F)));
    } else {
        out->push_back(static_cast<char>(0xF0 |
            ((cp >> 18) & 0x07)));
        out->push_back(static_cast<char>(0x80 |
            ((cp >> 12) & 0x3F)));
        out->push_back(static_cast<char>(0x80 |
            ((cp >> 6) & 0x3F)));
        out->push_back(static_cast<char>(0x80 |
            (cp & 0x3F)));
    }
}

// Helper: decode next UTF-8 codepoint from s at index *idx.
// Returns codepoint and advances *idx. On invalid sequence returns 0xFFFD.
uint32_t decode_utf8_at(const std::string &s, size_t *idx)
{
    const unsigned char *data = (const unsigned char *)s.data();
    size_t n = s.size();
    size_t i = *idx;
    if (i >= n) {
        return 0;
    }
    unsigned char c = data[i];
    if (c < 0x80) {
        *idx = i + 1;
        return c;
    }
    size_t len = 0;
    uint32_t cp = 0;
    if ((c & 0xE0) == 0xC0) {
        cp = c & 0x1F;
        len = 2;
    } else if ((c & 0xF0) == 0xE0) {
        cp = c & 0x0F;
        len = 3;
    } else if ((c & 0xF8) == 0xF0) {
        cp = c & 0x07;
        len = 4;
    } else {
        *idx = i + 1;
        return 0xFFFD;
    }
    if (i + len > n) {
        *idx = i + 1;
        return 0xFFFD;
    }
    for (size_t k = 1; k < len; ++k) {
        unsigned char cc = data[i + k];
        if ((cc & 0xC0) != 0x80) {
            *idx = i + 1;
            return 0xFFFD;
        }
        cp = (cp << 6) | (cc & 0x3F);
    }
    *idx = i + len;
    return cp;
}

// Minimal case-mapping tables (definitions moved into this translation unit to satisfy linker)
const MapPair LATIN_UPPER[] = {
    {0x00E0u, 0x00C0u}, // à -> À
    {0x00E1u, 0x00C1u}, // á -> Á
    {0x00E9u, 0x00C9u}, // é -> É
    {0x00F1u, 0x00D1u}, // ñ -> Ñ
    {0x00FCu, 0x00DCu}  // ü -> Ü
};
const size_t LATIN_UPPER_N = sizeof(LATIN_UPPER) / sizeof(LATIN_UPPER[0]);

const MapPair LATIN_LOWER[] = {
    {0x00C0u, 0x00E0u}, // À -> à
    {0x00C1u, 0x00E1u}, // Á -> á
    {0x00C9u, 0x00E9u}, // É -> é
    {0x00D1u, 0x00F1u}, // Ñ -> ñ
    {0x00DCu, 0x00FCu}  // Ü -> ü
};
const size_t LATIN_LOWER_N = sizeof(LATIN_LOWER) / sizeof(LATIN_LOWER[0]);

// Linear lookup helpers
static uint32_t lookup_upper(uint32_t cp)
{
    for (size_t i = 0; i < LATIN_UPPER_N; ++i) {
        if (LATIN_UPPER[i].from == cp) {
            return LATIN_UPPER[i].to;
        }
    }
    return cp;
}

static uint32_t lookup_lower(uint32_t cp)
{
    for (size_t i = 0; i < LATIN_LOWER_N; ++i) {
        if (LATIN_LOWER[i].from == cp) {
            return LATIN_LOWER[i].to;
        }
    }
    return cp;
}

void strcase_toggle(std::string *s, int mod)
{
    ensure_locale_initialized();
    std::string out;
    out.reserve(s->size());

    size_t idx = 0;
    while (idx < s->size()) {
        uint32_t cp = decode_utf8_at(*s, &idx);
        if (cp == 0xFFFD) {
            append_utf8(&out, 0xFFFD);
            continue;
        }

        if (cp <= 0x7F) {
            unsigned char ch = static_cast<unsigned char>(cp);
            if (mod == 0) {
                cp = static_cast<unsigned char>(std::toupper(ch));
            } else if (mod == 1) {
                cp = static_cast<unsigned char>(std::tolower(ch));
            } else {
                if (std::islower(ch)) {
                    cp = static_cast<unsigned char>(std::toupper(ch));
                } else if (std::isupper(ch)) {
                    cp = static_cast<unsigned char>(std::tolower(ch));
                }
            }
            append_utf8(&out, cp);
            continue;
        }

        wint_t w = static_cast<wint_t>(cp);
        if (mod == 0) {
            uint32_t outcp = lookup_upper(cp);
            // Always try towupper as fallback for all non-ASCII
            wint_t up = std::towupper(w);
            if (outcp != cp)
                append_utf8(&out, outcp);
            else if (up != w)
                append_utf8(&out, static_cast<uint32_t>(up));
            else
                append_utf8(&out, cp);
        } else if (mod == 1) {
            uint32_t outcp = lookup_lower(cp);
            wint_t lo = std::towlower(w);
            if (outcp != cp)
                append_utf8(&out, outcp);
            else if (lo != w)
                append_utf8(&out, static_cast<uint32_t>(lo));
            else
                append_utf8(&out, cp);
        } else {
            uint32_t up = lookup_upper(cp);
            uint32_t lo = lookup_lower(cp);
            wint_t upc = std::towupper(w);
            wint_t loc = std::towlower(w);
            if (up != cp)
                append_utf8(&out, up);
            else if (lo != cp)
                append_utf8(&out, lo);
            else if (std::iswlower(w) && upc != w)
                append_utf8(&out, static_cast<uint32_t>(upc));
            else if (std::iswupper(w) && loc != w)
                append_utf8(&out, static_cast<uint32_t>(loc));
            else
                append_utf8(&out, cp);
        }
    }

    s->swap(out);
}

// NOTE: Unicode helper implementations removed from this .cpp to avoid multiple-definition
// (they are now provided as inline functions in ft_string.hpp)