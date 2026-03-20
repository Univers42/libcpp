/* ************************************************************************** */
/*  demo_monokai.cpp -- Exhaustive TermWriter demo using Monokai theme        */
/*                                                                            */
/*  This demo loads the Monokai color theme from themes/theme_monokai.hpp     */
/*  and exercises EVERY rendering feature with content themed around           */
/*  a real-world C++ project build pipeline.                                  */
/*                                                                            */
/*  Config: themes/theme_monokai.hpp                                          */
/*  Build:  make compile_studio                                               */
/*  Run:    ./build/bin/studio/demo/demo_monokai                              */
/* ************************************************************************** */

#include "libcpp/term/writer.hpp"
#include "libcpp/term/stylesheet.hpp"
#include "libcpp/term/table.hpp"
#include "libcpp/term/color.hpp"
#include "themes/theme_monokai.hpp"
#include <iostream>

using libcpp::TermStyle;
using libcpp::TermWriter;
using libcpp::TermUtils;
using libcpp::Srgb;
using libcpp::Table;
using libcpp::Gradient;
using libcpp::Palette;

int main()
{
    /* ── Load the Monokai theme ──────────────────────────────── */
    TermStyle ts;
    TermWriter w(ts);
    monokai_theme().apply(ts, w);

    std::cout << "\n";

    // ════════════════════════════════════════════════════════════
    //  1. HEADINGS
    // ════════════════════════════════════════════════════════════
    w << "# Monokai -- Build Pipeline Report";
    w << "Warm, vibrant colors that pop on dark backgrounds.";
    w << "The classic code editor theme, now in your terminal.";

    w << "## Compilation Phase";
    w << "### Source File Processing";
    w << "Each .cpp file under src/ is compiled independently.";
    w << "Object files are placed in obj/ mirroring the source tree.";

    // ════════════════════════════════════════════════════════════
    //  2. CALLOUT BLOCKS
    // ════════════════════════════════════════════════════════════
    w << "## Build Diagnostics";

    w << ">![tip] Optimization Tip";
    w << "> Use -O2 for release builds. The compiler can";
    w << "> inline small functions and unroll tight loops.";
    w << "";

    w << ">![note] Compiler Note";
    w << "> We use c++ (the system compiler) rather than g++";
    w << "> or clang++ directly for maximum portability.";
    w << "";

    w << ">![warning] Deprecation Warning";
    w << "> std::auto_ptr was removed in C++17. Replace all";
    w << "> usages with std::unique_ptr before upgrading.";
    w << "";

    w << ">![danger] Segmentation Fault";
    w << "> Accessing _rows[MAX_ROWS] is out-of-bounds.";
    w << "> Always validate row_count() < MAX_ROWS before insert.";
    w << "";

    w << ">![important] ABI Compatibility";
    w << "> Changing the layout of ElemStyle breaks the ABI.";
    w << "> Recompile all dependent code after struct changes.";
    w << "";

    w << ">![success] Compilation Success";
    w << "> 39 translation units compiled in 2.1 seconds.";
    w << "> Zero warnings with -Wall -Wextra -Werror.";
    w << "";

    w << ">![error] Linker Error";
    w << "> Multiple definition of 'TermUtils::reset()'.";
    w << "> Ensure static methods are not defined in headers.";
    w << "";

    w << ">![info] Build System";
    w << "> GNU Make with pattern rules and automatic";
    w << "> dependency tracking. No CMake, no Meson.";
    w << "";

    // ════════════════════════════════════════════════════════════
    //  3. LOG LEVELS
    // ════════════════════════════════════════════════════════════
    w << "## Compiler Output";
    w << "!i [cc] Compiling stylesheet.cpp -> obj/term/stylesheet.o";
    w << "!i [cc] Compiling writer.cpp     -> obj/term/writer.o";
    w << "!w [cc] writer.cpp:42: unused parameter 'flags' [-Wunused-parameter]";
    w << "!x [ld] undefined reference to 'Gradient::apply(std::string const&)'";
    w << "!v [ar] libcpp.a archived (39 objects)";
    w << "!d [rt] ASAN: heap-buffer-overflow at table.cpp:205";
    w << "!t [gc] 4 temp objects collected after compilation phase";

    // ════════════════════════════════════════════════════════════
    //  4. INLINE FORMATTING
    // ════════════════════════════════════════════════════════════
    w << "## Code Style Guide";
    w << "**Always use explicit constructors for single-arg classes**";
    w << "*Consider marking utility functions as static or inline*";
    w << "~~Do not use #pragma once in 42 projects -- use include guards~~";
    w << "~Internal helpers should be prefixed with underscore: _emit()~";
    w << "__Mark all accessor methods with the const qualifier__";
    w << "`ElemStyle s = body_style; s.font = FONT_BOLD;`";

    // ════════════════════════════════════════════════════════════
    //  5. LISTS
    // ════════════════════════════════════════════════════════════
    w << "## Coding Standards";
    w << "### Required Practices";
    w << "- Every class follows Orthodox Canonical Form";
    w << "- No memory leaks (tested with LeakGuard + Valgrind)";
    w << "- All public methods documented with /** comments */";
    w << "- Max line length: 80 columns (soft), 120 (hard)";
    w << "- Tabs for indentation, spaces for alignment";
    w << "";
    w << "### Review Checklist";
    w << "1. Does the class have all 4 OCF methods?";
    w << "2. Are all resources freed in the destructor?";
    w << "3. Does operator= check for self-assignment?";
    w << "4. Are const methods marked const?";
    w << "5. Does it compile with -Wall -Wextra -Werror?";
    w << "6. Do all tests pass after the change?";

    // ════════════════════════════════════════════════════════════
    //  6. BLOCKQUOTES
    // ════════════════════════════════════════════════════════════
    w << "## Wisdom";
    w << "> Any fool can write code that a computer can understand.";
    w << "> Good programmers write code that humans can understand.";
    w << "> -- Martin Fowler";

    // ════════════════════════════════════════════════════════════
    //  7. SECTIONS
    // ════════════════════════════════════════════════════════════
    w << "## Pipeline Stages";
    w << "$$ Preprocessing / Expand macros, resolve includes, strip comments";
    w << "$$ Compilation / Parse C++17 AST, generate object code (.o files)";
    w << "$$ Archiving / Bundle objects into static archive (libcpp.a)";
    w << "$$ Linking / Resolve symbols, produce final executable";

    // ════════════════════════════════════════════════════════════
    //  8. MARKDOWN TABLE
    // ════════════════════════════════════════════════════════════
    w << "## Build Timing";
    w << "| Phase | Files | Time | Output |";
    w << "| :--- | :---: | ---: | :--- |";
    w << "| Compile | 39 .cpp | 1.84s | 39 .o files |";
    w << "| Archive | 39 .o | 0.12s | libcpp.a |";
    w << "| Alias | 1 .a | 0.01s | libftpp.a |";
    w << "| Demo | 4 .cpp | 0.32s | 4 binaries |";
    w << "| Tests | 8 .cpp | 0.18s | test_runner |";
    w << "| Total | 52 files | 2.47s | 6 outputs |";
    w << "";

    // ════════════════════════════════════════════════════════════
    //  9. IMPERATIVE TABLE
    // ════════════════════════════════════════════════════════════
    w << "### Monokai Color Assignments";
    {
        Table t(3);
        t.set_title("Monokai Pro Color Map");
        std::string h[3] = {"Color", "Hex", "Role in Theme"};
        t.header(h, 3);
        std::string rows[][3] = {
            {"Pink",       "#F92672", "H1 banner, keywords, critical errors"},
            {"Blue",       "#66D9EF", "H2 accent, info logs, types"},
            {"Green",      "#A6E22E", "H3 heading, success, strings"},
            {"Yellow",     "#E6DB74", "Warnings, search highlights"},
            {"Orange",     "#FD971F", "Modified, function parameters"},
            {"Purple",     "#AE81FF", "Trace, constants, numbers"},
            {"Comment",    "#75715E", "Quotes, dimmed secondary text"},
            {"Background", "#272822", "H1 bg, callout bg"},
            {"Gutter",     "#49483E", "Borders, separators"},
            {"Foreground", "#F8F8F2", "Body text, list items"}
        };
        for (int i = 0; i < 10; ++i)
            t.row(rows[i], 3);
        w.table(t);
    }

    // ════════════════════════════════════════════════════════════
    //  10. COLOR ENGINE
    // ════════════════════════════════════════════════════════════
    w << "## Color Engine";
    w << "### Monokai Gradient";
    {
        Gradient g(Srgb::from_hex("#F92672"), Srgb::from_hex("#AE81FF"));
        g.add_stop(Srgb::from_hex("#FD971F"));
        g.add_stop(Srgb::from_hex("#E6DB74"));
        g.add_stop(Srgb::from_hex("#A6E22E"));
        g.add_stop(Srgb::from_hex("#66D9EF"));
        std::cout << "  " << g.apply("Pink -> Orange -> Yellow -> Green -> Blue -> Purple") << "\n";

        std::string bar;
        for (int i = 0; i < 60; ++i) bar += "\xe2\x96\x88";
        std::cout << "  " << g.apply(bar) << "\n";
    }
    std::cout << "\n";

    w << "### Palette Swatches";
    {
        struct Swatch { const char* name; const char* hex; };
        Swatch sw[] = {
            {"Pink",   "#F92672"}, {"Orange", "#FD971F"},
            {"Yellow", "#E6DB74"}, {"Green",  "#A6E22E"},
            {"Blue",   "#66D9EF"}, {"Purple", "#AE81FF"},
            {"Gray",   "#75715E"}, {"White",  "#F8F8F2"}
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
    w << "## Summary";
    w.info("Monokai: warm and vibrant, ideal for build output")
     .warn("High-saturation colors can tire eyes after hours")
     .success("All 39 objects compile with zero warnings")
     .error("Test coverage: 92% (8 untested edge cases)")
     .code("make compile_studio && ./build/bin/studio/tests/test_runner")
     .bullet("Pink + Green = strongest visual contrast pair")
     .bullet("Blue carries informational weight")
     .bullet("Purple for secondary, analytical data")
     .ordered(1, "Include themes/theme_monokai.hpp")
     .ordered(2, "Call monokai_theme().apply(ts, w)")
     .ordered(3, "Stream markdown, see vibrant output")
     .nl();

    // ════════════════════════════════════════════════════════════
    //  FIN
    // ════════════════════════════════════════════════════════════
    w << "===";
    w << "## Theme: Monokai -- Complete";
    w << "Configuration file: themes/theme_monokai.hpp";
    w << "Warm, saturated colors for expressive build reports.";

    return 0;
}
