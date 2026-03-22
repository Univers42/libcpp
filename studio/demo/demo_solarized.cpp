/* ************************************************************************** */
/*  demo_solarized.cpp -- Exhaustive TermWriter demo using Solarized theme    */
/*                                                                            */
/*  This demo loads the Solarized Dark theme from themes/theme_solarized.hpp  */
/*  and exercises EVERY rendering feature with content themed around */
/*  a testing& quality assurance workflow.                                    */
/*                                                                            */
/*  Config: themes/theme_solarized.hpp                                        */
/*  Build:  make compile_studio                                               */
/*  Run:    ./build/bin/studio/demo/demo_solarized                            */
/* ************************************************************************** */

#include <iostream>
#include "libcpp/term/color.hpp"
#include "libcpp/term/stylesheet.hpp"
#include "libcpp/term/table.hpp"
#include "libcpp/term/writer.hpp"
#include "themes/theme_solarized.hpp"

using libcpp::Gradient;
using libcpp::Palette;
using libcpp::Srgb;
using libcpp::Table;
using libcpp::TermStyle;
using libcpp::TermUtils;
using libcpp::TermWriter;

int main() {
  /* ── Load the Solarized theme ────────────────────────────── */
  TermStyle ts;
  TermWriter w(ts);
  solarized_theme().apply(ts, w);

  std::cout << "\n";

  // ════════════════════════════════════════════════════════════
  //  1. HEADINGS
  // ════════════════════════════════════════════════════════════
  w << "# Solarized Dark -- Test Report";
  w << "Ethan Schoonover's precision-engineered color palette.";
  w << "Designed for readability across all lighting conditions.";

  w << "## Test Suite Results";
  w << "### Unit Test Summary";
  w << "Each suite is an isolated compilation unit with its own";
  w << "setup/teardown, assertions, and optional fuzzing.";

  // ════════════════════════════════════════════════════════════
  //  2. CALLOUT BLOCKS
  // ════════════════════════════════════════════════════════════
  w << "## Test Observations";

  w << ">![tip] Testing Best Practice";
  w << "> Write the test before the implementation.";
  w << "> If you can not test it, you can not trust it.";
  w << "";

  w << ">![note] Solarized Science";
  w << "> Both dark and light variants share the same";
  w << "> 8 accent colors. Only the base tones swap.";
  w << "";

  w << ">![warning] Flaky Test Detected";
  w << "> test_thread_pool::concurrent_submit fails 2% of";
  w << "> the time under heavy system load. Needs mutex fix.";
  w << "";

  w << ">![danger] Data Loss Risk";
  w << "> The snapshot test writes to /tmp. If /tmp is";
  w << "> a tmpfs, data is lost on reboot. Use a stable path.";
  w << "";

  w << ">![important] Coverage Threshold";
  w << "> Minimum 85% line coverage required for merge.";
  w << "> Current: 91.4% (up from 78% last sprint).";
  w << "";

  w << ">![success] All Suites Green";
  w << "> 36 tests across 8 suites: 36 passed, 0 failed.";
  w << "> Execution time: 11.7ms total.";
  w << "";

  w << ">![error] Assertion Failure";
  w << "> EXPECT_EQ(result.size(), 5) failed:";
  w << ">   actual=4, expected=5 at fuzzer.cpp:89";
  w << "";

  w << ">![info] Test Framework";
  w << "> Custom test::Suite with colored output, timers,";
  w << "> spies, snapshot comparison, and fuzz generation.";
  w << "";

  // ════════════════════════════════════════════════════════════
  //  3. LOG LEVELS
  // ════════════════════════════════════════════════════════════
  w << "## Test Runner Output";
  w << "!i [test] Running suite: core(8 cases)";
  w << "!v [test] PASS: arena_alloc_free(0.004ms)";
  w << "!v [test] PASS: observer_notify(0.012ms)";
  w << "!w [test] SLOW: memento_deep_copy(45ms > 10ms threshold)";
  w << "!x [test] FAIL: state_machine_transition -- bad state";
  w << "!d [test] ASAN: use-after-free in singleton cleanup";
  w << "!t [test] Suite complete: 7 passed, 1 failed(47ms)";

  // ════════════════════════════════════════════════════════════
  //  4. INLINE FORMATTING
  // ════════════════════════════════════════════════════════════
  w << "## Formatting in Reports";
  w << "**CRITICAL: test_state_machine needs immediate fix**";
  w << "*Note: flaky tests are quarantined in slow_tests/*";
  w << "~~Old snapshot format(.snap.txt) is deprecated~~";
  w << "~Run ID: 4f8a2c1e-b930-4d7a-8e12-6a4c3f9d5b7e~";
  w << "__Underlined text for section cross-references__";
  w << "`EXPECT_EQ(result.size(), 5); // assertion at fuzzer.cpp:89`";

  // ════════════════════════════════════════════════════════════
  //  5. LISTS
  // ════════════════════════════════════════════════════════════
  w << "## Test Categories";
  w << "### By Type";
  w << "- Unit tests: isolated function/method verification";
  w << "- Integration tests: multi-module interaction";
  w << "- Fuzz tests: randomized input generation";
  w << "- Snapshot tests: output comparison against baseline";
  w << "- Performance tests: timing against thresholds";
  w << "- Memory tests: leak detection with LeakGuard";
  w << "";
  w << "### Failure Triage Steps";
  w << "1. Read the assertion message and file:line";
  w << "2. Reproduce locally with the exact same seed";
  w << "3. Check if it is a flaky test(run 10x)";
  w << "4. Inspect git blame for recent changes to that area";
  w << "5. Write a minimal reproduction case";
  w << "6. Fix the root cause, not just the symptom";
  w << "7. Add a regression test for the specific bug";

  // ════════════════════════════════════════════════════════════
  //  6. BLOCKQUOTES
  // ════════════════════════════════════════════════════════════
  w << "## On Testing";
  w << "> Testing shows the presence, not the absence of bugs.";
  w << "> -- Edsger W. Dijkstra";
  w << "";
  w << "> A test that does not fail is not a test at all.";
  w << "> It is a confirmation of your assumptions.";

  // ════════════════════════════════════════════════════════════
  //  7. SECTIONS
  // ════════════════════════════════════════════════════════════
  w << "## Environment";
  w << "$$ Compiler / c++ (clang 17.0.6) with -std=c++17 -fsanitize=address";
  w << "$$ Platform / Linux 6.5.0-44, x86_64, 16GB RAM";
  w << "$$ Runner / Custom test::Suite with ANSI colored output";

  // ════════════════════════════════════════════════════════════
  //  8. MARKDOWN TABLE
  // ════════════════════════════════════════════════════════════
  w << "## Suite Breakdown";
  w << "| Suite | Cases | Pass | Fail | Time |";
  w << "| :--- | :---: | :---: | :---: | ---: |";
  w << "| bench | 3 | 3 | 0 | 2.1ms |";
  w << "| core | 8 | 7 | 1 | 1.8ms |";
  w << "| data | 5 | 5 | 0 | 0.9ms |";
  w << "| libftpp | 4 | 4 | 0 | 0.6ms |";
  w << "| mem | 3 | 3 | 0 | 0.4ms |";
  w << "| str | 5 | 5 | 0 | 0.7ms |";
  w << "| util | 6 | 6 | 0 | 3.1ms |";
  w << "| term | 2 | 2 | 0 | 2.1ms |";
  w << "";

  // ════════════════════════════════════════════════════════════
  //  9. IMPERATIVE TABLE
  // ════════════════════════════════════════════════════════════
  w << "### Solarized Color Map";
  {
    Table t(4);
    t.set_title("Solarized Dark Palette");
    std::string h[4] = {"Name", "Hex", "L*a*b* L", "Role"};
    t.header(h, 4);
    std::string rows[][4] = {
        {"base03", "#002B36", "15", "Background(dark)"},
        {"base02", "#073642", "20", "Highlights"},
        {"base01", "#586E75", "45", "Comments, secondary"},
        {"base00", "#657B83", "50", "Body text(light bg)"},
        {"base0", "#839496", "60", "Body text(dark bg)"},
        {"base1", "#93A1A1", "65", "Emphasis(light bg)"},
        {"base2", "#EEE8D5", "92", "Background(light)"},
        {"base3", "#FDF6E3", "97", "Highlights(light)"},
        {"yellow", "#B58900", "60", "Warnings, modified"},
        {"orange", "#CB4B16", "50", "H1 border, alert"},
        {"red", "#DC322F", "50", "Errors, danger"},
        {"magenta", "#D33682", "50", "Keywords, tags"},
        {"violet", "#6C71C4", "50", "Trace, numbers"},
        {"blue", "#268BD2", "55", "H2, info, links"},
        {"cyan", "#2AA198", "60", "Info, constants"},
        {"green", "#859900", "60", "H3, success, strings"}};
    for (int i = 0; i < 16; ++i) t.row(rows[i], 4);
    w.table(t);
  }

  // ════════════════════════════════════════════════════════════
  //  10. COLOR ENGINE
  // ════════════════════════════════════════════════════════════
  w << "## Color Engine";
  w << "### Solarized Accent Gradient";
  {
    Gradient g(Srgb::from_hex("#DC322F"), Srgb::from_hex("#859900"));
    g.add_stop(Srgb::from_hex("#CB4B16"));
    g.add_stop(Srgb::from_hex("#B58900"));
    g.add_stop(Srgb::from_hex("#2AA198"));
    g.add_stop(Srgb::from_hex("#268BD2"));
    g.add_stop(Srgb::from_hex("#6C71C4"));
    std::cout
        << "  "
        << g.apply("red -> orange -> yellow -> cyan -> blue -> violet -> green")
        << "\n";

    Gradient g2(Srgb::from_hex("#002B36"), Srgb::from_hex("#FDF6E3"));
    std::cout
        << "  "
        << g2.apply(
               "Dark ==============================================> Light")
        << "\n";

    std::string bar;
    for (int i = 0; i < 60; ++i) bar += "\xe2\x96\x88";
    Gradient g3(Srgb::from_hex("#268BD2"), Srgb::from_hex("#2AA198"));
    g3.add_stop(Srgb::from_hex("#859900"));
    std::cout << "  " << g3.apply(bar) << "\n";
  }
  std::cout << "\n";

  w << "### Palette Swatches";
  {
    struct Swatch {
      const char* name;
      const char* hex;
    };
    Swatch sw[] = {{"Red", "#DC322F"},    {"Orange", "#CB4B16"},
                   {"Yellow", "#B58900"}, {"Green", "#859900"},
                   {"Cyan", "#2AA198"},   {"Blue", "#268BD2"},
                   {"Violet", "#6C71C4"}, {"Magenta", "#D33682"}};
    std::cout << "  ";
    for (int i = 0; i < 8; ++i) {
      Srgb c = Srgb::from_hex(sw[i].hex);
      std::cout << c.to_ansi_fg() << "\xe2\x96\x88\xe2\x96\x88 " << sw[i].name
                << TermUtils::reset();
      if (i < 7) std::cout << "  ";
    }
    std::cout << "\n\n";
  }

  // ════════════════════════════════════════════════════════════
  //  11. IMPERATIVE CHAINING
  // ════════════════════════════════════════════════════════════
  w << "## Conclusions";
  w.info("Solarized is scientifically designed for readability")
      .warn("Both dark and light share the same 8 accents")
      .success("36 / 36 tests passing -- zero regressions")
      .error("1 known flaky test quarantined in slow_tests/")
      .danger("ASAN found 1 use-after-free in singleton cleanup")
      .code("solarized_theme().apply(ts, w);")
      .bullet("L*a*b* lightness values ensure uniform perceived brightness")
      .bullet("All accent colors are perceptually equidistant")
      .bullet("Background swap(dark <-> light) preserves contrast ratios")
      .ordered(1, "Include themes/theme_solarized.hpp")
      .ordered(2, "Call solarized_theme().apply(ts, w)")
      .ordered(3, "Create TermWriter and enjoy precision colors")
      .nl();

  // ════════════════════════════════════════════════════════════
  //  FIN
  // ════════════════════════════════════════════════════════════
  w << "===";
  w << "## Theme: Solarized Dark -- Complete";
  w << "Configuration file: themes/theme_solarized.hpp";
  w << "Precision colors for maximum readability in any lighting.";

  return 0;
}
