/* ************************************************************************** */
/*  theme_nord.hpp -- Nord color theme configuration                          */
/*                                                                            */
/*  Drop-in theme header. Include and call nord_theme().apply(ts)             */
/*  to skin a TermStyle with Nord's arctic color palette.                     */
/*                                                                            */
/*  Palette reference:  https://www.nordtheme.com                             */
/*    Polar Night  #2E3440  #3B4252  #434C5E  #4C566A                        */
/*    Snow Storm   #D8DEE9  #E5E9F0  #ECEFF4                                 */
/*    Frost        #8FBCBB  #88C0D0  #81A1C1  #5E81AC                        */
/*    Aurora       #BF616A  #D08770  #EBCB8B  #A3BE8C  #B48EAD               */
/* ************************************************************************** */

#ifndef THEME_NORD_HPP
# define THEME_NORD_HPP

# include "libcpp/term/stylesheet.hpp"

LIBCPP_THEME_BEGIN(nord_theme)

    /* ── Headings ─────────────────────────────────────────────── */
    RULE("h1",
        fg("#ECEFF4").bg("#2E3440").border("#88C0D0")
        .bold().center()
        .pad(3, 3, 1).spacing(1, 1)
        .glyph("\xe2\x97\x87 ")       /* ◇ */
        .sep("\xe2\x95\x90")           /* ═ */
    )
    RULE("h2",
        fg("#81A1C1").border("#4C566A")
        .bold()
        .pad(1, 1).spacing(1, 0)
        .glyph("\xe2\x97\x86 ")       /* ◆ */
        .sep("\xe2\x94\x81")           /* ━ */
    )
    RULE("h3",
        fg("#A3BE8C").border("#4C566A")
        .bold()
        .pad(1, 1).spacing(1, 0)
        .glyph("\xe2\x96\xb8 ")       /* ▸ */
        .sep("\xe2\x95\x8c")           /* ╌ */
    )

    /* ── Body & text ──────────────────────────────────────────── */
    RULE("body",  fg("#D8DEE9").pad_left(2))
    RULE("quote", fg("#616E88").italic().glyph("\xe2\x94\x82 ").margin_left(3))

    /* ── Log levels ───────────────────────────────────────────── */
    RULE("info",    fg("#88C0D0").glyph("\xe2\x84\xb9  "))
    RULE("warn",    fg("#EBCB8B").bold().glyph("\xe2\x9a\xa0  "))
    RULE("error",   fg("#BF616A").bold().glyph("\xe2\x9c\x97 "))
    RULE("success", fg("#A3BE8C").glyph("\xe2\x9c\x94 "))
    RULE("danger",  fg("#BF616A").bold().glyph("\xe2\x98\xa0  "))
    RULE("trace",   fg("#B48EAD").dim().glyph("\xe2\x97\x87 "))

    /* ── Separators & lists ───────────────────────────────────── */
    RULE("sep",     border("#4C566A").sep("\xe2\x94\x80"))
    RULE("bullet",  fg("#88C0D0").glyph("\xe2\x96\xb8 ").pad_left(3))  /* ▸ */
    RULE("ol",      fg("#88C0D0").pad_left(3).glyph(". "))              /* N. */

    /* ── Callout (fallback) ───────────────────────────────────── */
    RULE("callout", fg("#D8DEE9").bg("#2E3440").border("#4C566A").has_bg(true))

    /* ── Named callouts (Nord palette) ────────────────────────── */
    CALLOUT_RULE("tip",       fg("#A3BE8C").bg("#0E1A0E").border("#A3BE8C").glyph("\xe2\x9c\x94 ").has_bg(true))
    CALLOUT_RULE("note",      fg("#5E81AC").bg("#0C1220").border("#5E81AC").glyph("\xe2\x84\xb9  ").has_bg(true))
    CALLOUT_RULE("warning",   fg("#EBCB8B").bg("#1C1A0C").border("#EBCB8B").glyph("\xe2\x9a\xa0  ").has_bg(true))
    CALLOUT_RULE("danger",    fg("#BF616A").bg("#1E0C0E").border("#BF616A").glyph("\xe2\x98\xa0  ").has_bg(true))
    CALLOUT_RULE("important", fg("#B48EAD").bg("#1A0E1C").border("#B48EAD").glyph("\xe2\x97\x86 ").has_bg(true))
    CALLOUT_RULE("success",   fg("#A3BE8C").bg("#0C1A0A").border("#A3BE8C").glyph("\xe2\x9c\x94 ").has_bg(true))
    CALLOUT_RULE("error",     fg("#BF616A").bg("#200A0C").border("#BF616A").glyph("\xe2\x9c\x97 ").has_bg(true))
    CALLOUT_RULE("info",      fg("#88C0D0").bg("#0A161E").border("#88C0D0").glyph("\xe2\x84\xb9  ").has_bg(true))

    /* ── Inline styles ────────────────────────────────────────── */
    RULE("bold",      fg("#ECEFF4").font(libcpp::FONT_BOLD).pad_left(2))
    RULE("italic",    fg("#B48EAD").font(libcpp::FONT_ITALIC).pad_left(2))
    RULE("underline", fg("#88C0D0").font(libcpp::FONT_UNDERLINE).pad_left(2))
    RULE("strike",    fg("#4C566A").font(libcpp::FONT_STRIKE).pad_left(2))
    RULE("dim",       fg("#616E88").font(libcpp::FONT_DIM).pad_left(2))
    RULE("code",      fg("#A3BE8C").bg("#3B4252").font(libcpp::FONT_NONE).pad_left(2))

    /* ── Table ────────────────────────────────────────────────── */
    TABLE_RULE(
        border_preset(libcpp::BORDER_LIGHT)
        .border_color("#4C566A")
        .header_fg("#88C0D0").header_bg("#2E3440").header_font(libcpp::FONT_BOLD)
        .cell_fg("#D8DEE9").cell_bg("#2E3440").cell_alt_bg("#3B4252")
        .zebra(true).cell_has_bg(true).header_has_bg(true)
        .title_fg("#88C0D0").title_bg("#2E3440").title_has_bg(true)
        .footer_fg("#616E88")
    )

    /* ── Section ──────────────────────────────────────────────── */
    RULE("section", fg("#81A1C1").border("#4C566A").bold()
        .glyph("\xe2\x96\xb8 ").sep("\xe2\x94\x81"))

    WIDTH(66)

LIBCPP_THEME_END

#endif /* THEME_NORD_HPP */
