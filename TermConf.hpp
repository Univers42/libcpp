#ifndef TERMCONF_HPP
# define TERMCONF_HPP

# include "TermStyle.hpp"
# include "Srgb.hpp"

/*
 *  TermConf — centralized configuration for TermStyle
 *
 *  Instead of hardcoding spacing, colors, and glyphs everywhere,
 *  the user configures ONE TermConf object and applies it to a
 *  TermStyle instance.  Several presets are provided.
 *
 *  ┌─────────────────────────────────────────────────────┐
 *  │  Usage:                                              │
 *  │    TermStyle ts;                                     │
 *  │    TermConf::applyDefault(ts);  // default preset    │
 *  │    TermConf::applyMinimal(ts);  // minimal preset    │
 *  │    TermConf::applyCompact(ts);  // compact preset    │
 *  │                                                      │
 *  │  Or create your own:                                 │
 *  │    TermConf conf;                                    │
 *  │    conf.globalWidth = 80;                            │
 *  │    conf.h1SpaceBefore = 2;                           │
 *  │    conf.apply(ts);                                   │
 *  └─────────────────────────────────────────────────────┘
 */

class TermConf {
    public:
        /* ── global layout ────────────────────────────── */
        int         globalWidth;        // width for all elements

        /* ── heading spacing ──────────────────────────── */
        int         h1SpaceBefore;
        int         h1SpaceAfter;
        int         h2SpaceBefore;
        int         h2SpaceAfter;
        int         h3SpaceBefore;
        int         h3SpaceAfter;

        /* ── log spacing ──────────────────────────────── */
        int         logSpaceBefore;
        int         logSpaceAfter;

        /* ── block spacing ────────────────────────────── */
        int         sepSpaceBefore;
        int         sepSpaceAfter;
        int         quoteSpaceBefore;
        int         quoteSpaceAfter;
        int         sectionSpaceBefore;
        int         sectionSpaceAfter;
        int         textSpaceBefore;
        int         textSpaceAfter;

        /* ── heading colors ───────────────────────────── */
        Srgb        h1Fg;
        Srgb        h1Border;
        Srgb        h2Fg;
        Srgb        h2Border;
        Srgb        h3Fg;
        Srgb        h3Border;

        /* ── heading fonts ────────────────────────────── */
        int         h1Font;
        int         h2Font;
        int         h3Font;

        /* ── heading glyphs ───────────────────────────── */
        std::string h1Glyph;
        std::string h2Glyph;
        std::string h3Glyph;

        /* ── heading separators ───────────────────────── */
        std::string h1Sep;
        std::string h2Sep;
        std::string h3Sep;

        /* ── heading alignment ────────────────────────── */
        TermStyle::Align h1Align;
        TermStyle::Align h2Align;
        TermStyle::Align h3Align;

        /* ── log colors ───────────────────────────────── */
        Srgb        infoFg;
        Srgb        warnFg;
        Srgb        errorFg;
        Srgb        successFg;

        /* ── log glyphs ───────────────────────────────── */
        std::string infoGlyph;
        std::string warnGlyph;
        std::string errorGlyph;
        std::string successGlyph;

        /* ── quote ────────────────────────────────────── */
        Srgb        quoteFg;
        std::string quoteGlyph;

        /* ── separator ────────────────────────────────── */
        Srgb        sepBorder;
        std::string sepStr;

        /* ── text ─────────────────────────────────────── */
        Srgb        textFg;

        /* ── list ─────────────────────────────────────── */
        Srgb        bulletFg;
        std::string bulletGlyph;
        Srgb        olFg;

        /* ── callout ──────────────────────────────────── */
        Srgb        calloutFg;
        Srgb        calloutBg;
        Srgb        calloutBorder;
        std::string calloutGlyph;
        std::string calloutBodyGlyph;
        bool        calloutHasBg;

        /* ── h1 background ────────────────────────────── */
        Srgb        h1Bg;
        bool        h1HasBg;
        int         h1PadV;

        TermConf();
        ~TermConf();

        /* ── apply this config to a TermStyle ─────────── */
        void apply(TermStyle& ts) const;

        /* ── presets (static factory methods) ──────────── */
        static void applyDefault(TermStyle& ts);
        static void applyMinimal(TermStyle& ts);
        static void applyCompact(TermStyle& ts);
        static void applyWide(TermStyle& ts);
};

#endif
