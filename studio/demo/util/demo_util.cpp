/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   demo_util.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 19:30:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/22 19:30:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** demo_util.cpp — DevOps tool configuration and profiling
**
** Scenario: A deployment tool that parses CLI arguments, loads an INI
** configuration, benchmarks operations with Chronometer, and schedules
** periodic health checks with Timer. Real-world DevOps workflow.
**
** Theme: Forest Moss (earthy green on dark olive)
*/

#include <chrono>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <thread>

#include "libcpp/str/format.hpp"
#include "libcpp/util/argparser.hpp"
#include "libcpp/util/chronometer.hpp"
#include "libcpp/util/config.hpp"
#include "libcpp/util/timer.hpp"
#include "libcpp/term/stylesheet.hpp"
#include "libcpp/term/writer.hpp"

using libcpp::Srgb;
using libcpp::StyleSheet;
using libcpp::TermStyle;
using libcpp::TermWriter;

/* ── Forest Moss theme ───────────────────────────────────────────────── */

LIBCPP_THEME_BEGIN(forest_moss)
  RULE("h1", fg("#8BC34A").bg("#0A1A05").border("#8BC34A")
                 .bold().center().pad(3, 3, 1).spacing(1, 1)
                 .glyph("\xe2\x97\x86 ").sep("\xe2\x95\x90"))
  RULE("h2", fg("#689F38").border("#0D2208")
                 .bold().pad(1, 1).spacing(1, 0)
                 .glyph("\xe2\x96\xb8 ").sep("\xe2\x94\x81"))
  RULE("h3", fg("#AED581").border("#0D2208")
                 .bold().pad(1, 1).spacing(1, 0)
                 .glyph("\xe2\x97\x8f ").sep("\xe2\x95\x8c"))
  RULE("body",    fg("#C5D8A8").pad_left(2))
  RULE("quote",   fg("#7A9060").italic().glyph("\xe2\x94\x82 ").margin_left(3))
  RULE("info",    fg("#8BC34A").glyph("\xe2\x84\xb9  "))
  RULE("warn",    fg("#FDD835").bold().glyph("\xe2\x9a\xa0  "))
  RULE("error",   fg("#E53935").bold().glyph("\xe2\x9c\x97 "))
  RULE("success", fg("#66BB6A").glyph("\xe2\x9c\x94 "))
  RULE("trace",   fg("#5A7040").glyph("   "))
  RULE("sep",     border("#0D2208").sep("\xe2\x94\x80"))
  RULE("bullet",  fg("#C5D8A8").glyph("\xe2\x96\xaa ").pad_left(4))
  RULE("ol",      fg("#C5D8A8").pad_left(4))
  CALLOUT_RULE("tip",  fg("#DCEDC8").bg("#0A1A05").border("#689F38")
                           .glyph("\xe2\x9c\x94 "))
  CALLOUT_RULE("note", fg("#C5E1A5").bg("#081505").border("#8BC34A")
                           .glyph("\xe2\x84\xb9  "))
  TABLE_RULE(border_preset(libcpp::BORDER_ROUNDED).border_color("#689F38")
                 .header_fg("#8BC34A").header_bg("#0A1A05").header_font(libcpp::FONT_BOLD)
                 .header_has_bg(true)
                 .cell_fg("#C5D8A8").cell_bg("#0A1A05").cell_alt_bg("#0D1A08")
                 .zebra(true).cell_has_bg(true)
                 .title_fg("#8BC34A").title_bg("#0A1A05").title_has_bg(true)
                 .footer_fg("#7A9060"))
  WIDTH(74)
LIBCPP_THEME_END

/* helper: double to string with precision */
static std::string dtos(double v, int prec) {
  std::ostringstream o;
  o << std::fixed << std::setprecision(prec) << v;
  return o.str();
}

