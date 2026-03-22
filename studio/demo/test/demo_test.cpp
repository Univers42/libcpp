/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   demo_test.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 19:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/22 19:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** demo_test.cpp — Running a test suite for a calculator module
**
** Scenario: Build a mini calculator, write a full TestSuite with
** before_each/after_each hooks, use Spy to track function calls,
** and use StubInt/StubStr for mock return values. Shows the full
** testing workflow.
**
** Theme: Rose Gold (warm pink-rose on dark burgundy)
*/

#include <sstream>

#include "libcpp/str/format.hpp"
#include "libcpp/test/spy.hpp"
#include "libcpp/test/suite.hpp"
#include "libcpp/term/stylesheet.hpp"
#include "libcpp/term/writer.hpp"

using libcpp::Srgb;
using libcpp::StyleSheet;
using libcpp::TermStyle;
using libcpp::TermWriter;

/* ── Rose Gold theme ─────────────────────────────────────────────────── */

LIBCPP_THEME_BEGIN(rose_gold)
  RULE("h1", fg("#F48FB1").bg("#1A0A10").border("#F48FB1")
                 .bold().center().pad(3, 3, 1).spacing(1, 1)
                 .glyph("\xe2\x97\x86 ").sep("\xe2\x95\x90"))
  RULE("h2", fg("#E91E63").border("#330A18")
                 .bold().pad(1, 1).spacing(1, 0)
                 .glyph("\xe2\x96\xb8 ").sep("\xe2\x94\x81"))
  RULE("h3", fg("#F06292").border("#330A18")
                 .bold().pad(1, 1).spacing(1, 0)
                 .glyph("\xe2\x97\x8f ").sep("\xe2\x95\x8c"))
  RULE("body",    fg("#E8B4C8").pad_left(2))
  RULE("quote",   fg("#A06080").italic().glyph("\xe2\x94\x82 ").margin_left(3))
  RULE("info",    fg("#F48FB1").glyph("\xe2\x84\xb9  "))
  RULE("warn",    fg("#FFD54F").bold().glyph("\xe2\x9a\xa0  "))
  RULE("error",   fg("#EF5350").bold().glyph("\xe2\x9c\x97 "))
  RULE("success", fg("#81C784").glyph("\xe2\x9c\x94 "))
  RULE("trace",   fg("#8A5070").glyph("   "))
  RULE("sep",     border("#330A18").sep("\xe2\x94\x80"))
  RULE("bullet",  fg("#E8B4C8").glyph("\xe2\x96\xaa ").pad_left(4))
  RULE("ol",      fg("#E8B4C8").pad_left(4))
  CALLOUT_RULE("tip",  fg("#FCE4EC").bg("#1A0A10").border("#E91E63")
                           .glyph("\xe2\x9c\x94 "))
  CALLOUT_RULE("note", fg("#F8BBD0").bg("#150810").border("#F48FB1")
                           .glyph("\xe2\x84\xb9  "))
  TABLE_RULE(border_preset(libcpp::BORDER_ROUNDED).border_color("#E91E63")
                 .header_fg("#F48FB1").header_bg("#1A0A10").header_font(libcpp::FONT_BOLD)
                 .header_has_bg(true)
                 .cell_fg("#E8B4C8").cell_bg("#1A0A10").cell_alt_bg("#200D15")
                 .zebra(true).cell_has_bg(true)
                 .title_fg("#F48FB1").title_bg("#1A0A10").title_has_bg(true)
                 .footer_fg("#A06080"))
  WIDTH(74)
LIBCPP_THEME_END

/* ── Mini calculator to test ─────────────────────────────────────────── */

static int calc_add(int a, int b) { return a + b; }
static int calc_sub(int a, int b) { return a - b; }
static int calc_mul(int a, int b) { return a * b; }
static int calc_div(int a, int b) { return b == 0 ? 0 : a / b; }

/* Shared state for before_each demo */
static int g_accumulator = 0;

