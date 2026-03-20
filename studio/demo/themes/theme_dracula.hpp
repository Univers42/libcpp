/* ************************************************************************** */
/*  theme_dracula.hpp -- Dracula color theme configuration                    */
/*                                                                            */
/*  Drop-in theme header. Include and call dracula_theme().apply(ts)          */
/*  to skin a TermStyle with Dracula colors.                                  */
/*                                                                            */
/*  Palette reference:  https://draculatheme.com/contribute                   */
/*    Background  #282A36    Current Line #44475A                             */
/*    Foreground  #F8F8F2    Comment      #6272A4                             */
/*    Cyan        #8BE9FD    Green        #50FA7B                             */
/*    Orange      #FFB86C    Pink         #FF79C6                             */
/*    Purple      #BD93F9    Red          #FF5555                             */
/*    Yellow      #F1FA8C                                                     */
/* ************************************************************************** */

#ifndef THEME_DRACULA_HPP
# define THEME_DRACULA_HPP

# include "libcpp/term/stylesheet.hpp"

/*
** Macro-based theme definition.
** Usage:
**   #include "themes/theme_dracula.hpp"
**   TermStyle ts;
**   dracula_theme().apply(ts);
*/

LIBCPP_THEME_BEGIN(dracula_theme)

    /* ── Headings ─────────────────────────────────────────────── */
    RULE("h1",
        fg("#FF79C6").bg("#282A36").border("#FF79C6")
        .bold().center()
        .pad(3, 3, 1).spacing(1, 1)
        .glyph("\xe2\x97\x86 ")       /* ◆ */
        .sep("\xe2\x95\x90")           /* ═ */
    )
    RULE("h2",
        fg("#BD93F9").border("#6272A4")
        .bold()
        .pad(1, 1).spacing(1, 0)
        .glyph("\xe2\x96\xb8 ")       /* ▸ */
        .sep("\xe2\x94\x81")           /* ━ */
    )
    RULE("h3",
        fg("#50FA7B").border("#44475A")
        .bold().italic()
        .pad(1, 1).spacing(1, 0)
        .glyph("\xe2\x97\x8f ")       /* ● */
        .sep("\xe2\x95\x8c")           /* ╌ */
    )

    /* ── Body & text ──────────────────────────────────────────── */
    RULE("body",  fg("#F8F8F2").pad_left(2))
    RULE("quote", fg("#6272A4").italic().glyph("\xe2\x94\x82 ").margin_left(3))

    /* ── Log levels ───────────────────────────────────────────── */
    RULE("info",    fg("#8BE9FD").glyph("\xe2\x84\xb9  "))       /* ℹ  */
    RULE("warn",    fg("#FFB86C").bold().glyph("\xe2\x9a\xa0  ")) /* ⚠  */
    RULE("error",   fg("#FF5555").bold().glyph("\xe2\x9c\x97 ")) /* ✗ */
    RULE("success", fg("#50FA7B").glyph("\xe2\x9c\x94 "))        /* ✔ */
    RULE("danger",  fg("#FF5555").bold().glyph("\xe2\x98\xa0  ")) /* ☠  */
    RULE("trace",   fg("#BD93F9").dim().glyph("\xe2\x97\x87 "))  /* ◇ */

    /* ── Separators & lists ───────────────────────────────────── */
    RULE("sep",     border("#44475A").sep("\xe2\x94\x80"))        /* ─ */
    RULE("bullet",  fg("#BD93F9").glyph("\xe2\x97\x86 ").pad_left(3))  /* ◆ */
    RULE("ol",      fg("#BD93F9").pad_left(3).glyph(") "))        /* N) */

    /* ── Callout (fallback) ───────────────────────────────────── */
    RULE("callout", fg("#F8F8F2").bg("#282A36").border("#6272A4").has_bg(true))

    /* ── Named callouts (Dracula palette) ─────────────────────── */
    CALLOUT_RULE("tip",       fg("#50FA7B").bg("#0C1E12").border("#50FA7B").glyph("\xe2\x9c\x94 ").has_bg(true))
    CALLOUT_RULE("note",      fg("#8BE9FD").bg("#0C1223").border("#8BE9FD").glyph("\xe2\x84\xb9  ").has_bg(true))
    CALLOUT_RULE("warning",   fg("#FFB86C").bg("#231C0A").border("#FFB86C").glyph("\xe2\x9a\xa0  ").has_bg(true))
    CALLOUT_RULE("danger",    fg("#FF5555").bg("#260C0C").border("#FF5555").glyph("\xe2\x98\xa0  ").has_bg(true))
    CALLOUT_RULE("important", fg("#FF79C6").bg("#1F0A1F").border("#FF79C6").glyph("\xe2\x97\x86 ").has_bg(true))
    CALLOUT_RULE("success",   fg("#50FA7B").bg("#0A200D").border("#50FA7B").glyph("\xe2\x9c\x94 ").has_bg(true))
    CALLOUT_RULE("error",     fg("#FF5555").bg("#280A0A").border("#FF5555").glyph("\xe2\x9c\x97 ").has_bg(true))
    CALLOUT_RULE("info",      fg("#8BE9FD").bg("#0A1923").border("#8BE9FD").glyph("\xe2\x84\xb9  ").has_bg(true))

    /* ── Inline styles ────────────────────────────────────────── */
    RULE("bold",      fg("#F8F8F2").font(libcpp::FONT_BOLD).pad_left(2))
    RULE("italic",    fg("#BD93F9").font(libcpp::FONT_ITALIC).pad_left(2))
    RULE("underline", fg("#8BE9FD").font(libcpp::FONT_UNDERLINE).pad_left(2))
    RULE("strike",    fg("#6272A4").font(libcpp::FONT_STRIKE).pad_left(2))
    RULE("dim",       fg("#6272A4").font(libcpp::FONT_DIM).pad_left(2))
    RULE("code",      fg("#F1FA8C").bg("#282A36").font(libcpp::FONT_NONE).pad_left(2))

    /* ── Table ────────────────────────────────────────────────── */
    TABLE_RULE(
        border_preset(libcpp::BORDER_ROUNDED)
        .border_color("#6272A4")
        .header_fg("#FF79C6").header_bg("#282A36").header_font(libcpp::FONT_BOLD)
        .cell_fg("#F8F8F2").cell_bg("#282A36").cell_alt_bg("#2D303D")
        .zebra(true).cell_has_bg(true).header_has_bg(true)
        .title_fg("#FF79C6").title_bg("#282A36").title_has_bg(true)
        .footer_fg("#6272A4")
    )

    /* ── Section ──────────────────────────────────────────────── */
    RULE("section", fg("#BD93F9").border("#6272A4").bold()
        .glyph("\xe2\x96\xb8 ").sep("\xe2\x94\x81"))

    WIDTH(66)

LIBCPP_THEME_END

#endif /* THEME_DRACULA_HPP */
