/* stylesheet_demo.cpp -- StyleSheet configuration showcase
 *
 *  Demonstrates the CSS-like fluent configuration system.
 *  Shows: custom rules, theme presets, macro themes.
 *
 *  Build:  make compile_studio
 *  Run:    ./build/bin/studio/demo/stylesheet_demo
 */

#include "libcpp/term/stylesheet.hpp"
#include "libcpp/term/writer.hpp"
#include <iostream>

using libcpp::TermStyle;
using libcpp::TermWriter;
using libcpp::StyleSheet;
using libcpp::Rule;
using libcpp::Srgb;

/* ── Macro-based theme definition ────────────────────────────────────── */

LIBCPP_THEME_BEGIN(cyberpunk)
    RULE("h1",      fg("#FF2E97").bg("#0D0221").border("#FF2E97").bold().center().pad(3, 3, 1).spacing(1, 1).glyph("\xe2\x97\x86 ").sep("\xe2\x95\x90"))
    RULE("h2",      fg("#00FFC8").border("#007F64").bold().pad(1, 1).spacing(1, 0).glyph("\xe2\x96\xb8 ").sep("\xe2\x94\x81"))
    RULE("h3",      fg("#FFD300").border("#806900").bold().pad(1, 1).spacing(1, 0).glyph("\xe2\x97\x8f ").sep("\xe2\x95\x8c"))
    RULE("body",    fg("#E0E0FF").pad_left(2))
    RULE("info",    fg("#00FFC8").glyph("\xe2\x84\xb9  "))
    RULE("warn",    fg("#FFD300").bold().glyph("\xe2\x9a\xa0  "))
    RULE("error",   fg("#FF2E97").bold().glyph("\xe2\x9c\x97 "))
    RULE("success", fg("#00FF66").glyph("\xe2\x9c\x94 "))
    RULE("quote",   fg("#8080B0").italic().glyph("\xe2\x94\x82 ").margin_left(3))
    RULE("sep",     border("#1A0442").sep("\xe2\x94\x80"))
    RULE("bullet",  fg("#E0E0FF").glyph("\xe2\x97\x8f ").pad_left(3))
    WIDTH(64)
LIBCPP_THEME_END

/* ── Helper to demo a theme ──────────────────────────────────────────── */

static void demo_theme(const std::string& name, TermStyle& ts)
{
    TermWriter w(ts);
    w << ("# " + name);
    w << ("## Second Level Heading");
    w << ("### Third Level Heading");
    w << "Regular body text with standard padding.";
    w << "!i Information message";
    w << "!w Warning: something needs attention";
    w << "!x Error: critical failure detected";
    w << "!v Success: all operations completed";
    w << "> Design is not just what it looks like.";
    w << "> Design is how it works. -- Steve Jobs";
    w << "- First bullet item";
    w << "- Second bullet item";
    w << "- Third bullet item";
    w << "---";
    w << "";
}

int main()
{
    std::cout << "\n";

    /* ── 1. Fluent API (custom inline config) ─────────────────── */
    {
        TermStyle ts;
        StyleSheet ss;

        ss["h1"]
            .fg("#FF6B9D").bg("#1A0A1F").border("#FF6B9D")
            .bold().center()
            .pad(3, 3, 1).spacing(1, 1)
            .glyph("\xe2\x97\x86 ").sep("\xe2\x95\x90");

        ss["h2"]
            .fg("#82CFFF").border("#4080C0")
            .bold()
            .pad(1, 1).spacing(1, 0)
            .glyph("\xe2\x96\xb8 ").sep("\xe2\x94\x81");

        ss["h3"]
            .fg("#82DCA0").border("#408850")
            .bold()
            .pad(1, 1).spacing(1, 0)
            .glyph("\xe2\x97\x8f ").sep("\xe2\x95\x8c");

        ss["body"].fg("#D0D0DA").pad_left(2);
        ss["info"].fg("#50C8FF").glyph("\xe2\x84\xb9  ");
        ss["warn"].fg("#FFB040").bold().glyph("\xe2\x9a\xa0  ");
        ss["error"].fg("#FF5060").bold().glyph("\xe2\x9c\x97 ");
        ss["success"].fg("#50E880").glyph("\xe2\x9c\x94 ");
        ss["quote"].fg("#9098A0").italic().glyph("\xe2\x94\x82 ").margin_left(3);
        ss["bullet"].fg("#D0D0DA").glyph("\xe2\x97\x8f ").pad_left(3);
        ss["sep"].border("#404050").sep("\xe2\x94\x80");

        ss.width(64);
        ss.apply(ts);

        demo_theme("Fluent API -- Custom Inline Config", ts);
    }

    /* ── 2. Built-in preset: Dracula ──────────────────────────── */
    {
        TermStyle ts;
        StyleSheet::dracula().apply(ts);
        demo_theme("Built-in Preset -- Dracula", ts);
    }

    /* ── 3. Built-in preset: Nord ─────────────────────────────── */
    {
        TermStyle ts;
        StyleSheet::nord().apply(ts);
        demo_theme("Built-in Preset -- Nord", ts);
    }

    /* ── 4. Built-in preset: Monokai ──────────────────────────── */
    {
        TermStyle ts;
        StyleSheet::monokai().apply(ts);
        demo_theme("Built-in Preset -- Monokai", ts);
    }

    /* ── 5. Built-in preset: Solarized ────────────────────────── */
    {
        TermStyle ts;
        StyleSheet::solarized().apply(ts);
        demo_theme("Built-in Preset -- Solarized", ts);
    }

    /* ── 6. Macro-based theme: Cyberpunk ──────────────────────── */
    {
        TermStyle ts;
        cyberpunk().apply(ts);
        demo_theme("Macro Theme -- Cyberpunk", ts);
    }

    /* ── 7. Incremental override: extend a preset ─────────────── */
    {
        TermStyle ts;
        StyleSheet::dracula().apply(ts);

        StyleSheet override_ss;
        override_ss["h1"].fg("#FFD700").bg("#1A0A00").border("#FFD700");
        override_ss["h2"].fg("#FFD700").border("#B8860B");
        override_ss["info"].fg("#FFD700").glyph("\xe2\x98\x85 ");
        override_ss.apply(ts);

        demo_theme("Override -- Dracula + Gold Accents", ts);
    }

    /* ── Summary ──────────────────────────────────────────────── */
    {
        TermStyle ts;
        StyleSheet::dracula().apply(ts);
        TermWriter w(ts);
        w << "# StyleSheet Configuration";
        w << "Three ways to configure terminal styles:";
        w << "";
        w << "1. Fluent API: ss[\"h1\"].fg(\"#FF6B9D\").bold().center()";
        w << "2. Built-in presets: StyleSheet::dracula().apply(ts)";
        w << "3. Macro themes: LIBCPP_THEME_BEGIN / RULE / LIBCPP_THEME_END";
        w << "";
        w << "!v All methods are compile-time validated.";
        w << "!i No parser, no external files, no runtime overhead.";
        w << "!w Themes compose: apply a preset then override specific rules.";
        w << "===";
    }

    return 0;
}
