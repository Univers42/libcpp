/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   demo_data.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 16:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/22 16:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** demo_data.cpp — Employee payroll management system
**
** Scenario: Build an in-memory employee database, run analytics (salary
** averages, department breakdowns), export to CSV, and generate a
** quarterly payroll report — all using data/database, data/csv, data/date.
**
** Theme: Deep Ocean (cool teal-cyan on dark navy)
*/

#include <cstdlib>
#include <iomanip>
#include <sstream>

#include "libcpp/data/csv.hpp"
#include "libcpp/data/database.hpp"
#include "libcpp/data/date.hpp"
#include "libcpp/str/format.hpp"
#include "libcpp/term/stylesheet.hpp"
#include "libcpp/term/writer.hpp"

using libcpp::Srgb;
using libcpp::StyleSheet;
using libcpp::TermStyle;
using libcpp::TermWriter;

/* ── Deep Ocean theme ────────────────────────────────────────────────── */

LIBCPP_THEME_BEGIN(deep_ocean)
  RULE("h1", fg("#00E5FF").bg("#001F33").border("#00E5FF")
                 .bold().center().pad(3, 3, 1).spacing(1, 1)
                 .glyph("\xe2\x97\x86 ").sep("\xe2\x95\x90"))
  RULE("h2", fg("#0097A7").border("#003344")
                 .bold().pad(1, 1).spacing(1, 0)
                 .glyph("\xe2\x96\xb8 ").sep("\xe2\x94\x81"))
  RULE("h3", fg("#26C6DA").border("#003344")
                 .bold().pad(1, 1).spacing(1, 0)
                 .glyph("\xe2\x97\x8f ").sep("\xe2\x95\x8c"))
  RULE("body",    fg("#B0C4DE").pad_left(2))
  RULE("quote",   fg("#607080").italic().glyph("\xe2\x94\x82 ").margin_left(3))
  RULE("info",    fg("#00E5FF").glyph("\xe2\x84\xb9  "))
  RULE("warn",    fg("#FFC107").bold().glyph("\xe2\x9a\xa0  "))
  RULE("error",   fg("#FF5252").bold().glyph("\xe2\x9c\x97 "))
  RULE("success", fg("#69F0AE").glyph("\xe2\x9c\x94 "))
  RULE("sep",     border("#003344").sep("\xe2\x94\x80"))
  RULE("bullet",  fg("#B0C4DE").glyph("\xe2\x97\x86 ").pad_left(4))
  RULE("ol",      fg("#B0C4DE").pad_left(4))
  CALLOUT_RULE("tip",  fg("#B2F5EA").bg("#002020").border("#00BFA5")
                           .glyph("\xe2\x9c\x94 "))
  CALLOUT_RULE("note", fg("#80D8FF").bg("#001A30").border("#0091EA")
                           .glyph("\xe2\x84\xb9  "))
  TABLE_RULE(border_preset(libcpp::BORDER_ROUNDED).border_color("#0097A7")
                 .header_fg("#00E5FF").header_bg("#001F33")
                 .header_font(libcpp::FONT_BOLD).header_has_bg(true)
                 .cell_fg("#B0C4DE").cell_bg("#001F33").cell_alt_bg("#002A40")
                 .zebra(true).cell_has_bg(true)
                 .title_fg("#00E5FF").title_bg("#001F33").title_has_bg(true)
                 .footer_fg("#607080"))
  WIDTH(76)
LIBCPP_THEME_END

