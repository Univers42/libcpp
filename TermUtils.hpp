#ifndef TERMUTILS_HPP
# define TERMUTILS_HPP

# include <string>
# include "Srgb.hpp"

/*
 *  TermUtils — shared terminal/UTF-8 utility functions
 *
 *  ┌─────────────────────────────────────────────────────┐
 *  │  All functions are static — no instance needed.      │
 *  │                                                      │
 *  │  Used by TermStyle, TermTable, TermWriter, postman   │
 *  │  to avoid code duplication.                          │
 *  │                                                      │
 *  │  Categories:                                         │
 *  │    • UTF-8 measurement  : visWidth, visLen           │
 *  │    • String fitting     : fitTo, spaces, newlines    │
 *  │    • Glyph tiling       : repCh, tileSep             │
 *  │    • ANSI formatting    : reset, applyFg, applyBg,   │
 *  │                           applyFont                  │
 *  └─────────────────────────────────────────────────────┘
 */

class TermUtils {
    public:
        /* ── font flags (mirrored from TermStyle for convenience) */
        static const int NONE      = 0;
        static const int BOLD      = 1 << 0;
        static const int DIM       = 1 << 1;
        static const int ITALIC    = 1 << 2;
        static const int UNDERLINE = 1 << 3;
        static const int BLINK     = 1 << 4;
        static const int REVERSE   = 1 << 5;
        static const int STRIKE    = 1 << 6;

        /* ── UTF-8 visual width (pure — no ANSI awareness) ─────
         *  Counts display columns for a plain UTF-8 string.
         *  Multi-byte sequences count as 1 column each.        */
        static int visWidth(const std::string& s);

        /* ── UTF-8 visual width (ANSI-aware) ───────────────────
         *  Like visWidth but skips \033[...m escape sequences.
         *  Use this when measuring strings that contain color.  */
        static int visLen(const std::string& s);

        /* ── fit string to exact visual width ──────────────────
         *  If s is wider than w  → truncate to w-1 + '~'
         *  If s is narrower      → right-pad with spaces
         *  If s fits exactly     → return as-is                */
        static std::string fitTo(const std::string& s, int w);

        /* ── repeat a (possibly multi-byte) char n times ───── */
        static std::string repCh(const char* u, int n);

        /* ── tile a multi-byte separator unit to fill width ── */
        static std::string tileSep(const std::string& unit, int width);

        /* ── generate n spaces / n newlines ────────────────── */
        static std::string spaces(int n);
        static std::string newlines(int n);

        /* ── ANSI escape helpers ───────────────────────────── */
        static std::string reset();
        static std::string applyFg(const Srgb& c);
        static std::string applyBg(const Srgb& c);
        static std::string applyFont(int flags);

    private:
        /* pure utility — no instances */
        TermUtils();
        ~TermUtils();
        TermUtils(const TermUtils&);
        TermUtils& operator=(const TermUtils&);
};

#endif
