#include "TermUtils.hpp"

/* ═══════════════════════════════════════════════════════════
 *  visWidth — pure UTF-8 visual width (no ANSI awareness)
 *
 *  Correctly handles multi-byte UTF-8 sequences:
 *  - ASCII (0x00-0x7F): 1 byte, 1 column
 *  - 2-byte (0xC0-0xDF): 2 bytes, 1 column
 *  - 3-byte (0xE0-0xEF): 3 bytes, 1 column (e.g., box-drawing)
 *  - 4-byte (0xF0-0xF7): 4 bytes, 1 column
 *  - Continuation (0x80-0xBF): 0 columns (part of multibyte)
 * ═══════════════════════════════════════════════════════════ */

int TermUtils::visWidth(const std::string& s) {
    int w = 0;
    for (std::string::size_type i = 0; i < s.size(); ) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if      (c < 0x80) { ++w; i += 1; }
        else if (c < 0xC0) { i += 1; }
        else if (c < 0xE0) { ++w; i += 2; }
        else if (c < 0xF0) { ++w; i += 3; }
        else               { ++w; i += 4; }
    }
    return w;
}

/* ═══════════════════════════════════════════════════════════
 *  visLen — ANSI-aware UTF-8 visual width
 *
 *  Same as visWidth but skips \033[...m escape sequences,
 *  so it works on strings that contain ANSI color codes.
 * ═══════════════════════════════════════════════════════════ */

int TermUtils::visLen(const std::string& s) {
    int len = 0;
    bool inEsc = false;
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\033') {
            inEsc = true;
        } else if (inEsc) {
            if (s[i] == 'm') inEsc = false;
        } else {
            if ((static_cast<unsigned char>(s[i]) & 0xC0) != 0x80)
                ++len;
        }
    }
    return len;
}

/* ═══════════════════════════════════════════════════════════
 *  fitTo — fit a string to exact visual width
 *
 *  If the string is longer than w: truncate to w-1 + '~'
 *  If shorter: right-pad with spaces
 *  If exact: return as-is
 * ═══════════════════════════════════════════════════════════ */

std::string TermUtils::fitTo(const std::string& s, int w) {
    if (w <= 0) return "";
    std::string out;
    int vis = 0;
    for (std::string::size_type i = 0; i < s.size(); ) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        int len = 1;
        if      (c >= 0xF0) len = 4;
        else if (c >= 0xE0) len = 3;
        else if (c >= 0xC0) len = 2;
        if (c >= 0x80 && c < 0xC0) { ++i; continue; }
        if (vis + 1 > w) break;
        bool more = (i + static_cast<std::string::size_type>(len)) < s.size();
        if (vis + 1 == w && more) {
            bool onlySpaces = true;
            for (std::string::size_type j = i + len; j < s.size(); ++j)
                if (s[j] != ' ') { onlySpaces = false; break; }
            if (!onlySpaces) { out += '~'; ++vis; break; }
        }
        out.append(s, i, static_cast<std::string::size_type>(len));
        ++vis;
        i += static_cast<std::string::size_type>(len);
    }
    while (vis < w) { out += ' '; ++vis; }
    return out;
}

/* ═══════════════════════════════════════════════════════════
 *  repCh — repeat a (possibly multi-byte) UTF-8 char n times
 * ═══════════════════════════════════════════════════════════ */

std::string TermUtils::repCh(const char* u, int n) {
    std::string o;
    for (int i = 0; i < n; ++i) o += u;
    return o;
}

/* ═══════════════════════════════════════════════════════════
 *  tileSep — tile a multi-byte separator unit to fill width
 * ═══════════════════════════════════════════════════════════ */

std::string TermUtils::tileSep(const std::string& unit, int width) {
    if (unit.empty() || width <= 0)
        return "";
    std::string result;
    int unitVis = visLen(unit);
    if (unitVis <= 0) unitVis = 1;
    int repeats = width / unitVis;
    for (int i = 0; i < repeats; ++i)
        result += unit;
    return result;
}

/* ═══════════════════════════════════════════════════════════
 *  spaces / newlines
 * ═══════════════════════════════════════════════════════════ */

std::string TermUtils::spaces(int n) {
    if (n <= 0) return "";
    return std::string(n, ' ');
}

std::string TermUtils::newlines(int n) {
    if (n <= 0) return "";
    return std::string(n, '\n');
}

/* ═══════════════════════════════════════════════════════════
 *  ANSI escape helpers
 * ═══════════════════════════════════════════════════════════ */

std::string TermUtils::reset() { return "\033[0m"; }

std::string TermUtils::applyFg(const Srgb& c) {
    return c.toAnsi();
}

std::string TermUtils::applyBg(const Srgb& c) {
    return c.toBgAnsi();
}

std::string TermUtils::applyFont(int flags) {
    std::string r;
    if (flags & BOLD)      r += "\033[1m";
    if (flags & DIM)       r += "\033[2m";
    if (flags & ITALIC)    r += "\033[3m";
    if (flags & UNDERLINE) r += "\033[4m";
    if (flags & BLINK)     r += "\033[5m";
    if (flags & REVERSE)   r += "\033[7m";
    if (flags & STRIKE)    r += "\033[9m";
    return r;
}