int main() {
  TermStyle ts;
  forest_moss().apply(ts);
  TermWriter w(ts);

  w << "# DevOps Tool — Config & Profiling";
  w << "";
  w << "A deployment tool demonstrating `libcpp::util`: CLI argument";
  w << "parsing, INI configuration, chronometer profiling, and timers.";

  // ── ArgParser — CLI argument parsing ──────────────────────────────
  w << "## ArgParser — Command-Line Interface";

  libcpp::util::ArgParser parser("deploy", "Deployment automation tool");
  parser.version("1.2.0");
  parser.flag("-v", "--verbose", "Enable verbose output");
  parser.flag("-d", "--dry-run", "Simulate without executing");
  parser.flag("-f", "--force", "Force overwrite existing deployments");
  parser.option("-c", "--config", "Path to config file", "/etc/deploy.ini");
  parser.option("-e", "--env", "Target environment", "staging");
  parser.option("-p", "--port", "Server port", "8080");
  parser.option("-t", "--timeout", "Connection timeout (ms)", "5000");

  /* Simulate: deploy --verbose --env production --port 443 app.tar.gz */
  const char* argv[] = {
      "deploy", "--verbose", "--env", "production",
      "--port", "443", "app.tar.gz"
  };
  int argc = 7;
  parser.parse(argc, const_cast<char**>(argv));

  w << "Simulated command:";
  w << "> `deploy --verbose --env production --port 443 app.tar.gz`";
  w << "";

  w << "| Flag / Option | Value |";
  w << "| :--- | :--- |";
  w << ("| --verbose | " + std::string(parser.has("--verbose") ? "true" : "false") + " |");
  w << ("| --dry-run | " + std::string(parser.has("--dry-run") ? "true" : "false") + " |");
  w << ("| --force | " + std::string(parser.has("--force") ? "true" : "false") + " |");
  w << ("| --config | " + parser.get("--config") + " |");
  w << ("| --env | " + parser.get("--env") + " |");
  w << ("| --port | " + parser.get("--port") + " |");
  w << ("| --timeout | " + parser.get("--timeout") + " |");
  w << "";

  if (parser.positional_count() > 0)
    w << ("!i Positional arg: " + parser.positional(0));
  w << "";

  // ── Config — INI-style configuration ──────────────────────────────
  w << "## Config — INI Configuration";

  libcpp::util::Config cfg;
  cfg.load_string(
      "[server]\n"
      "host = 0.0.0.0\n"
      "port = 8080\n"
      "workers = 8\n"
      "\n"
      "[database]\n"
      "host = localhost\n"
      "port = 5432\n"
      "name = myapp_prod\n"
      "pool_min = 4\n"
      "pool_max = 32\n"
      "\n"
      "[logging]\n"
      "level = info\n"
      "file = /var/log/deploy.log\n"
      "rotate = true\n"
      "\n"
      "[tls]\n"
      "enabled = true\n"
      "cert = /etc/ssl/cert.pem\n"
      "key = /etc/ssl/key.pem\n"
  );

  w << "Loaded INI configuration:";
  w << "";

  std::string sections[8];
  int sec_count = cfg.sections(sections, 8);

  w << "| Section | Key | Value |";
  w << "| :--- | :--- | :--- |";
  for (int s = 0; s < sec_count; ++s) {
    for (int e = 0; e < cfg.entry_count(); ++e) {
      const auto& entry = cfg.entry(e);
      if (entry.section == sections[s]) {
        w << ("| " + entry.section + " | " + entry.key
            + " | " + entry.value + " |");
      }
    }
  }
  w << "";

  /* Query specific values */
  w << "### Querying Configuration";
  w << ("!i server.port = " + cfg.get("server", "port"));
  w << ("!i database.pool_max = "
      + libcpp::str::to_string(cfg.get_int("database", "pool_max", 0)));
  w << ("!i tls.enabled = "
      + std::string(cfg.get_bool("tls", "enabled", false) ? "true" : "false"));
  w << ("!i logging.file = " + cfg.get("logging", "file"));
  w << "";

  /* Modify and serialise */
  cfg.set("server", "port", parser.get("--port"));
  w << ("!v Updated server.port to " + cfg.get("server", "port")
      + " (from CLI)");
  w << "";

  // ── Chronometer — benchmarking ────────────────────────────────────
  w << "## Chronometer — Performance Profiling";
  w << "Benchmarking three simulated operations:";
  w << "";

  libcpp::util::Chronometer chrono;

  /* Benchmark 1: string concatenation */
  chrono.start();
  {
    std::string big;
    for (int i = 0; i < 10000; ++i)
      big += "x";
  }
  chrono.stop();
  double t1 = chrono.elapsedMicroseconds();

  /* Benchmark 2: math computation */
  chrono.reset();
  chrono.start();
  {
    volatile double sum = 0;
    for (int i = 0; i < 50000; ++i)
      sum += std::sqrt(static_cast<double>(i));
    (void)sum;
  }
  chrono.stop();
  double t2 = chrono.elapsedMicroseconds();

  /* Benchmark 3: config parse */
  chrono.reset();
  chrono.start();
  {
    libcpp::util::Config tmp;
    for (int i = 0; i < 100; ++i) {
      tmp.load_string("[bench]\nkey = value\n");
      tmp.clear();
    }
  }
  chrono.stop();
  double t3 = chrono.elapsedMicroseconds();

  w << "| Operation | Time |";
  w << "| :--- | ---: |";
  w << ("| 10k string concats | " + dtos(t1, 0) + " us |");
  w << ("| 50k sqrt() calls | " + dtos(t2, 0) + " us |");
  w << ("| 100 config parses | " + dtos(t3, 0) + " us |");
  w << "";

  w << ("!v Total profiled: " + dtos((t1 + t2 + t3) / 1000.0, 2) + " ms");
  w << "";

  // ── Timer — scheduled callbacks ───────────────────────────────────
  w << "## Timer — Health Check Scheduler";
  w << "Schedule a repeating health check every 50ms (3 ticks):";
  w << "";

  libcpp::util::Timer timer;
  int tick_count = 0;

  timer.setInterval([&]() {
    ++tick_count;
  }, std::chrono::milliseconds(50));

  /* Let it tick 3 times */
  std::this_thread::sleep_for(std::chrono::milliseconds(180));
  timer.stop();

  w << ("!i Timer ticked " + libcpp::str::to_string(tick_count) + " times");
  w << ("!v Timer stopped (isRunning: "
      + std::string(timer.isRunning() ? "true" : "false") + ")");
  w << "";

  // ── Summary callout ───────────────────────────────────────────────
  w << ">![tip] Util Module Summary";
  w << "> `ArgParser` — CLI flags, options, positional args, help text";
  w << "> `Config` — INI-style sections, get/set, type-safe queries";
  w << "> `Chronometer` — high-res timing with pause/resume";
  w << "> `Timer` — setTimeout + setInterval with background thread";
  w << "";

  w << "---";
  w << "!v DevOps tool demo complete — all utilities exercised.";
  return 0;
}