int main() {
  TermStyle ts;
  deep_ocean().apply(ts);
  TermWriter w(ts);

  w << "# Payroll Management System";
  w << "";
  w << "Quarterly payroll report for Q1-2026 — in-memory database demo";
  w << "using `libcpp::data` (Database, CsvDocument, Date).";

  // ── Build the employee database ───────────────────────────────────
  w << "## Employee Database";

  libcpp::data::Database db;
  db.add_column("id",         libcpp::data::ColType::INTEGER);
  db.add_column("name",       libcpp::data::ColType::STRING);
  db.add_column("department", libcpp::data::ColType::STRING);
  db.add_column("salary",     libcpp::data::ColType::DOUBLE);
  db.add_column("hire_date",  libcpp::data::ColType::DATE);

  struct Emp {
    int id; const char* name; const char* dept;
    const char* salary; const char* hire;
  };
  Emp employees[] = {
      {1,  "Alice Martin",  "Engineering", "85000",  "2022-03-15"},
      {2,  "Bob Nguyen",    "Engineering", "92000",  "2021-07-01"},
      {3,  "Clara Santos",  "Design",      "78000",  "2023-01-10"},
      {4,  "David Kim",     "Engineering", "105000", "2019-11-20"},
      {5,  "Eva Petrov",    "Marketing",   "72000",  "2024-02-28"},
      {6,  "Frank Weber",   "Design",      "81000",  "2022-09-05"},
      {7,  "Grace Liu",     "Marketing",   "68000",  "2024-06-15"},
      {8,  "Hugo Morales",  "Engineering", "98000",  "2020-04-12"},
      {9,  "Isla McLeod",   "HR",          "74000",  "2023-08-22"},
      {10, "Jules Bernard", "HR",          "71000",  "2024-01-03"},
  };

  for (int i = 0; i < 10; ++i) {
    std::map<std::string, std::string> row;
    row["id"]         = libcpp::str::to_string(employees[i].id);
    row["name"]       = employees[i].name;
    row["department"] = employees[i].dept;
    row["salary"]     = employees[i].salary;
    row["hire_date"]  = employees[i].hire;
    db.add_row(row);
  }

  /* Display full roster via markdown table */
  w << "| ID | Name | Department | Salary | Hire Date |";
  w << "| ---: | :--- | :--- | ---: | :---: |";
  for (std::size_t i = 0; i < db.count(); ++i) {
    const auto& r = db.table().rows()[i];
    w << ("| " + r.get_value("id")
        + " | " + r.get_value("name")
        + " | " + r.get_value("department")
        + " | $" + r.get_value("salary")
        + " | " + r.get_value("hire_date") + " |");
  }
  w << "";

  // ── Department analytics ──────────────────────────────────────────
  w << "## Department Analytics";

  std::string depts[] = {"Engineering", "Design", "Marketing", "HR"};
  w << "| Department | Headcount | Avg Salary | Total Cost |";
  w << "| :--- | :---: | ---: | ---: |";
  for (int d = 0; d < 4; ++d) {
    auto rows = db.where("department", depts[d]);
    double total = 0;
    for (std::size_t r = 0; r < rows.size(); ++r)
      total += std::atof(rows[r].get_value("salary").c_str());
    double avg = rows.empty() ? 0 : total / static_cast<double>(rows.size());
    w << ("| " + depts[d]
        + " | " + libcpp::str::to_string(static_cast<int>(rows.size()))
        + " | $" + libcpp::str::to_string(static_cast<int>(avg))
        + " | $" + libcpp::str::to_string(static_cast<int>(total)) + " |");
  }
  w << "";

  // ── Date arithmetic — tenure calculation ──────────────────────────
  w << "## Employee Tenure";

  libcpp::data::Date today = libcpp::data::Date::today();
  w << ("!i Report date: " + today.to_string("DD MMM YYYY")
      + " (" + today.weekday_name() + ")");
  w << "";

  w << "| Name | Hire Date | Days | Years |";
  w << "| :--- | :---: | ---: | ---: |";
  for (std::size_t i = 0; i < db.count(); ++i) {
    const auto& r = db.table().rows()[i];
    libcpp::data::Date hire =
        libcpp::data::Date::parse(r.get_value("hire_date"));
    int days = today.days_between(hire);
    if (days < 0) days = -days;
    double years = static_cast<double>(days) / 365.25;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << years;
    w << ("| " + r.get_value("name")
        + " | " + hire.to_string("DD MMM YYYY")
        + " | " + libcpp::str::to_string(days)
        + " | " + oss.str() + " |");
  }
  w << "";

  // ── CSV export preview ────────────────────────────────────────────
  w << "## CSV Export Preview";

  std::string csv_str = libcpp::data::export_markdown(db.table());
  w << "Database exported to Markdown table format:";
  w << "> (First 3 lines of the export)";
  {
    std::string lines[8];
    int n = libcpp::str::split(csv_str, '\n', lines, 8);
    for (int i = 0; i < n && i < 4; ++i)
      w << ("`" + lines[i] + "`");
  }
  w << "";

  // ── Sort demo ─────────────────────────────────────────────────────
  w << "## Top Earners";

  db.sort("salary", false);
  w << "Sorted by salary DESC — top 3:";
  w << "";
  w << "| Rank | Name | Salary |";
  w << "| :---: | :--- | ---: |";
  for (std::size_t i = 0; i < 3 && i < db.count(); ++i) {
    const auto& r = db.table().rows()[i];
    w << ("| " + libcpp::str::to_string(static_cast<int>(i + 1))
        + " | " + r.get_value("name")
        + " | $" + r.get_value("salary") + " |");
  }
  w << "";

  // ── Callout summary ───────────────────────────────────────────────
  w << ">![tip] Data Module Summary";
  w << "> `Database` — in-memory table with add/remove/where/sort/export";
  w << "> `CsvDocument` — RFC-4180 parser, load/save, aggregate helpers";
  w << "> `Date` — calendar arithmetic, formatting, epoch conversion";
  w << "> All with OCF (Orthodox Canonical Form) compliance.";
  w << "";

  w << "---";
  w << "!v Payroll report complete — 10 employees processed.";
  return 0;
}
