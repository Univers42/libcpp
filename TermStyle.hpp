#ifndef TERMSTYLE_HPP
# define TERMSTYLE_HPP

# include <string>
# include <sstream>
# include "Srgb.hpp"
# include "TermUtils.hpp"

/*
 *  TermStyle — markdown-like terminal renderer
 *
 *  ┌─────────────────────────────────────────────────────┐
 *  │  Headings  :  ts.h1("Title")  ts.h2()  ts.h3()     │
 *  │  Inline    :  ts.bold()  ts.italic()  ts.dim()      │
 *  │  Blocks    :  ts.quote()  ts.separator()  ts.hr()   │
 *  │  Logs      :  ts.info()  ts.warn()  ts.error()      │
 *  │  Section   :  ts.section("title", "body")           │
 *  └─────────────────────────────────────────────────────┘
 *
 *  Every element type owns an ElemStyle that the user can
 *  fully reconfigure (color, font, glyph, width, padding,
 *  alignment, separator string).
 */

/* ══════════════════════════════════════════════════════════
 *  Unicode glyph constants  (named, not hardcoded)
 * ══════════════════════════════════════════════════════════ */
namespace Glyph {
    // ── bullets / markers ─────────────────────────────────
    static const char BULLET[]    = "\xe2\x97\x8f ";  // ● 
    static const char DIAMOND[]   = "\xe2\x97\x86 ";  // ◆ 
    static const char TRIANGLE[]  = "\xe2\x96\xb8 ";  // ▸ 
    static const char ARROW[]     = "\xe2\x9e\x9c ";  // ➜ 
    static const char STAR[]      = "\xe2\x98\x85 ";  // ★ 

    // ── log level icons ───────────────────────────────────
    static const char INFO[]      = "\xe2\x84\xb9  "; // ℹ  
    static const char WARN[]      = "\xe2\x9a\xa0  "; // ⚠  
    static const char CROSS[]     = "\xe2\x9c\x97 ";  // ✗ 
    static const char CHECK[]     = "\xe2\x9c\x94 ";  // ✔ 
    static const char SKULL[]     = "\xe2\x98\xa0  "; // ☠  (danger / destructor)
    static const char CHAIN[]     = "\xe2\x97\x87 ";  // ◇  (trace / copy)

    // ── box drawing (quote bar) ───────────────────────────
    static const char VBAR[]      = "\xe2\x94\x83 ";  // ┃ 
    static const char VBAR_THIN[] = "\xe2\x94\x82 ";  // │ 

    // ── separator strings  (repeat with std::string(n, c)
    //    won't work for multi-byte, so we store full strings
    //    and tile them in renderSep) ───────────────────────
    static const char HDASH[]     = "\xe2\x94\x80";   // ─
    static const char HDASH_HVY[] = "\xe2\x94\x81";   // ━
    static const char DOUBLE[]    = "\xe2\x95\x90";   // ═
    static const char DOTTED[]    = "\xe2\x95\x8c";   // ╌
}

/* ══════════════════════════════════════════════════════════
 *  TermStyle class
 * ══════════════════════════════════════════════════════════ */
class TermStyle {
    public:
        /* ── alignment ─────────────────────────────────── */
        enum Align { LEFT, CENTER, RIGHT };

        /* ── font flags ────────────────────────────────── */
        static const int NONE      = 0;
        static const int BOLD      = 1 << 0;
        static const int DIM       = 1 << 1;
        static const int ITALIC    = 1 << 2;
        static const int UNDERLINE = 1 << 3;
        static const int BLINK     = 1 << 4;
        static const int REVERSE   = 1 << 5;
        static const int STRIKE    = 1 << 6;

        /* ── per-element style descriptor ──────────────── */
        struct ElemStyle {
            Srgb        fg;
            Srgb        bg;           // background color (used when hasBg=true)
            Srgb        border;
            int         font;
            Align       align;
            int         width;
            int         padL;
            int         padR;
            int         padV;         // vertical padding lines (filled bg)
            int         marginL;
            int         spaceBefore;  // newlines BEFORE element
            int         spaceAfter;   // newlines AFTER element
            bool        hasBg;        // render background color?
            std::string glyph;        // prefix glyph (from Glyph::)
            std::string bodyGlyph;    // body-line glyph (defaults to glyph)
            std::string sepStr;       // separator unit (multi-byte ok)

