/* ************************************************************************** */
/*  demo_nord.cpp -- Exhaustive TermWriter demo using Nord theme              */
/*                                                                            */
/*  This demo loads the Nord arctic color theme from themes/theme_nord.hpp    */
/*  and exercises EVERY rendering feature with content themed around           */
/*  software architecture and system design.                                  */
/*                                                                            */
/*  Config: themes/theme_nord.hpp                                             */
/*  Build:  make compile_studio                                               */
/*  Run:    ./build/bin/studio/demo/demo_nord                                 */
/* ************************************************************************** */

#include "libcpp/term/writer.hpp"
#include "libcpp/term/stylesheet.hpp"
#include "libcpp/term/table.hpp"
#include "libcpp/term/color.hpp"
#include "themes/theme_nord.hpp"
#include <iostream>

using libcpp::TermStyle;
using libcpp::TermWriter;
using libcpp::TermUtils;
using libcpp::Srgb;
using libcpp::Table;
using libcpp::Gradient;
using libcpp::Palette;
using libcpp::ElemStyle;

int main()
{
    /* ── Load the Nord theme ─────────────────────────────────── */
    TermStyle ts;
    TermWriter w(ts);
    nord_theme().apply(ts, w);

    std::cout << "\n";

    // ════════════════════════════════════════════════════════════
    //  1. HEADINGS
    // ════════════════════════════════════════════════════════════
    w << "# Nord Theme -- System Architecture";
    w << "A clean, arctic-inspired palette for technical documentation.";
    w << "Calm blues and greens reduce eye strain during long sessions.";

    w << "## Module Overview";
    w << "### Core Subsystems";
    w << "The library is organized into 9 independent modules,";
    w << "each with a single responsibility and clean interfaces.";

    // ════════════════════════════════════════════════════════════
    //  2. CALLOUT BLOCKS
    // ════════════════════════════════════════════════════════════
    w << "## Design Principles";

    w << ">![tip] Arctic Clarity";
    w << "> The Nord palette uses only 16 carefully balanced";
    w << "> colors. This constraint forces visual consistency.";
    w << "";

    w << ">![note] Frost Colors";
    w << "> The four Frost shades (#8FBCBB, #88C0D0, #81A1C1,";
    w << "> #5E81AC) form a cool-to-warm gradient for headings.";
    w << "";

    w << ">![warning] Snow Storm Contrast";
    w << "> Snow Storm whites on Polar Night backgrounds give";
    w << "> 7:1 contrast ratio -- above WCAG AAA requirements.";
    w << "";

    w << ">![danger] Aurora Red";
    w << "> BF616A is reserved for destructive actions and";
    w << "> critical errors. Use it sparingly for maximum impact.";
    w << "";

    w << ">![important] Polar Night";
    w << "> The four base dark colors (#2E3440 to #4C566A)";
    w << "> provide subtle depth without harsh boundaries.";
    w << "";

    w << ">![success] Integration Complete";
    w << "> All 9 modules integrate seamlessly. Zero circular";
    w << "> dependencies. Each module compiles independently.";
    w << "";

    w << ">![error] Dependency Conflict";
    w << "> Module 'net' requires pthread. Link with -lpthread";
    w << "> or use CXXFLAGS=-pthread during compilation.";
    w << "";

    w << ">![info] Color Science";
    w << "> Nord colors are designed in the CIELAB color space";
    w << "> for perceptual uniformity across different displays.";
    w << "";

    // ════════════════════════════════════════════════════════════
    //  3. LOG LEVELS
    // ════════════════════════════════════════════════════════════
    w << "## Build Output";
    w << "!i [build] Compiling 39 translation units...";
    w << "!w [build] Unused variable 'temp' in profiler.cpp:42";
    w << "!x [build] Implicit conversion loses precision (uint64 -> int)";
    w << "!v [build] Archive created: libcpp.a (39 objects, 847KB)";
    w << "!d [build] Stack buffer overflow in format.cpp:128";
    w << "!t [build] Total build time: 2.34s (parallel: 4 jobs)";

    // ════════════════════════════════════════════════════════════
    //  4. INLINE FORMATTING
    // ════════════════════════════════════════════════════════════
    w << "## Text Formatting";
    w << "**Bold emphasizes critical architectural decisions**";
    w << "*Italic marks implementation notes and alternatives*";
    w << "~~Strikethrough removes deprecated module names~~";
    w << "~Dim text for commit hashes and internal identifiers~";
    w << "__Underlined text highlights key interfaces__";
    w << "`std::string TermStyle::code(const std::string& msg) const`";

    // ════════════════════════════════════════════════════════════
    //  5. LISTS
    // ════════════════════════════════════════════════════════════
    w << "## Module Inventory";
    w << "### By Category";
    w << "- async: Workers, thread pool, persistent worker";
    w << "- core: Arena, memento, observer, singleton, signals";
    w << "- data: CSV, database, date, data buffer";
    w << "- log: Logger with file/console backends";
    w << "- math: Vectors, Perlin noise, RNG";
    w << "- mem: Leak guard, memory tracking";
    w << "- net: TCP client/server, message protocol";
    w << "- str: Case conversion, format, UTF-8";
    w << "- term: Colors, styles, tables, trees, progress, writer";
    w << "- test: Suite, fuzzer, snapshot, spy";
    w << "- util: Argparser, chronometer, config, timer";
    w << "";
    w << "### Build Order";
    w << "1. Compile all src/**/*.cpp to object files";
    w << "2. Archive into libcpp.a with ar rcs";
    w << "3. Create alias libftpp.a (cp libcpp.a libftpp.a)";
    w << "4. Link studio demos against the archive";
    w << "5. Run test_runner to validate all suites";

    // ════════════════════════════════════════════════════════════
    //  6. BLOCKQUOTES
    // ════════════════════════════════════════════════════════════
    w << "## Philosophy";
    w << "> Make each program do one thing well.";
    w << "> To do a new job, build afresh rather than";
    w << "> complicate old programs by adding new features.";
    w << "> -- Doug McIlroy, Unix Philosophy";

    // ════════════════════════════════════════════════════════════
    //  7. SECTIONS
    // ════════════════════════════════════════════════════════════
    w << "## Sections";
    w << "$$ Compilation / c++ -std=c++17 -Wall -Wextra -Werror -Iinclude -pthread";
    w << "$$ Linking / c++ main.cpp -L. -lcpp -o my_program";
    w << "$$ Testing / make compile_studio && ./build/bin/studio/tests/test_runner";

    // ════════════════════════════════════════════════════════════
    //  8. MARKDOWN TABLE
    // ════════════════════════════════════════════════════════════
    w << "## Architecture Table";
    w << "| Module | Files | Objects | Lines | Dependencies |";
    w << "| :--- | :---: | :---: | ---: | :--- |";
    w << "| async | 3 | 3 | 420 | core, util |";
    w << "| core | 10 | 8 | 1200 | (none) |";
    w << "| data | 4 | 4 | 680 | str, util |";
    w << "| log | 1 | 1 | 180 | term |";
    w << "| math | 4 | 4 | 350 | (none) |";
    w << "| mem | 1 | 1 | 120 | (none) |";
    w << "| net | 3 | 3 | 510 | async |";
    w << "| str | 3 | 3 | 290 | (none) |";
    w << "| term | 7 | 8 | 2400 | str |";
    w << "| test | 4 | 4 | 480 | str, term |";
    w << "| util | 4 | 4 | 360 | (none) |";
    w << "";

    // ════════════════════════════════════════════════════════════
    //  9. IMPERATIVE TABLE
    // ════════════════════════════════════════════════════════════
    w << "### Nord Palette Reference";
    {
        Table t(4);
        t.set_title("Nord Color Palette");
        std::string h[4] = {"Name", "Hex", "Group", "Usage"};
        t.header(h, 4);
        std::string rows[][4] = {
            {"Polar Night 1", "#2E3440", "Base", "Main background"},
            {"Polar Night 2", "#3B4252", "Base", "Elevated surfaces"},
            {"Polar Night 3", "#434C5E", "Base", "Selection, hover"},
            {"Polar Night 4", "#4C566A", "Base", "Comments, borders"},
            {"Snow Storm 1",  "#D8DEE9", "Text", "Body text"},
            {"Snow Storm 2",  "#E5E9F0", "Text", "Bright text"},
            {"Snow Storm 3",  "#ECEFF4", "Text", "Headings, emphasis"},
            {"Frost 1",       "#8FBCBB", "Frost", "Standalone tokens"},
            {"Frost 2",       "#88C0D0", "Frost", "H1, functions"},
            {"Frost 3",       "#81A1C1", "Frost", "H2, keywords"},
            {"Frost 4",       "#5E81AC", "Frost", "Deep accent"},
            {"Aurora Red",    "#BF616A", "Aurora", "Errors, danger"},
            {"Aurora Orange", "#D08770", "Aurora", "Warnings"},
            {"Aurora Yellow", "#EBCB8B", "Aurora", "Caution, search"},
            {"Aurora Green",  "#A3BE8C", "Aurora", "Success, strings"},
            {"Aurora Purple", "#B48EAD", "Aurora", "Trace, numbers"}
        };
        for (int i = 0; i < 16; ++i)
            t.row(rows[i], 4);
        w.table(t);
    }

    // ════════════════════════════════════════════════════════════
    //  10. COLOR ENGINE
    // ════════════════════════════════════════════════════════════
    w << "## Color Engine";
    w << "### Nord Frost Gradient";
    {
        Gradient g(Srgb::from_hex("#8FBCBB"), Srgb::from_hex("#5E81AC"));
        g.add_stop(Srgb::from_hex("#88C0D0"));
        g.add_stop(Srgb::from_hex("#81A1C1"));
        std::cout << "  " << g.apply("Frost: calm, icy, focused -- the Nord experience") << "\n";

        Gradient g2(Srgb::from_hex("#BF616A"), Srgb::from_hex("#B48EAD"));
        g2.add_stop(Srgb::from_hex("#D08770"));
        g2.add_stop(Srgb::from_hex("#EBCB8B"));
        g2.add_stop(Srgb::from_hex("#A3BE8C"));
        std::cout << "  " << g2.apply("Aurora: red, orange, yellow, green, purple") << "\n";

        std::string bar;
        for (int i = 0; i < 60; ++i) bar += "\xe2\x96\x88";
        Gradient g3(Srgb::from_hex("#2E3440"), Srgb::from_hex("#ECEFF4"));
        std::cout << "  " << g3.apply(bar) << "\n";
    }
    std::cout << "\n";

    w << "### Palette Swatches";
    {
        struct Swatch { const char* name; const char* hex; };
        Swatch sw[] = {
            {"Frost1",  "#8FBCBB"}, {"Frost2",  "#88C0D0"},
            {"Frost3",  "#81A1C1"}, {"Frost4",  "#5E81AC"},
            {"Red",     "#BF616A"}, {"Orange",  "#D08770"},
            {"Yellow",  "#EBCB8B"}, {"Green",   "#A3BE8C"},
            {"Purple",  "#B48EAD"}, {"Snow",    "#ECEFF4"}
        };
        std::cout << "  ";
        for (int i = 0; i < 10; ++i)
        {
            Srgb c = Srgb::from_hex(sw[i].hex);
            std::cout << c.to_ansi_fg() << "\xe2\x96\x88\xe2\x96\x88 "
                      << sw[i].name << TermUtils::reset();
            if (i < 9) std::cout << "  ";
        }
        std::cout << "\n\n";
    }

    // ════════════════════════════════════════════════════════════
    //  11. IMPERATIVE CHAINING
    // ════════════════════════════════════════════════════════════
    w << "## Imperative API";
    w.info("Nord's calm palette is ideal for log-heavy applications")
     .warn("Aurora colors are accent-only -- use them sparingly")
     .success("All modules compile independently with zero warnings")
     .error("Never mix Polar Night with Snow Storm on the same element")
     .code("nord_theme().apply(ts, w);")
     .bullet("16 colors total, designed for dark backgrounds")
     .bullet("CIELAB-balanced for perceptual uniformity")
     .bullet("WCAG AAA contrast on all text/background pairs")
     .ordered(1, "Include themes/theme_nord.hpp")
     .ordered(2, "Call nord_theme().apply(ts, w)")
     .ordered(3, "Create TermWriter w(ts) and stream")
     .nl();

    // ════════════════════════════════════════════════════════════
    //  FIN
    // ════════════════════════════════════════════════════════════
    w << "===";
    w << "## Theme: Nord -- Complete";
    w << "Configuration file: themes/theme_nord.hpp";
    w << "An arctic, clean palette for focused technical writing.";

    return 0;
}
