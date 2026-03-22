/* writer_demo.cpp -- TermWriter showcase
 *
 *  Zero-config demo: just create a TermWriter and write
 *  markdown. All callout presets are built-in.
 *
 *  Build:  make compile_studio
 *  Run:    ./build/bin/studio/demo/writer_demo
 */

#include "libcpp/term/writer.hpp"
#include "libcpp/term/style.hpp"
#include "libcpp/term/table.hpp"
#include "libcpp/term/color.hpp"
#include <iostream>

using libcpp::TermStyle;
using libcpp::TermConf;
using libcpp::TermWriter;
using libcpp::TermUtils;
using libcpp::Srgb;
using libcpp::Table;
using libcpp::Palette;
using libcpp::Gradient;

int main()
{
    TermStyle ts;
    TermConf::apply_default(ts);
    TermWriter w(ts);

    // ===== 1. Headings & Structure =====
    w << "# libcpp -- Terminal Rendering Engine";
    w << "A zero-config markdown-to-terminal renderer.";
    w << "Write markdown, get beautiful styled output.";

    w << "## Heading Levels";
    w << "### Third-Level Heading";
    w << "Each heading level has its own color, glyph, and separator.";

    // ===== 2. Callout Blocks =====
    w << "## Callout Blocks";
    w << "Eight built-in callout presets, zero configuration needed.";

    w << ">![tip] Pro Tip";
    w << "> Use operator<< to stream markdown lines.";
    w << "> The writer parses and styles them instantly.";
    w << "";

    w << ">![note] A Note on Design";
    w << "> Every element is rendered through ElemStyle.";
    w << "> Colors, glyphs, padding -- all configurable.";
    w << "";

    w << ">![warning] Compiler Warning";
    w << "> The -Werror flag turns every warning into a";
    w << "> hard error. Always test with all flags on.";
    w << "";

    w << ">![danger] Memory Safety";
    w << "> Never free memory allocated by another module.";
    w << "> Use RAII or smart pointers instead.";
    w << "";

    w << ">![important] Key Concept";
    w << "> Orthodox Canonical Form: every class needs a";
    w << "> default ctor, copy ctor, operator=, and dtor.";
    w << "";

    w << ">![success] All Tests Passing";
    w << "> 36 tests across 8 suites completed in 12ms.";
    w << "> Zero failures, zero leaks.";
    w << "";

    w << ">![error] Compilation Failed";
    w << "> src/term/writer.cpp:503: no matching constructor.";
    w << "> Table(int, TableStyle) does not exist.";
    w << "";

    w << ">![info] How Callouts Work";
    w << "> Start with >![type] Title on one line.";
    w << "> Continue with > body on subsequent lines.";
    w << "> End with an empty line to close the block.";
    w << "";

    // ===== 3. Log Levels =====
    w << "## Log Levels";
    w << "!i Server started on port 8080";
    w << "!w Memory usage above 80% threshold";
    w << "!x Segmentation fault in module::init()";
    w << "!v Build completed successfully (0 errors)";
    w << "!d Critical: stack overflow detected";
    w << "!t [GC] Collected 1247 objects in 3.2ms";

    // ===== 4. Inline Formatting =====
    w << "## Inline Formatting";
    w << "**Bold text for emphasis**";
    w << "*Italic text for subtle notes*";
    w << "~~Strikethrough for deprecated items~~";
    w << "~Dim text for secondary information~";

    // ===== 5. Lists =====
    w << "## Lists";
    w << "- Bullet lists use the dash prefix";
    w << "- Each item gets a colored bullet glyph";
    w << "- Consistent padding and alignment";
    w << "";
    w << "1. Ordered lists use N. prefix";
    w << "2. Numbers are rendered inline";
    w << "3. Same padding system as bullets";

    // ===== 6. Blockquotes & Sections =====
    w << "## Blockquotes";
    w << "> Simplicity is the ultimate sophistication.";
    w << "> -- Leonardo da Vinci";

    w << "## Sections";
    w << "$$ Architecture / Modules: async, data, math, mem, net, str, term, test, util.";
    w << "$$ Philosophy / Do one thing well. Compose freely. No hidden magic.";

    // ===== 7. Tables =====
    w << "## Tables";
    w << "| Module | Objects | Description |";
    w << "| --- | --- | --- |";
    w << "| term | 9 | Styles, tables, trees, progress |";
    w << "| str | 3 | Case conversion, format, UTF-8 |";
    w << "| test | 4 | Fuzzer, snapshot, spy, suite |";
    w << "| data | 4 | CSV, database, date, buffer |";
    w << "| async | 3 | Workers, thread pool, persistent |";
    w << "";

    w.h3("Imperative Table API");
    {
        Table t(3);
        std::string h[3] = {"Syntax", "Meaning", "Example"};
        t.header(h, 3);
        std::string r1[3] = {"# Title",  "H1 heading",   "Full-width box"};
        std::string r2[3] = {"## Title", "H2 heading",   "Arrow + separator"};
        std::string r3[3] = {">![type]", "Callout open",  "Obsidian-style block"};
        std::string r4[3] = {"!i text",  "Info log",      "Blue info icon"};
        std::string r5[3] = {"- item",   "Bullet list",   "Colored bullet glyph"};
        std::string r6[3] = {"---",      "Separator",     "Thin horizontal rule"};
        t.row(r1, 3); t.row(r2, 3); t.row(r3, 3);
        t.row(r4, 3); t.row(r5, 3); t.row(r6, 3);
        w.table(t);
    }

    // ===== 8. Color Engine =====
    w << "## Color Engine";

    w.h3("Gradients");
    {
        Gradient g1(Srgb(0, 200, 255), Srgb(200, 0, 255));
        g1.add_stop(Srgb(255, 200, 0));
        std::cout << "  " << g1.apply("The quick brown fox jumps over the lazy dog") << "\n";

        Gradient g2(Srgb(255, 60, 100), Srgb(100, 60, 255));
        g2.add_stop(Srgb(60, 220, 255));
        std::cout << "  " << g2.apply("libcpp \xe2\x80\x94 a modern C++ utility library for 42") << "\n";

        Gradient g3(Palette::green(), Palette::cyan());
        g3.add_stop(Palette::blue());
        std::string bar;
        for (int i = 0; i < 56; ++i) bar += "\xe2\x96\x88";
        std::cout << "  " << g3.apply(bar) << "\n";
    }
    std::cout << "\n";

    w.h3("Color Manipulation");
    {
        Srgb base = Palette::red();
        std::cout << "  base   = " << base.to_ansi_fg() << base.to_hex()
                  << TermUtils::reset() << "\n";
        for (int i = 1; i <= 4; ++i)
        {
            double r = i * 0.2;
            Srgb li = base.lighten(r), da = base.darken(r);
            std::cout << "  +" << static_cast<int>(r * 100) << "%    = "
                      << li.to_ansi_fg() << li.to_hex() << TermUtils::reset()
                      << "   -" << static_cast<int>(r * 100) << "%    = "
                      << da.to_ansi_fg() << da.to_hex() << TermUtils::reset()
                      << "\n";
        }
    }
    std::cout << "\n";

    w.h3("Named Palette");
    {
        const char* names[] = {
            "red", "green", "blue", "yellow", "cyan",
            "magenta", "orange", "purple", "teal", "pink"
        };
        std::cout << "  ";
        for (int i = 0; i < 10; ++i)
        {
            Srgb c = Srgb::named(names[i]);
            std::cout << c.to_ansi_fg() << "\xe2\x96\x88\xe2\x96\x88 "
                      << names[i] << TermUtils::reset();
            if (i < 9) std::cout << "  ";
        }
        std::cout << "\n\n";
    }

    // ===== 9. Imperative Chaining =====
    w << "## Imperative Chaining API";
    w.info("All methods return *this for fluent chaining")
     .warn("This includes .info(), .warn(), .error(), .h1() ...")
     .success("Build: 38 objects archived")
     .bullet("Orthodox Canonical Form on every class")
     .bullet("No memory leaks (LeakGuard verified)")
     .bullet("C++98 compatible, C++17 optional features")
     .ordered(1, "Create a TermStyle")
     .ordered(2, "Apply a preset with TermConf")
     .ordered(3, "Construct a TermWriter")
     .ordered(4, "Stream markdown with operator<<")
     .nl();

    // ===== Fin =====
    w << "===";
    w << "## Demo Complete";
    w << "All styles rendered with zero manual configuration.";

    return 0;
}