            ElemStyle();
            ElemStyle(const Srgb& fg_, const Srgb& bg_, const Srgb& brd_,
                      int font_, Align a, int w,
                      int pl, int pr, int pv, int ml,
                      int sb, int sa, bool bg,
                      const std::string& gl, const std::string& sep);
        };

    private:
        ElemStyle _h1;
        ElemStyle _h2;
        ElemStyle _h3;
        ElemStyle _text;
        ElemStyle _infoSt;
        ElemStyle _warnSt;
        ElemStyle _errorSt;
        ElemStyle _successSt;
        ElemStyle _dangerSt;
        ElemStyle _traceSt;
        ElemStyle _quoteSt;
        ElemStyle _sepSt;
        ElemStyle _calloutSt;
        ElemStyle _bulletSt;    // unordered list item
        ElemStyle _olSt;        // ordered list item

    public:
        TermStyle();
        ~TermStyle();

        /* ── style accessors ───────────────────────────── */
        ElemStyle& h1Style();
        ElemStyle& h2Style();
        ElemStyle& h3Style();
        ElemStyle& textStyle();
        ElemStyle& infoStyle();
        ElemStyle& warnStyle();
        ElemStyle& errorStyle();
        ElemStyle& successStyle();
        ElemStyle& dangerStyle();
        ElemStyle& traceStyle();
        ElemStyle& quoteStyle();
        ElemStyle& sepStyle();
        ElemStyle& calloutStyle();
        ElemStyle& bulletStyle();
        ElemStyle& olStyle();

        /* ── headings ──────────────────────────────────── */
        std::string h1(const std::string& title) const;
        std::string h2(const std::string& title) const;
        std::string h3(const std::string& title) const;

        /* ── inline styles ─────────────────────────────── */
        std::string text(const std::string& msg) const;
        std::string bold(const std::string& msg) const;
        std::string italic(const std::string& msg) const;
        std::string underline(const std::string& msg) const;
        std::string dim(const std::string& msg) const;
        std::string strike(const std::string& msg) const;

        /* ── blocks ────────────────────────────────────── */
        std::string quote(const std::string& msg) const;
        std::string separator() const;
        std::string hr() const;
        std::string section(const std::string& title,
                            const std::string& body) const;
        std::string callout(const std::string& label,
                            const std::string& l1,
                            const std::string& l2 = "",
                            const std::string& l3 = "",
                            const std::string& l4 = "",
                            const std::string& l5 = "",
                            const std::string& l6 = "",
                            const std::string& l7 = "",
                            const std::string& l8 = "",
                            const std::string& l9 = "",
                            const std::string& l10 = "") const;

        /* ── log-level shortcuts ───────────────────────── */
        std::string info(const std::string& msg) const;
        std::string warn(const std::string& msg) const;
        std::string error(const std::string& msg) const;
        std::string success(const std::string& msg) const;
        std::string danger(const std::string& msg) const;
        std::string trace(const std::string& msg) const;

        /* ── list items ────────────────────────────────── */
        std::string bullet(const std::string& msg) const;
        std::string ordered(int num, const std::string& msg) const;

    private:
        /* ── helpers (rendering logic, not duplicated) ──── */
        std::string alignText(const std::string& txt,
                              int available, Align a) const;
        std::string renderLine(const ElemStyle& es,
                               const std::string& content) const;
        std::string renderBgLine(const ElemStyle& es,
                                 const std::string& content) const;
        std::string renderBgBlank(const ElemStyle& es) const;
        std::string renderSep(const ElemStyle& es) const;
        std::string renderHeading(const ElemStyle& es,
                                  const std::string& title) const;

    public:
        /* ── backward-compatible static delegates ──────── *
         *  All forward to TermUtils.  Kept so existing      *
         *  callers (TermWriter, tests) don't break.        */
        static int         visLen(const std::string& s);
        static std::string reset();
        static std::string tileSep(const std::string& unit, int width);
        static std::string newlines(int n);
};

#endif
