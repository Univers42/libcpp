/* ************************************************************************** */
/*  theme_monokai.hpp -- Monokai Pro color theme configuration                */
/*                                                                            */
/*  Drop-in theme header. Include and call monokai_theme().apply(ts)          */
/*  to skin a TermStyle with Monokai's vibrant colors.                        */
/*                                                                            */
/*  Palette reference:  https://monokai.pro/                                  */
/*    Background  #272822    Gutter       #49483E                         */
/*    Foreground  #F8F8F2    Comment      #75715E                         */
/*    Red/Pink    #F92672    Orange       #FD971F                         */
/*    Yellow      #E6DB74    Green        #A6E22E                         */
/*    Blue        #66D9EF    Purple       #AE81FF                         */
/* ************************************************************************** */

#ifndef THEME_MONOKAI_HPP
# define THEME_MONOKAI_HPP

# include "libcpp/term/stylesheet.hpp"

LIBCPP_THEME_BEGIN(monokai_theme)

    /* ── Headings ─────────────────────────────────────────────── */
    RULE("h1",
        fg("#F92672").bg("#272822").border("#F92672")
        .bold().center()
        .pad(3, 3, 1).spacing(1, 1)
        .glyph("\xe2\x97\x86 ")       /* ◆ */
        .sep("\xe2\x95\x90")           /* ═ */
    )
    RULE("h2",
        fg("#66D9EF").border("#49483E")
        .bold()
        .pad(1, 1).spacing(1, 0)
        .glyph("\xe2\x9e\x9c ")       /* ➜ */
        .sep("\xe2\x94\x81")           /* ━ */
    )
    RULE("h3",
        fg("#A6E22E").border("#49483E")
        .bold()
        .pad(1, 1).spacing(1, 0)
        .glyph("\xe2\x98\x85 ")       /* ★ */
        .sep("\xe2\x95\x8c")           /* ╌ */
    )

    /* ── Body & text ──────────────────────────────────────────── */
    RULE("body",  fg("#F8F8F2").pad_left(2))
    RULE("quote", fg("#75715E").italic().glyph("\xe2\x94\x82 ").margin_left(3))

    /* ── Log levels ───────────────────────────────────────────── */
    RULE("info",    fg("#66D9EF").glyph("\xe2\x84\xb9  "))
    RULE("warn",    fg("#E6DB74").bold().glyph("\xe2\x9a\xa0  "))
    RULE("error",   fg("#F92672").bold().glyph("\xe2\x9c\x97 "))
    RULE("success", fg("#A6E22E").glyph("\xe2\x9c\x94 "))
    RULE("danger",  fg("#F92672").bold().glyph("\xe2\x98\xa0  "))
    RULE("trace",   fg("#AE81FF").dim().glyph("\xe2\x97\x87 "))

    /* ── Separators & lists ───────────────────────────────────── */
    RULE("sep",     border("#49483E").sep("\xe2\x94\x80"))
    RULE("bullet",  fg("#FD971F").glyph("\xe2\x98\x85 ").pad_left(3))  /* ★ */
    RULE("ol",      fg("#FD971F").pad_left(3).glyph("- "))              /* N- */

    /* ── Callout (fallback) ───────────────────────────────────── */
    RULE("callout", fg("#F8F8F2").bg("#272822").border("#49483E").has_bg(true))

    /* ── Named callouts (Monokai palette) ─────────────────────── */
    CALLOUT_RULE("tip",       fg("#A6E22E").bg("#0E1A06").border("#A6E22E").glyph("\xe2\x9c\x94 ").has_bg(true))
    CALLOUT_RULE("note",      fg("#66D9EF").bg("#061A22").border("#66D9EF").glyph("\xe2\x84\xb9  ").has_bg(true))
    CALLOUT_RULE("warning",   fg("#E6DB74").bg("#1C1A06").border("#E6DB74").glyph("\xe2\x9a\xa0  ").has_bg(true))
    CALLOUT_RULE("danger",    fg("#F92672").bg("#1E0610").border("#F92672").glyph("\xe2\x98\xa0  ").has_bg(true))
    CALLOUT_RULE("important", fg("#AE81FF").bg("#14061E").border("#AE81FF").glyph("\xe2\x97\x86 ").has_bg(true))
    CALLOUT_RULE("success",   fg("#A6E22E").bg("#0A1A04").border("#A6E22E").glyph("\xe2\x9c\x94 ").has_bg(true))
    CALLOUT_RULE("error",     fg("#F92672").bg("#200408").border("#F92672").glyph("\xe2\x9c\x97 ").has_bg(true))
    CALLOUT_RULE("info",      fg("#66D9EF").bg("#04161E").border("#66D9EF").glyph("\xe2\x84\xb9  ").has_bg(true))

    /* ── Inline styles ────────────────────────────────────────── */
    RULE("bold",      fg("#F8F8F2").font(libcpp::FONT_BOLD).pad_left(2))
    RULE("italic",    fg("#E6DB74").font(libcpp::FONT_ITALIC).pad_left(2))
    RULE("underline", fg("#66D9EF").font(libcpp::FONT_UNDERLINE).pad_left(2))
    RULE("strike",    fg("#75715E").font(libcpp::FONT_STRIKE).pad_left(2))
    RULE("dim",       fg("#75715E").font(libcpp::FONT_DIM).pad_left(2))
    RULE("code",      fg("#A6E22E").bg("#2E2F26").font(libcpp::FONT_NONE).pad_left(2))

    /* ── Table ────────────────────────────────────────────────── */
    TABLE_RULE(
        border_preset(libcpp::BORDER_HEAVY)
        .border_color("#49483E")
        .header_fg("#F92672").header_bg("#272822").header_font(libcpp::FONT_BOLD)
        .cell_fg("#F8F8F2").cell_bg("#272822").cell_alt_bg("#2E2F26")
        .zebra(true).cell_has_bg(true).header_has_bg(true)
        .title_fg("#F92672").title_bg("#272822").title_has_bg(true)
        .footer_fg("#75715E")
    )

    /* ── Section ──────────────────────────────────────────────── */
    RULE("section", fg("#66D9EF").border("#49483E").bold()
        .glyph("\xe2\x96\xb8 ").sep("\xe2\x94\x81"))

    WIDTH(66)

LIBCPP_THEME_END

#endif /* THEME_MONOKAI_HPP */
