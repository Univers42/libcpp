/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   demo_tables.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 15:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/22 15:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** demo_tables.cpp — Exhaustive Table Customization Showcase
**
** Demonstrates ALL table features:
**   1. Default light border
**   2. Rounded modern with zebra stripes + header bg
**   3. Heavy borders with row separators
**   4. Double-line academic/matrix style
**   5. ASCII fallback
**   6. No-border minimal
**   7. Title bar + footer bar
**   8. Row highlighting (focus rows)
**   9. Column alignment (left/center/right)
**  10. Postman-style test report
**  11. Attribute-list (single column, no header)
**  12. Wide 6-column data matrix
*/

#include "libcpp/term/writer.hpp"
#include "libcpp/term/stylesheet.hpp"

using namespace libcpp;

int main()
{
	TermStyle ts;
	StyleSheet::dracula().apply(ts);
	TermWriter w(ts);

	w.h1("Table Customization Showcase");
	w.nl();
	w.text("Every table style preset and configuration option demonstrated.");
	w.nl();

	/* ── 1. Default (light border) ─────────────────────────────────── */
	w.h2("1. Default Light Borders");
	{
		Table t(3);
		t.header3("Language", "Paradigm", "Year");
		t.row3("C++",    "Multi-paradigm",  "1985");
		t.row3("Rust",   "Systems",         "2010");
		t.row3("Python", "Dynamic",         "1991");
		t.row3("Haskell","Functional",       "1990");
		w.table(t);
	}
	w.nl();

	/* ── 2. Rounded Modern with Zebra ──────────────────────────────── */
	w.h2("2. Rounded + Zebra Stripes + Header BG");
	{
		Table t(4);
		TableStyle s = TableStyle::modern();
		t.set_style(s);
		t.set_title("Server Fleet Status");
		t.header4("Host", "CPU%", "Memory", "Status");
		t.row4("web-01",    "23%",  "4.2 GB / 8 GB",   "healthy");
		t.row4("web-02",    "67%",  "6.1 GB / 8 GB",   "warning");
		t.row4("api-01",    "12%",  "2.0 GB / 16 GB",  "healthy");
		t.row4("db-master", "89%",  "14.8 GB / 16 GB", "critical");
		t.row4("db-replica","34%",  "8.3 GB / 16 GB",  "healthy");
		t.row4("cache-01",  "5%",   "1.2 GB / 4 GB",   "healthy");
		t.set_footer("6 hosts monitored | last check: 12:34:56");
		w.table(t);
	}
	w.nl();

	/* ── 3. Heavy with Row Separators ──────────────────────────────── */
	w.h2("3. Heavy Borders + Row Separators");
	{
		Table t(3);
		TableStyle s = TableStyle::heavy_style();
		t.set_style(s);
		t.header3("Module", "Tests", "Coverage");
		t.row3("term::style",     "8 / 8 pass",    "94%");
		t.row3("term::writer",    "12 / 12 pass",   "87%");
		t.row3("term::table",     "6 / 6 pass",     "91%");
		t.row3("async::worker",   "4 / 4 pass",     "78%");
		t.row3("net::server",     "6 / 6 pass",     "82%");
		w.table(t);
	}
	w.nl();

	/* ── 4. Double-line Academic ───────────────────────────────────── */
	w.h2("4. Double-Line Borders (Academic)");
	{
		Table t(4);
		TableStyle s;
		s.border = BORDER_DOUBLE;
		s.border_color = Srgb(100, 140, 200);
		s.header_fg = Srgb(180, 220, 255);
		s.header_font = FONT_BOLD;
		s.cell_fg = Srgb(200, 200, 210);
		t.set_style(s);
		t.set_title("Complexity Analysis");
		t.header4("Algorithm", "Best", "Average", "Worst");
		t.row4("Binary Search",   "O(1)",      "O(log n)",  "O(log n)");
		t.row4("Quick Sort",      "O(n log n)","O(n log n)","O(n\xc2\xb2)");
		t.row4("Merge Sort",      "O(n log n)","O(n log n)","O(n log n)");
		t.row4("Hash Lookup",     "O(1)",      "O(1)",      "O(n)");
		t.row4("Heap Sort",       "O(n log n)","O(n log n)","O(n log n)");
		t.set_col_align(1, ALIGN_CENTER);
		t.set_col_align(2, ALIGN_CENTER);
		t.set_col_align(3, ALIGN_CENTER);
		w.table(t);
	}
	w.nl();

	/* ── 5. ASCII Fallback ─────────────────────────────────────────── */
	w.h2("5. ASCII Fallback (for legacy terminals)");
	{
		Table t(3);
		TableStyle s;
		s.border = BORDER_ASCII;
		s.border_color = Srgb(150, 150, 150);
		s.header_fg = Srgb(255, 255, 255);
		s.cell_fg = Srgb(180, 180, 180);
		t.set_style(s);
		t.header3("PID", "Command", "CPU%");
		t.row3("1234", "/usr/bin/nginx", "2.3");
		t.row3("5678", "/usr/bin/node",  "15.7");
		t.row3("9012", "/usr/sbin/sshd", "0.1");
		w.table(t);
	}
	w.nl();

	/* ── 6. Minimal (no borders) ───────────────────────────────────── */
	w.h2("6. Minimal / No Borders");
	{
		Table t(3);
		TableStyle s = TableStyle::minimal();
		s.header_fg = Srgb(130, 200, 255);
		s.cell_fg = Srgb(190, 190, 200);
		t.set_style(s);
		t.header3("Key", "Type", "Default");
		t.row3("border",       "BorderPreset", "BORDER_LIGHT");
		t.row3("border_color", "Srgb",         "(100,100,110)");
		t.row3("header_fg",    "Srgb",         "(255,255,255)");
		t.row3("zebra",        "bool",         "false");
		t.row3("pad",          "int",          "1");
		w.table(t);
	}
	w.nl();

	/* ── 7. Title + Footer Bar ─────────────────────────────────────── */
	w.h2("7. Title Bar + Footer Bar");
	{
		Table t(3);
		TableStyle s;
		s.border = BORDER_ROUNDED;
		s.border_color = Srgb(80, 180, 130);
		s.header_fg = Srgb(100, 255, 180);
		s.header_bg = Srgb(10, 35, 25);
		s.header_has_bg = true;
		s.header_font = FONT_BOLD;
		s.cell_fg = Srgb(200, 220, 210);
		s.title_fg = Srgb(100, 255, 180);
		s.title_bg = Srgb(10, 35, 25);
		s.title_has_bg = true;
		s.title_font = FONT_BOLD;
		s.footer_fg = Srgb(80, 140, 100);
		s.footer_font = FONT_DIM;
		t.set_style(s);
		t.set_title("Environment Variables");
		t.header3("Variable", "Value", "Source");
		t.row3("CXX",      "c++",             "Makefile");
		t.row3("CXXFLAGS", "-std=c++17 -Wall", "Makefile");
		t.row3("PATH",     "/usr/local/bin",   "shell");
		t.row3("HOME",     "/home/dlesieur",   "system");
		t.set_footer("4 variables | exported at build time");
		w.table(t);
	}
	w.nl();

	/* ── 8. Row Highlighting ───────────────────────────────────────── */
	w.h2("8. Row Highlighting / Focus");
	{
		Table t(4);
		TableStyle s;
		s.border = BORDER_HEAVY;
		s.border_color = Srgb(120, 80, 180);
		s.header_fg = Srgb(200, 170, 255);
		s.header_font = FONT_BOLD;
		s.cell_fg = Srgb(190, 190, 200);
		s.focus_style.fg = Srgb(255, 100, 100);
		s.focus_style.has_fg = true;
		s.focus_style.bg = Srgb(50, 15, 15);
		s.focus_style.has_bg = true;
		s.focus_style.font = FONT_BOLD;
		s.focus_style.has_font = true;
		t.set_style(s);
		t.set_title("Build Errors");
		t.header4("File", "Line", "Severity", "Message");
		t.row4("style.cpp",  "142", "warning", "unused variable 'tmp'");
		t.row4("table.cpp",  "87",  "error",   "no matching function call");
		t.highlight_last_row();
		t.row4("writer.cpp", "203", "warning", "implicit conversion");
		t.row4("table.cpp",  "91",  "error",   "expected ';' before '}'");
		t.highlight_last_row();
		t.row4("color.cpp",  "55",  "warning", "comparison signed/unsigned");
		w.table(t);
	}
	w.nl();

	/* ── 9. Column Alignment ───────────────────────────────────────── */
	w.h2("9. Column Alignment (Left / Center / Right)");
	{
		Table t(4);
		TableStyle s;
		s.border = BORDER_ROUNDED;
		s.border_color = Srgb(100, 130, 180);
		s.header_fg = Srgb(180, 210, 255);
		s.cell_fg = Srgb(200, 205, 215);
		s.header_font = FONT_BOLD;
		t.set_style(s);
		t.header4("Item", "Qty", "Unit Price", "Total");
		t.set_col_align(0, ALIGN_LEFT);
		t.set_col_align(1, ALIGN_CENTER);
		t.set_col_align(2, ALIGN_RIGHT);
		t.set_col_align(3, ALIGN_RIGHT);
		t.row4("libcpp.a",        "1",   "$0.00",   "$0.00");
		t.row4("Coffee",          "47",  "$3.50",   "$164.50");
		t.row4("Debugging Hours", "128", "$95.00",  "$12,160.00");
		t.row4("Sanity",          "0",   "N/A",     "Priceless");
		t.set_footer("Total: $12,324.50 (coffee is the real cost)");
		w.table(t);
	}
	w.nl();

	/* ── 10. Postman-Style Test Report ─────────────────────────────── */
	w.h2("10. Postman-Style Report");
	{
		Table t(4);
		TableStyle s = TableStyle::postman();
		t.set_style(s);
		t.set_title("TEST REPORT  --  36 assertions  36 passed  0 failed");
		t.header4("#", "Suite", "Assertion", "Result");
		t.set_col_align(0, ALIGN_RIGHT);
		t.set_col_align(3, ALIGN_CENTER);
		t.row4("1",  "TermStyle", "h1() returns non-empty",      "PASS");
		t.row4("2",  "TermStyle", "h2() includes border color",  "PASS");
		t.row4("3",  "Table",     "render() correct col widths", "PASS");
		t.row4("4",  "Table",     "zebra alternates bg color",   "PASS");
		t.row4("5",  "Writer",    "callout uniform bg width",    "PASS");
		t.row4("6",  "Writer",    "parse() handles all syntax",  "PASS");
		t.highlight_row(0);
		t.highlight_row(2);
		t.highlight_row(4);
		t.set_footer("All 36 assertions passed.");
		w.table(t);
	}
	w.nl();

	/* ── 11. Attribute List (single column) ────────────────────────── */
	w.h2("11. Attribute List (Key-Value Pairs)");
	{
		Table t(2);
		TableStyle s;
		s.border = BORDER_ROUNDED;
		s.border_color = Srgb(100, 160, 140);
		s.show_header = false;
		s.cell_fg = Srgb(200, 210, 200);
		s.zebra = true;
		s.cell_bg = Srgb(15, 22, 18);
		s.cell_alt_bg = Srgb(22, 30, 25);
		s.cell_has_bg = true;
		t.set_style(s);
		t.set_title("Configuration");
		t.set_cols(2);
		std::string r1[] = {"compiler",    "c++ (clang 17.0)"};
		std::string r2[] = {"standard",    "C++17"};
		std::string r3[] = {"flags",       "-Wall -Wextra -Werror"};
		std::string r4[] = {"objects",     "39"};
		std::string r5[] = {"tests",       "36 (all passing)"};
		std::string r6[] = {"demos",       "8"};
		std::string r7[] = {"archive",     "libcpp.a (+ libftpp.a alias)"};
		t.row(r1, 2); t.row(r2, 2); t.row(r3, 2);
		t.row(r4, 2); t.row(r5, 2); t.row(r6, 2); t.row(r7, 2);
		w.table(t);
	}
	w.nl();

	/* ── 12. Wide 6-Column Matrix ──────────────────────────────────── */
	w.h2("12. Wide Data Matrix (6 Columns)");
	{
		Table t(6);
		TableStyle s;
		s.border = BORDER_HEAVY;
		s.border_color = Srgb(80, 100, 160);
		s.header_fg = Srgb(160, 200, 255);
		s.header_bg = Srgb(20, 25, 45);
		s.header_has_bg = true;
		s.header_font = FONT_BOLD;
		s.cell_fg = Srgb(190, 195, 210);
		s.zebra = true;
		s.cell_bg = Srgb(12, 14, 22);
		s.cell_alt_bg = Srgb(18, 22, 35);
		s.cell_has_bg = true;
		s.row_separator = false;
		t.set_style(s);
		t.set_title("Color Reference: Dracula Palette");
		t.header6("Name", "Hex", "R", "G", "B", "Usage");
		t.set_col_align(2, ALIGN_RIGHT);
		t.set_col_align(3, ALIGN_RIGHT);
		t.set_col_align(4, ALIGN_RIGHT);
		t.row6("Background", "#282A36", "40",  "42",  "54",  "editor bg");
		t.row6("Current",    "#44475A", "68",  "71",  "90",  "selection");
		t.row6("Foreground", "#F8F8F2", "248", "248", "242", "text");
		t.row6("Comment",    "#6272A4", "98",  "114", "164", "comments");
		t.row6("Cyan",       "#8BE9FD", "139", "233", "253", "types");
		t.row6("Green",      "#50FA7B", "80",  "250", "123", "strings");
		t.row6("Orange",     "#FFB86C", "255", "184", "108", "constants");
		t.row6("Pink",       "#FF79C6", "255", "121", "198", "keywords");
		t.row6("Purple",     "#BD93F9", "189", "147", "249", "functions");
		t.row6("Red",        "#FF5555", "255", "85",  "85",  "errors");
		t.row6("Yellow",     "#F1FA8C", "241", "250", "140", "warnings");
		t.set_footer("11 colors | Dracula Theme v1.0 | draculatheme.com");
		w.table(t);
	}
	w.nl();

	/* ── 13. Matrix style ──────────────────────────────────────────── */
	w.h2("13. Matrix Style (Green Terminal)");
	{
		Table t(4);
		TableStyle s = TableStyle::matrix();
		t.set_style(s);
		t.header4("PID", "USER", "CMD", "MEM");
		t.row4("1",    "root",    "/sbin/init",     "4.2M");
		t.row4("142",  "www",     "nginx: master",  "12.8M");
		t.row4("143",  "www",     "nginx: worker",  "8.4M");
		t.row4("256",  "dlesieur","vim style.cpp",   "24.1M");
		t.row4("1024", "dlesieur","make compile",    "156.2M");
		w.table(t);
	}
	w.nl();

	w.sep();
	w.text("Table customization showcase complete.");
	w.text("13 different table styles demonstrated.");

	return 0;
}
