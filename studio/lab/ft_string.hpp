/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_string.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 18:30:53 by marvin            #+#    #+#             */
/*   Updated: 2025/12/23 18:30:53 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_STRING_HPP
#define FT_STRING_HPP

#include <wchar.h>  // wcwidth
#include <string>
#include <vector>

// Minimal, C++98-friendly header for ft_string.cpp
#include <string>
#include <sstream>
#include <cstddef>
#include <wchar.h>
#include <stdint.h> /* use C header to avoid <cstdint> C++11 requirement */

#ifdef __cplusplus
extern "C" {
#endif

/* Map pair used for simple case-mapping tables */
typedef struct {
    uint32_t from;
    uint32_t to;
} MapPair;

/* Declaration of case-mapping tables (definitions may live in another unit) */
extern const MapPair LATIN_UPPER[];
extern const size_t LATIN_UPPER_N;
extern const MapPair LATIN_LOWER[];
extern const size_t LATIN_LOWER_N;

#ifdef __cplusplus
}
#endif

namespace testing {
namespace internal {

class String {
public:
    static const char* CloneCString(const char* c_str);
    static bool CStringEquals(const char* lhs, const char* rhs);
    static std::string ShowWideCString(const wchar_t* wide_c_str);
    static bool WideCStringEquals(const wchar_t* lhs, const wchar_t* rhs);
    static bool CaseInsensitiveCStringEquals(const char* lhs, const char* rhs);
    static bool CaseInsensitiveWideCStringEquals(const wchar_t* lhs, const wchar_t* rhs);
    static bool EndsWithCaseInsensitive(const std::string &str, const std::string &suffix);
    static std::string FormatIntWidth2(int val);
    static std::string FormatIntWidthN(int val, int width);
    static std::string FormatHexInt(int val);
    static std::string FormatHexUint32(uint32_t val);
    static std::string FormatByte(unsigned char value);
};

std::string StringStreamToString(::std::stringstream *stream);

} // namespace internal
} // namespace testing

/* UTF-8 / wide helpers */
std::wstring ft_str_to_wstr(const std::string &s);
void append_utf8(std::string *out, uint32_t cp);
uint32_t decode_utf8_at(const std::string &s, size_t *idx);
void strcase_toggle(std::string *s, int mod);

/* Unicode helpers (inline in header to allow inclusion from multiple TUs) */
namespace Unicode {

// Convert UTF-8 to wstring (uses ft_str_to_wstr implemented in ft_string.cpp)
inline std::wstring utf8_to_wstring_local(const std::string &s) {
    return ft_str_to_wstr(s);
}

} // namespace Unicode

#endif // FT_STRING_HPP