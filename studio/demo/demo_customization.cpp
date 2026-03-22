/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   demo_customization.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 15:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/22 15:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** demo_customization.cpp — Full Style Personalization Showcase
**
** Demonstrates that EVERY element is customizable:
**   - Headings(H1/H2/H3) — font, color, glyph, separator, alignment
**   - Body text — font, fg, padding
**   - Bold / Italic / Dim / Underline / Strike — fg + font override
**   - Bullet lists — glyph, fg, padding
**   - Ordered lists — fg, padding, number style
**   - Blockquotes — glyph, fg, margin, font
**   - Log levels — glyph, fg, font per level
**   - Callouts — fg, bg, border, glyph, has_bg(per callout type!)
**   - Separators — border color, glyph
**   - Sections — inherits from h2 style
**   - Tables — see demo_tables.cpp for exhaustive table showcase
*/

#include "libcpp/term/stylesheet.hpp"
#include "libcpp/term/writer.hpp"

using namespace libcpp;

int main() {
  /* ── Build a fully custom theme from scratch ───────────────────── */
  TermStyle ts;

  StyleSheet ss;

  /* Headings: ocean blues with wave glyphs */
  ss["h1"]
      .fg("#7FDBFF")
      .bg("#0D1B2A")
      .border("#7FDBFF")
      .bold()
      .center()
      .pad(3, 3, 1)
      .spacing(1, 1)
      .glyph("\xe2\x97\x86 ")
      .sep("\xe2\x95\x90");

  ss["h2"]
      .fg("#39CCCC")
      .border("#1B2838")
      .bold()
      .pad(1, 1)
      .spacing(1, 0)
      .glyph("\xe2\x96\xb8 ")
      .sep("\xe2\x94\x81");

  ss["h3"]
      .fg("#01FF70")
      .border("#1B2838")
      .bold()
      .pad(1, 1)
      .spacing(1, 0)
      .glyph("\xe2\x97\x8f ")
      .sep("\xe2\x95\x8c");

  /* Body& inline */
  ss["body"].fg("#E8E8E8").pad_left(2);
  ss["quote"].fg("#556677").italic().glyph("\xe2\x94\x82 ").margin_left(3);

  /* Log levels: fully custom glyph + color per level */
  ss["info"].fg("#7FDBFF").glyph("\xe2\x84\xb9  ");
  ss["warn"].fg("#FFDC00").bold().glyph("\xe2\x9a\xa0  ");
  ss["error"].fg("#FF4136").bold().glyph("\xe2\x9c\x97 ");
  ss["success"].fg("#2ECC40").glyph("\xe2\x9c\x94 ");
  ss["danger"].fg("#FF851B").bold().glyph("\xe2\x98\xa0  ");
  ss["trace"].fg("#B10DC9").dim().glyph("\xe2\x97\x87 ");

  /* Separator */
  ss["sep"].border("#1B2838").sep("\xe2\x94\x80");

  /* Lists: custom bullets */
  ss["bullet"].fg("#E8E8E8").glyph("\xe2\x97\x86 ").pad_left(4);
  ss["ol"].fg("#E8E8E8").pad_left(4);

  /* Callout defaults */
  ss["callout"].fg("#E8E8E8").bg("#0D1B2A").border("#39CCCC").has_bg(true);

  /* Table theme config — via StyleSheet TableRule */
  ss.table()
      .border_preset(BORDER_ROUNDED)
      .border_color("#39CCCC")
      .header_fg("#7FDBFF")
      .header_bg("#0D1B2A")
      .header_font(FONT_BOLD)
      .cell_fg("#E8E8E8")
      .cell_bg("#0D1B2A")
      .cell_alt_bg("#0F2232")
      .zebra(true)
      .cell_has_bg(true)
      .header_has_bg(true)
      .title_fg("#7FDBFF")
      .title_bg("#0D1B2A")
      .title_has_bg(true)
      .footer_fg("#39CCCC");

  ss.width(68);

  TermWriter w(ts);
  ss.apply(ts, w);

  /* ── Register CUSTOM callout types ────────────────────────────── */
  {
    /* ocean-blue tip */
    ElemStyle e;
    e.fg = Srgb(200, 230, 255);
    e.bg = Srgb(8, 20, 35);
    e.border = Srgb(80, 180, 220);
    e.glyph = "\xf0\x9f\x90\xac "; /* 🐬 */
    e.has_bg = true;
    e.font = FONT_NONE;
    e.width = 68;
    e.pad_l = 1;
    e.pad_r = 1;
    e.margin_l = 1;
    w.define_callout("ocean", e);
  }
  {
    /* fire warning */
    ElemStyle e;
    e.fg = Srgb(255, 220, 150);
    e.bg = Srgb(40, 20, 5);
    e.border = Srgb(255, 140, 0);
    e.glyph = "\xf0\x9f\x94\xa5 "; /* 🔥 */
    e.has_bg = true;
    e.font = FONT_NONE;
    e.width = 68;
    e.pad_l = 1;
    e.pad_r = 1;
    e.margin_l = 1;
    w.define_callout("fire", e);
  }
  {
    /* star bookmark */
    ElemStyle e;
    e.fg = Srgb(255, 255, 200);
    e.bg = Srgb(30, 28, 10);
    e.border = Srgb(255, 220, 50);
    e.glyph = "\xe2\x98\x85 "; /* ★ */
    e.has_bg = true;
    e.font = FONT_NONE;
    e.width = 68;
    e.pad_l = 1;
    e.pad_r = 1;
    e.margin_l = 1;
    w.define_callout("star", e);
  }
  {
    /* bug report */
    ElemStyle e;
    e.fg = Srgb(255, 180, 180);
    e.bg = Srgb(35, 10, 10);
    e.border = Srgb(255, 60, 60);
    e.glyph = "\xf0\x9f\x90\x9b "; /* 🐛 */
    e.has_bg = true;
    e.font = FONT_NONE;
    e.width = 68;
    e.pad_l = 1;
    e.pad_r = 1;
    e.margin_l = 1;
    w.define_callout("bug", e);
  }

  /* ── Showcase ─────────────────────────────────────────────────── */

  w.h1("Full Customization Demo");
  w.nl();
  w.text("This demo shows that EVERY element is personalizable.");
  w.text("Colors, glyphs, fonts, padding, alignment — all configurable.");
  w.nl();

  /* Headings */
  w.h2("Heading Styles");
  w.h3("Sub-section Example");
  w.text("H1 = centered banner with double-border box and bg color.");
  w.text("H2 = left accent bar with heavy underline.");
  w.text("H3 = colored bullet with dotted underline.");
  w.nl();

  /* Inline formatting */
  w.h2("Inline Formatting");
  w.bold("Bold text — FONT_BOLD override on body style");
  w.italic("Italic text — FONT_ITALIC override on body style");
  w.dim("Dim text — FONT_DIM override on body style");
  w.underline("Underlined text — FONT_UNDERLINE override");
  w.strike("Strikethrough text — FONT_STRIKE override");
  w.nl();

  /* Lists */
  w.h2("List Customization");
  w.text("Unordered list with custom diamond glyph:");
  w.bullet("First item with custom glyph and padding");
  w.bullet("Second item — notice the 4-space left pad");
  w.bullet("Third item — glyph is fully configurable");
  w.nl();
  w.text("Ordered list with custom padding:");
  w.ordered(1, "Enable the StyleSheet system");
  w.ordered(2, "Set bullet.glyph to any Unicode symbol");
  w.ordered(3, "Set ol.pad_left for ordered list indent");
  w.nl();

  /* Blockquote */
  w.h2("Blockquote Customization");
  w.quote("The border glyph is configurable — thin, thick, or custom.");
  w.quote("The margin_left controls the indent depth.");
  w.quote("Font is set to italic by the theme.");
  w.nl();

  /* Log levels */
  w.h2("Log Level Customization");
  w.text("Each log level has its own fg color, font, and glyph:");
  w.info("Custom info: aqua blue with info symbol");
  w.warn("Custom warning: golden yellow with warning triangle");
  w.error("Custom error: bright red with cross mark");
  w.success("Custom success: green with checkmark");
  w.danger("Custom danger: orange with skull");
  w.trace("Custom trace: purple with diamond(dim)");
  w.nl();

  /* Separators */
  w.h2("Separator Customization");
  w.text("Default separator(thin dash):");
  w.sep();
  w.text("Heavy rule(built-in hr() method):");
  w.hr();
  w.nl();

  /* Built-in callouts */
  w.h2("Built-in Callout Types");
  w.callout("tip", "Tip", "Use define_callout() to register custom callout",
            "types with their own colors and glyphs.");
  w.nl();
  w.callout("warning", "Warning",
            "The bg color now fills uniformly to the width",
            "of the widest line in the callout block.");
  w.nl();
  w.callout("danger", "Danger Zone", "Delete operations cannot be undone.",
            "Always verify before proceeding.");
  w.nl();

  /* CUSTOM callouts */
  w.h2("Custom Callout Types");
  w.text("You can register ANY callout type with define_callout():");
  w.nl();

  w.callout("ocean", "Ocean Tip", "This callout uses a dolphin emoji glyph",
            "with an ocean-blue color scheme.");
  w.nl();

  w.callout("fire", "Fire Warning", "This callout uses a fire emoji glyph",
            "with warm orange/amber colors.");
  w.nl();

  w.callout("star", "Starred Bookmark", "Custom callout with a star glyph",
            "and golden yellow border.");
  w.nl();

  w.callout("bug", "Bug Report #42",
            "Segfault in callout renderer when bg width",
            "was calculated per-line instead of uniformly.",
            "FIXED: now measures all lines first.");
  w.nl();

  /* Section */
  w.h2("Section Blocks");
  w.section("Architecture",
            "StyleSheet pattern: declare rules, apply to TermStyle");
  w.section("Performance",
            "Zero-cost: all config resolved at construction time");
  w.nl();

  /* How to customize */
  w.h2("How to Personalize Everything");
  {
    Table t(3);
    TableStyle s;
    s.border = BORDER_ROUNDED;
    s.border_color = Srgb(80, 180, 200);
    s.header_fg = Srgb(140, 230, 255);
    s.header_bg = Srgb(10, 25, 35);
    s.header_has_bg = true;
    s.header_font = FONT_BOLD;
    s.cell_fg = Srgb(200, 210, 220);
    s.zebra = true;
    s.cell_bg = Srgb(8, 15, 22);
    s.cell_alt_bg = Srgb(15, 22, 32);
    s.cell_has_bg = true;
    t.set_style(s);
    t.set_title("Customization Reference");
    t.header3("Element", "Selector", "Key Properties");
    t.row3("Heading 1", "h1",
           "fg, bg, border, font, align, glyph, sep, pad, spacing");
    t.row3("Heading 2", "h2", "fg, border, font, glyph, sep, pad, spacing");
    t.row3("Heading 3", "h3", "fg, border, font, glyph, sep, pad, spacing");
    t.row3("Body text", "body", "fg, font, pad_left");
    t.row3("Blockquote", "quote", "fg, font, glyph, margin_left");
    t.row3("Info log", "info", "fg, font, glyph");
    t.row3("Warning log", "warn", "fg, font, glyph");
    t.row3("Error log", "error", "fg, font, glyph");
    t.row3("Success log", "success", "fg, font, glyph");
    t.row3("Danger log", "danger", "fg, font, glyph");
    t.row3("Trace log", "trace", "fg, font, glyph");
    t.row3("Separator", "sep", "border, sep(glyph char)");
    t.row3("Bullet list", "bullet", "fg, glyph, pad_left");
    t.row3("Ordered list", "ol", "fg, pad_left");
    t.row3("Callout block", "callout", "fg, bg, border, glyph, has_bg");
    t.row3("Custom callout", "define_callout()",
           "fg, bg, border, glyph, font, width");
    t.set_footer("16 element types | All customizable via StyleSheet");
    w.table(t);
  }
  w.nl();

  w.sep();
  w.success("Customization demo complete — everything is personalizable!");

  return 0;
}
