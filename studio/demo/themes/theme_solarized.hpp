/* ************************************************************************** */
/*  theme_solarized.hpp -- Solarized Dark color theme configuration           */
/*                                                                            */
/*  Drop-in theme header. Include and call solarized_theme().apply(ts)        */
/*  to skin a TermStyle with Ethan Schoonover's Solarized Dark palette.       */
/*                                                                            */
/*  Palette reference:  https://ethanschoonover.com/solarized/                */
/*    Base03  #002B36   Base02  #073642   Base01  #586E75                     */
/*    Base00  #657B83   Base0   #839496   Base1   #93A1A1                     */
/*    Yellow  #B58900   Orange  #CB4B16   Red     #DC322F                     */
/*    Magenta #D33682   Violet  #6C71C4   Blue    #268BD2                     */
/*    Cyan    #2AA198   Green   #859900                                       */
/* ************************************************************************** */

#ifndef THEME_SOLARIZED_HPP
#define THEME_SOLARIZED_HPP

#include "libcpp/term/stylesheet.hpp"

LIBCPP_THEME_BEGIN(solarized_theme)

/* ── Headings ─────────────────────────────────────────────── */
RULE("h1", fg("#FDF6E3")
               .bg("#002B36")
               .border("#CB4B16")
               .bold()
               .center()
               .pad(3, 3, 1)
               .spacing(1, 1)
               .glyph("\xe2\x9e\x9c ") /* ➜ */
               .sep("\xe2\x95\x90")    /* ═ */
)
RULE("h2", fg("#268BD2")
               .border("#073642")
               .bold()
               .pad(1, 1)
               .spacing(1, 0)
               .glyph("\xe2\x97\x87 ") /* ◇ */
               .sep("\xe2\x94\x81")    /* ━ */
)
RULE("h3", fg("#859900")
               .border("#073642")
               .bold()
               .italic()
               .pad(1, 1)
               .spacing(1, 0)
               .glyph("\xc2\xb7 ")  /* · */
               .sep("\xe2\x95\x8c") /* ╌ */
)

/* ── Body& text ──────────────────────────────────────────── */
RULE("body", fg("#839496").pad_left(2))
RULE("quote", fg("#586E75").italic().glyph("\xe2\x94\x82 ").margin_left(3))

/* ── Log levels ───────────────────────────────────────────── */
RULE("info", fg("#2AA198").glyph("\xe2\x84\xb9  "))
RULE("warn", fg("#B58900").bold().glyph("\xe2\x9a\xa0  "))
RULE("error", fg("#DC322F").bold().glyph("\xe2\x9c\x97 "))
RULE("success", fg("#859900").glyph("\xe2\x9c\x94 "))
RULE("danger", fg("#DC322F").bold().glyph("\xe2\x98\xa0  "))
RULE("trace", fg("#6C71C4").dim().glyph("\xe2\x97\x87 "))

/* ── Separators& lists ───────────────────────────────────── */
RULE("sep", border("#073642").sep("\xe2\x94\x80"))
RULE("bullet", fg("#2AA198").glyph("\xe2\x97\x8b ").pad_left(3))  /* ○ */
RULE("ol", fg("#2AA198").pad_left(3).body_glyph("(").glyph(") ")) /* (N) */

/* ── Callout(fallback) ───────────────────────────────────── */
RULE("callout", fg("#839496").bg("#002B36").border("#073642").has_bg(true))

/* ── Named callouts(Solarized palette) ───────────────────── */
CALLOUT_RULE("tip", fg("#859900")
                        .bg("#001A00")
                        .border("#859900")
                        .glyph("\xe2\x9c\x94 ")
                        .has_bg(true))
CALLOUT_RULE("note", fg("#268BD2")
                         .bg("#001828")
                         .border("#268BD2")
                         .glyph("\xe2\x84\xb9  ")
                         .has_bg(true))
CALLOUT_RULE("warning", fg("#B58900")
                            .bg("#1A1400")
                            .border("#B58900")
                            .glyph("\xe2\x9a\xa0  ")
                            .has_bg(true))
CALLOUT_RULE("danger", fg("#DC322F")
                           .bg("#1A0404")
                           .border("#DC322F")
                           .glyph("\xe2\x98\xa0  ")
                           .has_bg(true))
CALLOUT_RULE("important", fg("#D33682")
                              .bg("#1A041A")
                              .border("#D33682")
                              .glyph("\xe2\x97\x86 ")
                              .has_bg(true))
CALLOUT_RULE("success", fg("#859900")
                            .bg("#0A1800")
                            .border("#859900")
                            .glyph("\xe2\x9c\x94 ")
                            .has_bg(true))
CALLOUT_RULE("error", fg("#DC322F")
                          .bg("#200204")
                          .border("#DC322F")
                          .glyph("\xe2\x9c\x97 ")
                          .has_bg(true))
CALLOUT_RULE("info", fg("#2AA198")
                         .bg("#001A1A")
                         .border("#2AA198")
                         .glyph("\xe2\x84\xb9  ")
                         .has_bg(true))

/* ── Inline styles ────────────────────────────────────────── */
RULE("bold", fg("#FDF6E3").font(libcpp::FONT_BOLD).pad_left(2))
RULE("italic", fg("#6C71C4").font(libcpp::FONT_ITALIC).pad_left(2))
RULE("underline", fg("#2AA198").font(libcpp::FONT_UNDERLINE).pad_left(2))
RULE("strike", fg("#586E75").font(libcpp::FONT_STRIKE).pad_left(2))
RULE("dim", fg("#586E75").font(libcpp::FONT_DIM).pad_left(2))
RULE("code", fg("#B58900").bg("#073642").font(libcpp::FONT_NONE).pad_left(2))

/* ── Table ────────────────────────────────────────────────── */
TABLE_RULE(border_preset(libcpp::BORDER_DOUBLE)
               .border_color("#073642")
               .header_fg("#CB4B16")
               .header_bg("#002B36")
               .header_font(libcpp::FONT_BOLD)
               .cell_fg("#839496")
               .cell_bg("#002B36")
               .cell_alt_bg("#073642")
               .zebra(true)
               .cell_has_bg(true)
               .header_has_bg(true)
               .title_fg("#CB4B16")
               .title_bg("#002B36")
               .title_has_bg(true)
               .footer_fg("#586E75"))

/* ── Section ──────────────────────────────────────────────── */
RULE("section", fg("#268BD2")
                    .border("#073642")
                    .bold()
                    .glyph("\xe2\x96\xb8 ")
                    .sep("\xe2\x94\x81"))

WIDTH(66)

LIBCPP_THEME_END

#endif /* THEME_SOLARIZED_HPP */