int main() {
  TermStyle ts;
  rose_gold().apply(ts);
  TermWriter w(ts);

  w << "# Test Suite — Calculator Module";
  w << "";
  w << "Demonstrating `libcpp::test` with a full test suite for a";
  w << "mini calculator: assertions, hooks, spy, and stubs.";

  // ── Run the actual test suite ─────────────────────────────────────
  w << "## Running the Tests";
  w << "The suite output appears on stderr (TestSuite uses its own";
  w << "renderer). Below is a summary of what runs:";
  w << "";

  libcpp::test::TestSuite suite("Calculator");

  suite.before_each([](libcpp::test::TestSuite&) {
    g_accumulator = 0;
  });

  suite.test("add: 2 + 3 = 5", [](libcpp::test::TestSuite& s) {
    ASSERT_EQ(s, calc_add(2, 3), 5);
  });

  suite.test("add: negative numbers", [](libcpp::test::TestSuite& s) {
    ASSERT_EQ(s, calc_add(-10, 3), -7);
    ASSERT_EQ(s, calc_add(-5, -5), -10);
  });

  suite.test("sub: 10 - 4 = 6", [](libcpp::test::TestSuite& s) {
    ASSERT_EQ(s, calc_sub(10, 4), 6);
  });

  suite.test("mul: 7 * 6 = 42", [](libcpp::test::TestSuite& s) {
    ASSERT_EQ(s, calc_mul(7, 6), 42);
  });

  suite.test("mul: zero", [](libcpp::test::TestSuite& s) {
    ASSERT_EQ(s, calc_mul(100, 0), 0);
  });

  suite.test("div: 20 / 4 = 5", [](libcpp::test::TestSuite& s) {
    ASSERT_EQ(s, calc_div(20, 4), 5);
  });

  suite.test("div: by zero returns 0", [](libcpp::test::TestSuite& s) {
    ASSERT_EQ(s, calc_div(42, 0), 0);
  });

  suite.test("before_each resets accumulator", [](libcpp::test::TestSuite& s) {
    ASSERT_EQ(s, g_accumulator, 0);
    g_accumulator = 999;
  });

  suite.test("accumulator is fresh again", [](libcpp::test::TestSuite& s) {
    ASSERT_EQ(s, g_accumulator, 0);
  });

  suite.skip("TODO: floating-point division", [](libcpp::test::TestSuite& s) {
    ASSERT_EQ(s, 1, 1);
  });

  int exit_code = suite.run();
  w << "";

  /* Display summary in our themed output */
  w << "| Metric | Count |";
  w << "| :--- | ---: |";
  w << ("| Total tests | "
      + libcpp::str::to_string(suite.total()) + " |");
  w << ("| Passed | "
      + libcpp::str::to_string(suite.passed()) + " |");
  w << ("| Failed | "
      + libcpp::str::to_string(suite.failed()) + " |");
  w << ("| Skipped | "
      + libcpp::str::to_string(suite.skipped()) + " |");
  w << "";

  std::string status = (exit_code == 0) ? "ALL PASSED" : "SOME FAILED";
  if (exit_code == 0)
    w << ("!v " + status);
  else
    w << ("!x " + status);
  w << "";

  // ── Spy — tracking function calls ────────────────────────────────
  w << "## Spy — Call Tracking";
  w << "Track which functions are called and with what arguments:";
  w << "";

  libcpp::test::Spy spy;

  spy.call("calc_add", calc_add(3, 4));
  spy.call("calc_mul", calc_mul(6, 7));
  spy.call("calc_sub", calc_sub(10, 3));
  spy.call("calc_add", calc_add(100, 200));

  w << "| # | Tag | Argument |";
  w << "| :---: | :--- | ---: |";
  for (int i = 0; i < spy.count(); ++i) {
    w << ("| " + libcpp::str::to_string(i + 1)
        + " | " + spy.at(i).tag
        + " | " + libcpp::str::to_string(static_cast<int>(spy.at(i).arg_int))
        + " |");
  }
  w << "";

  w << ("!i Total calls: " + libcpp::str::to_string(spy.count()));
  w << ("!i Last tag: " + spy.last_tag());
  w << ("!i was_called_with(\"calc_mul\"): "
      + std::string(spy.was_called_with("calc_mul") ? "true" : "false"));
  w << "";

  // ── StubInt & StubStr ─────────────────────────────────────────────
  w << "## Stubs — Mock Return Values";

  libcpp::test::StubInt stub_int(42);
  w << ("!i StubInt initial: " + libcpp::str::to_string(stub_int.get()));
  stub_int.set(99);
  w << ("!i StubInt after set(99): " + libcpp::str::to_string(stub_int.get()));
  w << ("!i StubInt call_count: "
      + libcpp::str::to_string(stub_int.call_count()));
  w << "";

  libcpp::test::StubStr stub_str("hello");
  w << ("!i StubStr initial: \"" + stub_str.get() + "\"");
  stub_str.set("world");
  w << ("!i StubStr after set: \"" + stub_str.get() + "\"");
  w << ("!i StubStr call_count: "
      + libcpp::str::to_string(stub_str.call_count()));
  w << "";

  // ── Assertion macros reference ────────────────────────────────────
  w << "## Assertion Macro Reference";

  w << "| Macro | Description |";
  w << "| :--- | :--- |";
  w << "| ASSERT_TRUE(s, expr) | Expression must be true |";
  w << "| ASSERT_FALSE(s, expr) | Expression must be false |";
  w << "| ASSERT_EQ(s, a, b) | Integer equality |";
  w << "| ASSERT_EQ_STR(s, a, b) | String equality |";
  w << "| ASSERT_EQ_DBL(s, a, b, e) | Double with epsilon |";
  w << "| ASSERT_NEQ(s, a, b) | Not equal |";
  w << "| ASSERT_LT(s, a, b) | Less than |";
  w << "| ASSERT_GT(s, a, b) | Greater than |";
  w << "| ASSERT_NULL(s, p) | Pointer is null |";
  w << "| ASSERT_NOT_NULL(s, p) | Pointer is non-null |";
  w << "";

  // ── Summary callout ───────────────────────────────────────────────
  w << ">![tip] Test Module Summary";
  w << "> `TestSuite` — register/run tests with hooks and assertions";
  w << "> `Spy` — track function calls with tag + argument";
  w << "> `StubInt` / `StubStr` — mock return values with call counting";
  w << "> `ASSERT_*` macros — file:line reporting on failure";
  w << "";

  w << "---";
  w << ("!v Test demo complete — "
      + libcpp::str::to_string(suite.passed()) + " assertions verified.");
  return 0;
}
