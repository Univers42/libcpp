/* ************************************************************************** */
/*  demo_dracula.cpp -- Exhaustive TermWriter demo using Dracula theme        */
/*                                                                            */
/*  This demo loads the Dracula color theme from themes/theme_dracula.hpp     */
/*  and exercises EVERY rendering feature: headings, callouts, tables,        */
/*  log levels, lists, sections, quotes, inline formatting, gradients.        */
/*                                                                            */
/*  Config: themes/theme_dracula.hpp                                          */
/*  Build:  make compile_studio                                               */
/*  Run:    ./build/bin/studio/demo/demo_dracula                              */
/* ************************************************************************** */

#include "libcpp/term/writer.hpp"
#include "libcpp/term/stylesheet.hpp"
#include "libcpp/term/table.hpp"
#include "libcpp/term/color.hpp"
#include "themes/theme_dracula.hpp"
#include <iostream>

using libcpp::TermStyle;
using libcpp::TermWriter;
using libcpp::TermUtils;
using libcpp::Srgb;
using libcpp::Table;
using libcpp::TableStyle;
using libcpp::Gradient;
using libcpp::Palette;
using libcpp::ElemStyle;

int main()
{
    /* ── Load the Dracula theme ──────────────────────────────── */
    TermStyle ts;
    TermWriter w(ts);
    dracula_theme().apply(ts, w);

    std::cout << "\n";

    // ════════════════════════════════════════════════════════════
    //  1. HEADINGS
    // ════════════════════════════════════════════════════════════
    w << "# Dracula Theme Showcase";
    w << "The iconic dark theme with vivid pink, purple, cyan, and green.";
    w << "Every element below is styled by **themes/theme_dracula.hpp**.";

    w << "## Typography & Heading Hierarchy";
    w << "### Primary, Secondary, Tertiary";
    w << "H1 renders as a full-width banner with background and double borders.";
    w << "H2 uses a vertical accent bar with a heavy underline.";
    w << "H3 shows a colored bullet with a dotted underline.";

    // ════════════════════════════════════════════════════════════
    //  2. ALL 8 CALLOUT BLOCKS
    // ════════════════════════════════════════════════════════════
    w << "## Callout Blocks";

    w << ">![tip] Dracula Tip";
    w << "> Use hex colors like #FF79C6 in the StyleSheet";
    w << "> to match the exact Dracula palette values.";
    w << "";

    w << ">![note] Design Note";
    w << "> The background tints are intentionally subtle";
    w << "> so the left border bar carries the visual weight.";
    w << "";

    w << ">![warning] Performance Warning";
    w << "> True-color (24-bit) ANSI requires a modern terminal.";
    w << "> Fallback to 256-color is not yet implemented.";
    w << "";

    w << ">![danger] Critical Issue";
    w << "> Calling delete on a stack-allocated object is UB.";
    w << "> Always check ownership semantics before freeing.";
    w << "";

    w << ">![important] Architecture Decision";
    w << "> StyleSheet uses a bitmask so only declared properties";
    w << "> override the defaults. Undeclared fields pass through.";
    w << "";

    w << ">![success] Build Status: Passing";
    w << "> 39 objects compiled with -Wall -Wextra -Werror.";
    w << "> 36 test cases pass across all 8 suites.";
    w << "";

    w << ">![error] Linking Error";
    w << "> undefined reference to 'libcpp::StyleSheet::dracula()'";
    w << "> Fix: ensure stylesheet.cpp is compiled into the archive.";
    w << "";

    w << ">![info] How Themes Work";
    w << "> 1. Create a .hpp with LIBCPP_THEME_BEGIN / RULE / LIBCPP_THEME_END";
    w << "> 2. Include it and call my_theme().apply(ts)";
    w << "> 3. Pass ts to TermWriter. Done.";
    w << "";

    // ════════════════════════════════════════════════════════════
    //  3. LOG LEVELS
    // ════════════════════════════════════════════════════════════
    w << "## Log Levels";
    w << "!i [dracula] Theme loaded: 16 rules, width=66";
    w << "!w [dracula] Color #FF79C6 may clip on 256-color terminals";
    w << "!x [dracula] StyleSheet overflow: MAX_RULES=32 exceeded";
    w << "!v [dracula] All rendering tests passed in 8.4ms";
    w << "!d [dracula] Double-free detected in TermWriter destructor";
    w << "!t [dracula] Callout registry: 8 presets loaded (0.02ms)";

    // ════════════════════════════════════════════════════════════
    //  4. INLINE FORMATTING
    // ════════════════════════════════════════════════════════════
    w << "## Inline Formatting";
    w << "**Bold text stands out for critical emphasis**";
    w << "*Italic text whispers a subtle secondary note*";
    w << "~~Strikethrough for deprecated or removed features~~";
    w << "~Dim text fades gently into the background~";
    w << "__Underlined text draws the eye with a baseline accent__";
    w << "`Inline code rendered with a background highlight`";

    // ════════════════════════════════════════════════════════════
    //  5. LISTS
    // ════════════════════════════════════════════════════════════
    w << "## Lists";
    w << "### Unordered";
    w << "- Orthodox Canonical Form on every class";
    w << "- Rule-of-three: constructor, copy, destructor";
    w << "- RAII for all resource management";
    w << "- No raw new/delete outside of constructors/destructors";
    w << "";
    w << "### Ordered";
    w << "1. Define the theme in a .hpp header file";
    w << "2. Use LIBCPP_THEME_BEGIN and RULE macros";
    w << "3. Include the header in your demo .cpp";
    w << "4. Call my_theme().apply(ts) before creating TermWriter";
    w << "5. Stream markdown with operator<< -- done";

    // ════════════════════════════════════════════════════════════
    //  6. BLOCKQUOTES
    // ════════════════════════════════════════════════════════════
    w << "## Blockquotes";
    w << "> Programs must be written for people to read,";
    w << "> and only incidentally for machines to execute.";
    w << "> -- Abelson & Sussman, SICP";

    // ════════════════════════════════════════════════════════════
    //  7. SECTIONS
    // ════════════════════════════════════════════════════════════
    w << "## Sections";
    w << "$$ Color Palette / Pink #FF79C6, Purple #BD93F9, Cyan #8BE9FD, Green #50FA7B";
    w << "$$ Design Goal / Vivid accent colors on a deep dark background (#282A36)";

    // ════════════════════════════════════════════════════════════
    //  8. MARKDOWN TABLE (pipe syntax)
    // ════════════════════════════════════════════════════════════
    w << "## Markdown Tables";
    w << "| Dracula Color | Hex | Usage |";
    w << "| :--- | :---: | ---: |";
    w << "| Pink | #FF79C6 | H1, keywords, functions |";
    w << "| Purple | #BD93F9 | H2, numbers, constants |";
    w << "| Cyan | #8BE9FD | Info, types, parameters |";
    w << "| Green | #50FA7B | H3, strings, success |";
    w << "| Orange | #FFB86C | Warnings, modifiers |";
    w << "| Red | #FF5555 | Errors, deletions |";
    w << "| Yellow | #F1FA8C | Searches, highlights |";
    w << "| Comment | #6272A4 | Borders, dimmed text |";
    w << "";

    // ════════════════════════════════════════════════════════════
    //  9. IMPERATIVE TABLE
    // ════════════════════════════════════════════════════════════
    w << "### Imperative Table";
    {
        Table t(4);
        t.set_title("StyleSheet Configuration Reference");
        std::string h[4] = {"Property", "Type", "Example", "Description"};
        t.header(h, 4);
        std::string r1[4] = {".fg()",      "hex/Srgb", "#FF79C6",   "Foreground color"};
        std::string r2[4] = {".bg()",      "hex/Srgb", "#282A36",   "Background color"};
        std::string r3[4] = {".border()",  "hex/Srgb", "#6272A4",   "Border/accent color"};
        std::string r4[4] = {".bold()",    "flag",     "--",        "Bold font weight"};
        std::string r5[4] = {".italic()",  "flag",     "--",        "Italic style"};
        std::string r6[4] = {".center()",  "align",    "--",        "Center alignment"};
        std::string r7[4] = {".glyph()",   "string",   "◆ ",       "Leading glyph"};
        std::string r8[4] = {".sep()",     "string",   "═",        "Separator character"};
        std::string r9[4] = {".pad()",     "int,int",  "3, 3",     "Left/right padding"};
        std::string r10[4] = {".width()",  "int",      "66",       "Total element width"};
        t.row(r1, 4); t.row(r2, 4); t.row(r3, 4); t.row(r4, 4); t.row(r5, 4);
        t.row(r6, 4); t.row(r7, 4); t.row(r8, 4); t.row(r9, 4); t.row(r10, 4);
        w.table(t);
    }

    // ════════════════════════════════════════════════════════════
    //  10. COLOR ENGINE — GRADIENTS
    // ════════════════════════════════════════════════════════════
    w << "## Color Engine";
    w << "### Dracula Gradient";
    {
        Gradient g(Srgb::from_hex("#FF79C6"), Srgb::from_hex("#BD93F9"));
        g.add_stop(Srgb::from_hex("#8BE9FD"));
        g.add_stop(Srgb::from_hex("#50FA7B"));
        std::cout << "  " << g.apply("The quick brown fox jumps over the lazy dog -- Dracula") << "\n";

        std::string bar;
        for (int i = 0; i < 60; ++i) bar += "\xe2\x96\x88";
        std::cout << "  " << g.apply(bar) << "\n";
    }
    std::cout << "\n";

    w << "### Palette Swatches";
    {
        struct Swatch { const char* name; const char* hex; };
        Swatch sw[] = {
            {"Pink",    "#FF79C6"}, {"Purple",  "#BD93F9"},
            {"Cyan",    "#8BE9FD"}, {"Green",   "#50FA7B"},
            {"Orange",  "#FFB86C"}, {"Red",     "#FF5555"},
            {"Yellow",  "#F1FA8C"}, {"Comment", "#6272A4"}
        };
        std::cout << "  ";
        for (int i = 0; i < 8; ++i)
        {
            Srgb c = Srgb::from_hex(sw[i].hex);
            std::cout << c.to_ansi_fg() << "\xe2\x96\x88\xe2\x96\x88 "
                      << sw[i].name << TermUtils::reset();
            if (i < 7) std::cout << "  ";
        }
        std::cout << "\n\n";
    }

    // ════════════════════════════════════════════════════════════
    //  11. IMPERATIVE CHAINING
    // ════════════════════════════════════════════════════════════
    w << "## Imperative Chaining";
    w.info("All TermWriter methods return *this for fluent chaining")
     .warn("Mix imperative calls with operator<< freely")
     .success("39 objects archived, 36 tests passing")
     .bullet("Headings: .h1() .h2() .h3()")
     .bullet("Logs: .info() .warn() .error() .success() .danger() .trace()")
     .bullet("Blocks: .quote() .callout() .section() .table()")
     .bullet("Formatting: .bold() .italic() .dim() .strike() .underline() .code()")
     .code("int main() { return 0; }")
     .ordered(1, "Include themes/theme_dracula.hpp")
     .ordered(2, "Call dracula_theme().apply(ts, w)")
     .ordered(3, "Create TermWriter w(ts)")
     .ordered(4, "Stream your markdown")
     .nl();

    // ════════════════════════════════════════════════════════════
    //  FIN
    // ════════════════════════════════════════════════════════════
    w << "===";
    w << "## Theme: Dracula -- Complete";
    w << "Configuration file: themes/theme_dracula.hpp";
    w << "Every rendering element exercised above.";

    return 0;
}
