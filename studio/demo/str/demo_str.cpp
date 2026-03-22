/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   demo_str.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 16:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/22 16:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** demo_str.cpp — Real-world string processing pipeline
**
** Scenario: A CLI tool that parses messy user input from a form submission,
** normalises identifiers, validates UTF-8, and generates clean output.
** Uses every function from str/case, str/format, and str/utf8.
**
** Theme: Amber Hacker (warm orange-amber on dark)
*/

#include "libcpp/str/case.hpp"
#include "libcpp/str/format.hpp"
#include "libcpp/str/utf8.hpp"
#include "libcpp/term/stylesheet.hpp"
#include "libcpp/term/writer.hpp"

using libcpp::Srgb;
using libcpp::StyleSheet;
using libcpp::TermStyle;
using libcpp::TermWriter;

/* ── Amber Hacker theme ──────────────────────────────────────────────── */

LIBCPP_THEME_BEGIN(amber_hacker)
  RULE("h1", fg("#FFB000").bg("#1A1000").border("#FFB000")
                 .bold().center().pad(3, 3, 1).spacing(1, 1)
                 .glyph("\xe2\x97\x86 ").sep("\xe2\x95\x90"))
  RULE("h2", fg("#FFA000").border("#3D2800")
                 .bold().pad(1, 1).spacing(1, 0)
                 .glyph("\xe2\x96\xb8 ").sep("\xe2\x94\x81"))
  RULE("h3", fg("#E8A020").border("#3D2800")
                 .bold().pad(1, 1).spacing(1, 0)
                 .glyph("\xe2\x97\x8f ").sep("\xe2\x95\x8c"))
  RULE("body",    fg("#D4A860").pad_left(2))
  RULE("quote",   fg("#887040").italic().glyph("\xe2\x94\x82 ").margin_left(3))
  RULE("info",    fg("#FFB000").glyph("\xe2\x84\xb9  "))
  RULE("warn",    fg("#FF8C00").bold().glyph("\xe2\x9a\xa0  "))
  RULE("error",   fg("#FF4500").bold().glyph("\xe2\x9c\x97 "))
  RULE("success", fg("#B8D430").glyph("\xe2\x9c\x94 "))
  RULE("sep",     border("#3D2800").sep("\xe2\x94\x80"))
  RULE("bullet",  fg("#D4A860").glyph("\xe2\x96\xaa ").pad_left(4))
  RULE("ol",      fg("#D4A860").pad_left(4))
  CALLOUT_RULE("tip",  fg("#D4C890").bg("#1A1400").border("#B8A020")
                           .glyph("\xe2\x9c\x94 "))
  CALLOUT_RULE("note", fg("#FFB860").bg("#1A1200").border("#FF9000")
                           .glyph("\xe2\x84\xb9  "))
  TABLE_RULE(border_preset(libcpp::BORDER_ROUNDED).border_color("#FFA000")
                 .header_fg("#FFB000").header_bg("#1A1000")
                 .header_font(libcpp::FONT_BOLD).header_has_bg(true)
                 .cell_fg("#D4A860").cell_bg("#1A1000").cell_alt_bg("#201800")
                 .zebra(true).cell_has_bg(true)
                 .title_fg("#FFB000").title_bg("#1A1000").title_has_bg(true)
                 .footer_fg("#887040"))
  WIDTH(72)
LIBCPP_THEME_END

