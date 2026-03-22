/* ═══════════════════════════════════════════════════════════
 *  termstyle_showcase.cpp
 *
 *  Full demonstration of the TermStyle + TermConf system
 *  using the libc (namespaced libcpp::) API.
 *
 *  Build:  make compile_studio
 *  Run:    ./build/bin/studio/termstyle_showcase
 * ═══════════════════════════════════════════════════════════ */

#include "libcpp/term/style.hpp"
#include "libcpp/term/color.hpp"
#include <iostream>

using libcpp::TermStyle;
using libcpp::TermConf;
using libcpp::TermUtils;
using libcpp::Srgb;
using libcpp::Palette;

int main() {

    /* ══════════════════════════════════════════════════════
     *  PART A — Default preset
     * ══════════════════════════════════════════════════════ */
    {
        TermStyle ts;
        TermConf::apply_default(ts);

        std::cout << ts.h1("PART A: Default Preset") << std::endl;
        std::cout << ts.h2("Heading Levels") << std::endl;
        std::cout << ts.h3("h3 — Third-Level Heading") << std::endl;
        std::cout << ts.separator() << std::endl;

        std::cout << ts.info("Informational message")   << std::endl;
        std::cout << ts.warn("Warning message")          << std::endl;
        std::cout << ts.error("Error message")           << std::endl;
        std::cout << ts.success("Success message")       << std::endl;
        std::cout << ts.separator() << std::endl;

        std::cout << ts.h3("Inline Styles") << std::endl;
        std::cout << ts.bold("Bold text")                << std::endl;
        std::cout << ts.italic("Italic text")            << std::endl;
        std::cout << ts.underline("Underlined text")     << std::endl;
        std::cout << ts.dim("Dim text")                  << std::endl;
        std::cout << ts.separator() << std::endl;

        std::cout << ts.h3("Block Elements") << std::endl;
        std::cout << ts.quote("The only way to do great work") << std::endl;
        std::cout << ts.quote("is to love what you do.")       << std::endl;
        std::cout << ts.separator() << std::endl;
        std::cout << ts.hr() << std::endl;
        std::cout << ts.section("Section Title",
            "Body text inside a section block.") << std::endl;
    }

    /* ══════════════════════════════════════════════════════
     *  PART B — Minimal preset
     * ══════════════════════════════════════════════════════ */
    {
        TermStyle ts;
        TermConf::apply_minimal(ts);

        std::cout << ts.h1("PART B: Minimal Preset") << std::endl;
        std::cout << ts.h2("Second-Level Heading")   << std::endl;
        std::cout << ts.info("Minimal info")         << std::endl;
        std::cout << ts.warn("Minimal warning")      << std::endl;
        std::cout << ts.error("Minimal error")       << std::endl;
        std::cout << ts.success("Minimal success")   << std::endl;
        std::cout << ts.separator() << std::endl;
        std::cout << ts.quote("A minimal quote.") << std::endl;
    }

    /* ══════════════════════════════════════════════════════
     *  PART C — Compact preset (zero spacing)
     * ══════════════════════════════════════════════════════ */
    {
        TermStyle ts;
        TermConf::apply_compact(ts);

        std::cout << ts.h1("PART C: Compact Preset") << std::endl;
        std::cout << ts.info("Compact info")         << std::endl;
        std::cout << ts.warn("Compact warning")      << std::endl;
        std::cout << ts.error("Compact error")       << std::endl;
        std::cout << ts.success("Compact success")   << std::endl;
        std::cout << ts.separator() << std::endl;
        std::cout << ts.quote("Compact quote.") << std::endl;
    }

    /* ══════════════════════════════════════════════════════
     *  PART D — Custom user configuration
     * ══════════════════════════════════════════════════════ */
    {
        TermStyle ts;
        TermConf conf;

        conf.global_width      = 70;
        conf.h1_space_before   = 2;
        conf.h1_space_after    = 1;
        conf.apply(ts);

        std::cout << ts.h1("PART D: Custom Config") << std::endl;
        std::cout << ts.h2("Custom Second-Level")   << std::endl;
        std::cout << ts.info("Custom info")          << std::endl;
        std::cout << ts.error("Custom error")        << std::endl;
        std::cout << ts.success("Custom success")    << std::endl;
        std::cout << ts.separator() << std::endl;
        std::cout << ts.quote("Custom quote") << std::endl;
        std::cout << ts.section("Custom Section",
            "Body with custom width.") << std::endl;
    }

    /* ══════════════════════════════════════════════════════
     *  PART E — Glyph constants catalog
     * ══════════════════════════════════════════════════════ */
    {
        TermStyle ts;
        TermConf::apply_default(ts);

        std::cout << ts.h1("PART E: Glyph Constants") << std::endl;
        std::cout << "  BULLET  = " << libcpp::Glyph::BULLET  << std::endl;
        std::cout << "  DIAMOND = " << libcpp::Glyph::DIAMOND << std::endl;
        std::cout << "  ARROW   = " << libcpp::Glyph::ARROW   << std::endl;
        std::cout << "  STAR    = " << libcpp::Glyph::STAR    << std::endl;
        std::cout << "  CHECK   = " << libcpp::Glyph::CHECK   << std::endl;
        std::cout << "  CROSS   = " << libcpp::Glyph::CROSS   << std::endl;
        std::cout << "  DOT     = " << libcpp::Glyph::DOT     << std::endl;
        std::cout << "  HDASH   = " << libcpp::Glyph::HDASH
                  << libcpp::Glyph::HDASH << libcpp::Glyph::HDASH
                  << libcpp::Glyph::HDASH << std::endl;
    }

    /* ══════════════════════════════════════════════════════
     *  PART F — Palette & Gradient
     * ══════════════════════════════════════════════════════ */
    {
        TermStyle ts;
        TermConf::apply_default(ts);

        std::cout << ts.h1("PART F: Colors & Palette") << std::endl;

        Srgb colors[] = {
            Palette::red(), Palette::green(), Palette::blue(),
            Palette::yellow(), Palette::magenta(), Palette::cyan(),
            Palette::orange(), Palette::purple(), Palette::pink()
        };
        const char* names[] = {
            "red", "green", "blue", "yellow", "magenta", "cyan",
            "orange", "purple", "pink"
        };
        for (int i = 0; i < 9; ++i) {
            std::cout << "  " << colors[i].to_ansi_fg()
                      << libcpp::Glyph::BULLET << " " << names[i]
                      << " (" << colors[i].to_hex() << ")"
                      << TermUtils::reset() << std::endl;
        }

        std::cout << std::endl;
        libcpp::Gradient grad(Palette::blue(), Palette::red());
        grad.add_stop(Palette::green());
        std::cout << "  Gradient: " << grad.apply("========== gradient demo ==========")
                  << std::endl;
    }

    /* ══════════════════════════════════════════════════════
     *  PART G — Font flag combinations
     * ══════════════════════════════════════════════════════ */
    {
        TermStyle ts;
        TermConf::apply_default(ts);

        std::cout << ts.h1("PART G: Font Flag Combos") << std::endl;
        std::cout << ts.bold("BOLD")                << std::endl;
        std::cout << ts.dim("DIM")                  << std::endl;
        std::cout << ts.italic("ITALIC")            << std::endl;
        std::cout << ts.underline("UNDERLINE")      << std::endl;
    }

    /* ── Done ──────────────────────────────────────────── */
    {
        TermStyle ts;
        TermConf::apply_default(ts);
        std::cout << ts.h1("Showcase Complete") << std::endl;
    }

    return 0;
}
