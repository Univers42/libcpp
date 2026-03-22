/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   demo_log.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 17:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/22 17:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** demo_log.cpp — Application boot sequence with structured logging
**
** Scenario: Simulate a server startup that initialises subsystems,
** connects to a database, loads configuration, and opens a socket.
** Demonstrates ConsoleLogger, FileLogger, NullLogger, decorators,
** log levels, and convenience macros.
**
** Theme: Matrix Green (phosphor green on black)
*/

#include <sstream>

#include "libcpp/log/logger.hpp"
#include "libcpp/log/macros.hpp"
#include "libcpp/str/format.hpp"
#include "libcpp/term/stylesheet.hpp"
#include "libcpp/term/writer.hpp"

using libcpp::Srgb;
using libcpp::StyleSheet;
using libcpp::TermStyle;
using libcpp::TermWriter;

/* ── Matrix Green theme ──────────────────────────────────────────────── */

LIBCPP_THEME_BEGIN(matrix_green)
  RULE("h1", fg("#00FF41").bg("#0A0F0A").border("#00FF41")
                 .bold().center().pad(3, 3, 1).spacing(1, 1)
                 .glyph("\xe2\x97\x86 ").sep("\xe2\x95\x90"))
  RULE("h2", fg("#00CC33").border("#0D1A0D")
                 .bold().pad(1, 1).spacing(1, 0)
                 .glyph("\xe2\x96\xb8 ").sep("\xe2\x94\x81"))
  RULE("h3", fg("#33FF66").border("#0D1A0D")
                 .bold().pad(1, 1).spacing(1, 0)
                 .glyph("\xe2\x97\x8f ").sep("\xe2\x95\x8c"))
  RULE("body",    fg("#88CC88").pad_left(2))
  RULE("quote",   fg("#558855").italic().glyph("\xe2\x94\x82 ").margin_left(3))
  RULE("info",    fg("#00FF41").glyph("\xe2\x84\xb9  "))
  RULE("warn",    fg("#FFAA00").bold().glyph("\xe2\x9a\xa0  "))
  RULE("error",   fg("#FF3333").bold().glyph("\xe2\x9c\x97 "))
  RULE("success", fg("#00FF41").glyph("\xe2\x9c\x94 "))
  RULE("danger",  fg("#FF0000").bold().glyph("\xe2\x9c\x97 "))
  RULE("trace",   fg("#447744").glyph("   "))
  RULE("sep",     border("#0D1A0D").sep("\xe2\x94\x80"))
  RULE("bullet",  fg("#88CC88").glyph("\xe2\x96\xaa ").pad_left(4))
  RULE("ol",      fg("#88CC88").pad_left(4))
  CALLOUT_RULE("tip",  fg("#AAFFAA").bg("#0A140A").border("#00CC33")
                           .glyph("\xe2\x9c\x94 "))
  CALLOUT_RULE("note", fg("#66FF99").bg("#0A100A").border("#00FF41")
                           .glyph("\xe2\x84\xb9  "))
  TABLE_RULE(border_preset(libcpp::BORDER_ROUNDED).border_color("#00CC33")
                 .header_fg("#00FF41").header_bg("#0A0F0A").header_font(libcpp::FONT_BOLD)
                 .header_has_bg(true)
                 .cell_fg("#88CC88").cell_bg("#0A0F0A").cell_alt_bg("#0D140D")
                 .zebra(true).cell_has_bg(true)
                 .title_fg("#00FF41").title_bg("#0A0F0A").title_has_bg(true)
                 .footer_fg("#558855"))
  WIDTH(74)
LIBCPP_THEME_END