int main() {
  TermStyle ts;
  amber_hacker().apply(ts);
  TermWriter w(ts);

  w << "# String Processing Pipeline";
  w << "";
  w << "Real scenario: normalising messy user-submitted form data into";
  w << "clean database identifiers, display names, and API slugs.";

  // ── 1. Raw input ──────────────────────────────────────────────────
  w << "## Step 1 — Raw User Input";

  std::string raw[] = {
      "  john DOE  ", "  Ren\xc3\xa9 L'H\xc3\xa9ritier  ",
      " mar\xc3" "\xad" "a GARC\xc3" "\x8d" "A  ",
      "  \xc3\x96zlem Y\xc4\xb1ld\xc4\xb1z "
  };

  w << "| # | Raw Value |";
  w << "| :---: | :--- |";
  for (int i = 0; i < 4; ++i)
    w << ("| " + libcpp::str::to_string(i) + " | [" + raw[i] + "] |");
  w << "";

  // ── 2. Trim & validate UTF-8 ─────────────────────────────────────
  w << "## Step 2 — Trim & Validate UTF-8";

  std::string trimmed[4];
  for (int i = 0; i < 4; ++i)
    trimmed[i] = libcpp::str::trim(raw[i]);

  w << "| Name | Bytes | Codepoints | Valid? |";
  w << "| :--- | :---: | :---: | :---: |";
  for (int i = 0; i < 4; ++i) {
    int bytes = static_cast<int>(trimmed[i].size());
    int cps = libcpp::str::utf8_len(trimmed[i]);
    std::string ok = libcpp::str::utf8_valid(trimmed[i]) ? "yes" : "no";
    w << ("| " + trimmed[i] + " | " + libcpp::str::to_string(bytes)
        + " | " + libcpp::str::to_string(cps) + " | " + ok + " |");
  }
  w << "";

  // ── 3. Case conversions ───────────────────────────────────────────
  w << "## Step 3 — Case Normalisation";
  w << "Generate display names, database keys, and API slugs:";
  w << "";

  w << "| Input | Title | UPPER | snake_case | kebab-case |";
  w << "| :--- | :--- | :--- | :--- | :--- |";
  for (int i = 0; i < 4; ++i) {
    w << ("| " + trimmed[i]
        + " | " + libcpp::str::to_title(trimmed[i])
        + " | " + libcpp::str::to_upper(trimmed[i])
        + " | " + libcpp::str::to_snake_case(trimmed[i])
        + " | " + libcpp::str::to_kebab_case(trimmed[i]) + " |");
  }
  w << "";

  // ── 4. String manipulation ────────────────────────────────────────
  w << "## Step 4 — String Manipulation";

  std::string email = "  user.name@EXAMPLE.COM  ";
  std::string clean = libcpp::str::to_lower(libcpp::str::trim(email));
  w << ("!i Raw email:   [" + email + "]");
  w << ("!i Cleaned:     [" + clean + "]");
  w << "";

  std::string path = "/api/v2/users/42/profile";
  w << ("Analysing URL path: `" + path + "`");
  w << ("- starts_with /api: " + libcpp::str::to_string(
        libcpp::str::starts_with(path, "/api")));
  w << ("- ends_with /profile: " + libcpp::str::to_string(
        libcpp::str::ends_with(path, "/profile")));
  w << ("- contains 'users': " + libcpp::str::to_string(
        libcpp::str::contains(path, "users")));
  w << ("- count of '/': " + libcpp::str::to_string(
        libcpp::str::count(path, "/")));
  w << "";

  /* Split the path */
  std::string parts[16];
  int n = libcpp::str::split(path.substr(1), '/', parts, 16);
  w << "Split path segments:";
  for (int i = 0; i < n; ++i)
    w << (libcpp::str::to_string(i + 1) + ". " + parts[i]);
  w << "";

  /* Padding & truncation */
  w << "### Padding & Truncation";
  w << ("!i pad_left(20, '.'):  [" +
       libcpp::str::pad_left("Status", 20, '.') + "]");
  w << ("!i pad_right(20, '.'): [" +
       libcpp::str::pad_right("Status", 20, '.') + "]");
  w << ("!i center(20, '-'):    [" +
       libcpp::str::center("Status", 20, '-') + "]");
  w << ("!i truncate(8):        [" +
       libcpp::str::truncate("Hello, World!", 8, "...") + "]");
  w << "";

  // ── 5. Format / Message builder ───────────────────────────────────
  w << "## Step 5 — Template Formatting";

  std::string tpl = "Hello {0}, you have {1} unread messages from {2}.";
  std::string result = libcpp::str::fmt(tpl, "John", "5", "admin@42.fr");
  w << ("> " + result);
  w << "";

  libcpp::str::Message msg;
  msg << "Build report: " << 39 << " objects compiled in " << 1.42 << "s";
  w << ("!v " + msg.str());
  w << "";

  std::string csv = "John,Doe,42,Paris";
  w << ("!i replace_all(',', ' | '): "
      + libcpp::str::replace_all(csv, ",", " | "));
  w << "";

  std::string mods[] = {"async", "bench", "core", "data", "log"};
  w << ("!i join(' > '): " + libcpp::str::join(mods, 5, " > "));
  w << "";

  // ── Summary callout ───────────────────────────────────────────────
  w << ">![tip] String Module Summary";
  w << "> `case.hpp` — 9 conversion functions (upper, lower, title, snake...)";
  w << "> `format.hpp` — fmt(), split(), join(), trim(), pad(), Message...";
  w << "> `utf8.hpp` — encode/decode, validate, length, substr, wstring";
  w << "";

  w << "---";
  w << "!v String pipeline complete — all input normalised.";
  return 0;
}