int main() {
  TermStyle ts;
  matrix_green().apply(ts);
  TermWriter w(ts);

  w << "# Application Boot Sequence";
  w << "";
  w << "Simulated server startup demonstrating the `libcpp::log` module:";
  w << "loggers, decorators, macros, and log level filtering.";

  // ── Logger types ──────────────────────────────────────────────────
  w << "## Logger Types";

  w << "| Logger | Description | Output |";
  w << "| :--- | :--- | :--- |";
  w << "| ConsoleLogger | Colored output to stderr | Terminal |";
  w << "| FileLogger | Appends to a log file | Disk |";
  w << "| NullLogger | Discards all messages | /dev/null |";
  w << "| TimestampDecorator | Prepends timestamp | Wraps inner |";
  w << "| LogColorDecorator | Adds ANSI colors | Wraps inner |";
  w << "";

  // ── Set up the logging stack ──────────────────────────────────────
  w << "## Initialising the Log Stack";
  w << "";
  w << "1. Create a `ConsoleLogger` as the base sink";
  w << "2. Wrap it with `TimestampDecorator` for timestamps";
  w << "3. Wrap that with `LogColorDecorator` for colored output";
  w << "4. Set it as the global logger via `set_global()`";
  w << "";

  libcpp::log::ConsoleLogger console;
  console.set_min_level(libcpp::log::LTRACE);
  libcpp::log::TimestampDecorator stamped(&console);
  libcpp::log::LogColorDecorator colored(&stamped);
  libcpp::log::set_global(&colored);

  w << "!v Logger stack initialised (Console -> Timestamp -> Color)";
  w << "";

  // ── Simulated boot sequence ───────────────────────────────────────
  w << "## Server Boot Sequence";
  w << "The following log messages are sent through the decorator chain:";
  w << "";

  LOG_INFO("=== Server boot sequence started ===");
  LOG_TRACE("Loading configuration from /etc/app/config.ini");
  LOG_DEBUG("Config: max_connections=128, timeout_ms=5000");
  LOG_INFO("Configuration loaded: 42 entries from 3 sections");

  w << "!t Config loaded — 42 entries from 3 sections";
  w << "";

  LOG_INFO("Initialising database connection pool...");
  LOG_DEBUG("Pool: min=4, max=32, host=localhost:5432");
  LOG_INFO("Database pool ready: 4 connections established");

  w << "!i Database pool ready — 4 connections";
  w << "";

  LOG_WARN("TLS certificate expires in 14 days — renew soon");
  LOG_INFO("Starting HTTP listener on 0.0.0.0:8080");
  LOG_INFO("Worker threads: 8 (matching CPU cores)");

  w << "!! TLS certificate expires in 14 days";
  w << "!i HTTP listener started on 0.0.0.0:8080";
  w << "";

  // ── Level filtering ───────────────────────────────────────────────
  w << "## Log Level Filtering";
  w << "Set min level to WARN — only WARN, ERROR, FATAL pass through:";
  w << "";

  console.set_min_level(libcpp::log::LWARN);
  LOG_TRACE("This TRACE is filtered out");
  LOG_DEBUG("This DEBUG is filtered out");
  LOG_INFO("This INFO is filtered out");
  LOG_WARN("This WARNING gets through");
  LOG_ERROR("This ERROR gets through");

  w << "!t TRACE — filtered (below threshold)";
  w << "!t DEBUG — filtered (below threshold)";
  w << "!i INFO — filtered (below threshold)";
  w << "!! WARN — passes through";
  w << "!x ERROR — passes through";
  w << "";

  /* Reset level for remaining demo */
  console.set_min_level(libcpp::log::LTRACE);

  // ── Log level reference ───────────────────────────────────────────
  w << "## Log Level Reference";

  w << "| Level | Value | Macro | Use Case |";
  w << "| :--- | :---: | :--- | :--- |";
  w << "| TRACE | 0 | LOG_TRACE() | Fine-grained debug info |";
  w << "| DEBUG | 1 | LOG_DEBUG() | Diagnostic messages |";
  w << "| INFO | 2 | LOG_INFO() | Normal operational events |";
  w << "| WARN | 3 | LOG_WARN() | Potential issues |";
  w << "| ERROR | 4 | LOG_ERROR() | Failures, recoverable |";
  w << "| FATAL | 5 | LOG_FATAL() | Unrecoverable, abort |";
  w << "";

  // ── NullLogger ────────────────────────────────────────────────────
  w << "## NullLogger — The Silent Sink";
  w << "> Useful for benchmarks or production hot-paths where logging";
  w << "> overhead must be zero. All messages are simply discarded.";
  w << "";

  libcpp::log::NullLogger null_log;
  null_log.log(libcpp::log::LFATAL, "This message goes nowhere");
  w << "!v NullLogger accepted and discarded a FATAL message";
  w << "";

  // ── Lifecycle macros ──────────────────────────────────────────────
  w << "## Lifecycle Macros";
  w << "`LOG_CTOR(name)` and `LOG_DTOR(name)` track object lifetime:";
  w << "";
  w << "```";
  w << "MyClass::MyClass()  { LOG_CTOR(\"MyClass\"); }";
  w << "MyClass::~MyClass() { LOG_DTOR(\"MyClass\"); }";
  w << "```";
  w << "";

  LOG_CTOR("DemoServer");
  w << "!t DemoServer constructed";
  LOG_DTOR("DemoServer");
  w << "!t DemoServer destroyed";
  w << "";

  // ── Summary callout ───────────────────────────────────────────────
  w << ">![tip] Log Module Summary";
  w << "> `ILogger` — abstract interface for pluggable log sinks";
  w << "> `ConsoleLogger` — stderr with level filtering";
  w << "> `FileLogger` — appends to disk file";
  w << "> `TimestampDecorator` — prepends ISO timestamp";
  w << "> `LogColorDecorator` — ANSI color per level";
  w << "> `LOG_*()` macros — stream-style convenience";
  w << "";

  w << "---";
  w << "!v Boot sequence complete — all subsystems operational.";

  /* Clean up: unset global so it doesn't point to stack objects */
  libcpp::log::set_global(static_cast<libcpp::log::ILogger*>(0));
  return 0;
}
