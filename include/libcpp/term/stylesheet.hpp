/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stylesheet.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/22 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_TERM_STYLESHEET_HPP
#define LIBCPP_TERM_STYLESHEET_HPP

/*
** StyleSheet -- CSS-like fluent configuration for TermStyle
**
** Usage(reads like a config file, compiles with zero overhead):
**
**   StyleSheet ss;
**   ss["h1"].fg("#FF6B9D").bg("#1A0A1F").bold().center().sep("═").pad(3, 3, 1);
**   ss["h2"].fg("#82CFFF").bold().glyph("▸ ").sep("━");
**   ss["h3"].fg("#82DCA0").bold().glyph("● ");
**   ss["body"].fg("#D0D0DA");
**   ss["quote"].fg("#A0AAB4").italic().glyph("│ ").margin_left(3);
**   ss["info"].fg("#50C8FF").glyph("ℹ  ");
**   ss.width(72);
**   ss.apply(my_style);
**
** Or using macro shortcuts for a header-file "theme":
**
**   LIBCPP_THEME_BEGIN(my_theme)
**     RULE("h1",   fg("#FF6B9D").bg("#1A0A1F").bold().center().sep("═"))
**     RULE("h2",   fg("#82CFFF").bold().glyph("▸ ").sep("━"))
**     RULE("h3",   fg("#82DCA0").bold().glyph("● "))
**     RULE("body", fg("#D0D0DA"))
**     RULE("info", fg("#50C8FF").glyph("ℹ  "))
**     CALLOUT_RULE("tip",
* fg("#A0E0A0").bg("#0C1E12").border("#3CC864").glyph("✔ "))
**     CALLOUT_RULE("note",
* fg("#AAC8F0").bg("#0C1223").border("#4682E6").glyph("ℹ  "))
**     WIDTH(72)
**   LIBCPP_THEME_END
**
** Supported selectors(element names):
**   h1, h2, h3, body, quote, info, warn, error,
**   success, danger, trace, sep, section, callout,
**   bullet, ol, bold, italic, underline, strike, dim, code
**
** Named callout selectors(via CALLOUT_RULE or ss["callout.<name>"]):
**   callout.tip, callout.note, callout.warning, callout.danger,
**   callout.important, callout.success, callout.error, callout.info
**
** Each rule is a builder(Rule) with fluent setters.
** StyleSheet::apply() maps rules to ElemStyle members.
**
** Orthodox Canonical Form compliant.
*/

#include <string>
#include "libcpp/term/style.hpp"
#include "libcpp/term/table.hpp"

namespace libcpp {

/* forward */
class TermWriter;

/* ======================================================================
 *  TableRule -- fluent builder for TableStyle(table-specific properties)
 * ====================================================================== */
class TableRule {
 public:
  TableRule();
  TableRule(const TableRule& other);
  TableRule& operator=(const TableRule& other);
  ~TableRule();

  /* border */
  TableRule& border_preset(BorderPreset bp);
  TableRule& border_color(const std::string& hex);
  TableRule& border_color(const Srgb& c);

  /* header */
  TableRule& header_fg(const std::string& hex);
  TableRule& header_fg(const Srgb& c);
  TableRule& header_bg(const std::string& hex);
  TableRule& header_bg(const Srgb& c);
  TableRule& header_font(int f);
  TableRule& header_has_bg(bool v);
  TableRule& show_header(bool v);
  TableRule& header_separator(bool v);

  /* cells */
  TableRule& cell_fg(const std::string& hex);
  TableRule& cell_fg(const Srgb& c);
  TableRule& cell_bg(const std::string& hex);
  TableRule& cell_bg(const Srgb& c);
  TableRule& cell_alt_bg(const std::string& hex);
  TableRule& cell_alt_bg(const Srgb& c);
  TableRule& zebra(bool v);
  TableRule& cell_has_bg(bool v);

  /* title& footer */
  TableRule& title_fg(const std::string& hex);
  TableRule& title_fg(const Srgb& c);
  TableRule& title_bg(const std::string& hex);
  TableRule& title_bg(const Srgb& c);
  TableRule& title_font(int f);
  TableRule& title_has_bg(bool v);
  TableRule& footer_fg(const std::string& hex);
  TableRule& footer_fg(const Srgb& c);
  TableRule& footer_font(int f);

  /* layout */
  TableRule& pad(int p);
  TableRule& outer_margin(int m);
  TableRule& max_col_width(int w);
  TableRule& show_row_numbers(bool v);
  TableRule& row_separator(bool v);
  TableRule& compact(bool v);

  /* focus style */
  TableRule& focus_fg(const std::string& hex);
  TableRule& focus_fg(const Srgb& c);
  TableRule& focus_bg(const std::string& hex);
  TableRule& focus_bg(const Srgb& c);
  TableRule& focus_font(int f);

  /* presets */
  TableRule& preset_minimal();
  TableRule& preset_modern();
  TableRule& preset_heavy();
  TableRule& preset_matrix();
  TableRule& preset_postman();

  /* apply declared properties to a TableStyle */
  void apply_to(TableStyle& ts) const;

  /* check if any property was set */
  bool has_values() const;

 private:
  enum TblFlag {
    T_BORDER_PRESET = 1u << 0,
    T_BORDER_COLOR = 1u << 1,
    T_HEADER_FG = 1u << 2,
    T_HEADER_BG = 1u << 3,
    T_HEADER_FONT = 1u << 4,
    T_HEADER_HAS_BG = 1u << 5,
    T_SHOW_HEADER = 1u << 6,
    T_HEADER_SEP = 1u << 7,
    T_CELL_FG = 1u << 8,
    T_CELL_BG = 1u << 9,
    T_CELL_ALT_BG = 1u << 10,
    T_ZEBRA = 1u << 11,
    T_CELL_HAS_BG = 1u << 12,
    T_TITLE_FG = 1u << 13,
    T_TITLE_BG = 1u << 14,
    T_TITLE_FONT = 1u << 15,
    T_TITLE_HAS_BG = 1u << 16,
    T_FOOTER_FG = 1u << 17,
    T_FOOTER_FONT = 1u << 18,
    T_PAD = 1u << 19,
    T_OUTER_MARGIN = 1u << 20,
    T_MAX_COL_WIDTH = 1u << 21,
    T_SHOW_ROW_NUMS = 1u << 22,
    T_ROW_SEPARATOR = 1u << 23,
    T_COMPACT = 1u << 24,
    T_FOCUS_FG = 1u << 25,
    T_FOCUS_BG = 1u << 26,
    T_FOCUS_FONT = 1u << 27,
    T_PRESET = 1u << 28
  };

  unsigned int _set;
  TableStyle _base; /* when T_PRESET is set, start from this */
  BorderPreset _border;
  Srgb _border_color;
  Srgb _header_fg;
  Srgb _header_bg;
  int _header_font;
  bool _header_has_bg;
  bool _show_header;
  bool _header_sep;
  Srgb _cell_fg;
  Srgb _cell_bg;
  Srgb _cell_alt_bg;
  bool _zebra;
  bool _cell_has_bg;
  Srgb _title_fg;
  Srgb _title_bg;
  int _title_font;
  bool _title_has_bg;
  Srgb _footer_fg;
  int _footer_font;
  int _pad;
  int _outer_margin;
  int _max_col_width;
  bool _show_row_nums;
  bool _row_separator;
  bool _compact;
  Srgb _focus_fg;
  Srgb _focus_bg;
  int _focus_font;
};

/* ======================================================================
 *  Rule -- one CSS-like property block(fluent builder)
 * ====================================================================== */
class Rule {
 public:
  Rule();
  Rule(const Rule& other);
  Rule& operator=(const Rule& other);
  ~Rule();

  /* color properties — accept hex "#RRGGBB" or Srgb */
  Rule& fg(const std::string& hex);
  Rule& fg(const Srgb& c);
  Rule& bg(const std::string& hex);
  Rule& bg(const Srgb& c);
  Rule& border(const std::string& hex);
  Rule& border(const Srgb& c);

  /* font shortcuts */
  Rule& bold();
  Rule& italic();
  Rule& dim();
  Rule& underline();
  Rule& strike();
  Rule& blink();
  Rule& reverse();
  Rule& font(int flags);

  /* alignment */
  Rule& left();
  Rule& center();
  Rule& right();
  Rule& align(Align a);

  /* dimensions */
  Rule& width(int w);
  Rule& pad(int left, int right);
  Rule& pad(int left, int right, int vert);
  Rule& pad_left(int n);
  Rule& pad_right(int n);
  Rule& pad_vert(int n);
  Rule& margin_left(int n);
  Rule& spacing(int before, int after);
  Rule& space_before(int n);
  Rule& space_after(int n);

  /* decoration */
  Rule& glyph(const std::string& g);
  Rule& body_glyph(const std::string& g);
  Rule& sep(const std::string& s);
  Rule& has_bg(bool v);

  /* apply this rule's properties onto an ElemStyle */
  void apply_to(ElemStyle& es) const;

 private:
  /* each property has a "set" flag so we only override what was declared */
  enum PropFlag {
    P_FG = 1 << 0,
    P_BG = 1 << 1,
    P_BORDER = 1 << 2,
    P_FONT = 1 << 3,
    P_ALIGN = 1 << 4,
    P_WIDTH = 1 << 5,
    P_PAD_L = 1 << 6,
    P_PAD_R = 1 << 7,
    P_PAD_V = 1 << 8,
    P_MARGIN_L = 1 << 9,
    P_SPACE_B = 1 << 10,
    P_SPACE_A = 1 << 11,
    P_HAS_BG = 1 << 12,
    P_GLYPH = 1 << 13,
    P_BODY_GLYPH = 1 << 14,
    P_SEP = 1 << 15
  };

  unsigned int _set; /* bitmask of which props are declared */
  Srgb _fg;
  Srgb _bg;
  Srgb _border;
  int _font;
  Align _align;
  int _width;
  int _pad_l;
  int _pad_r;
  int _pad_v;
  int _margin_l;
  int _space_before;
  int _space_after;
  bool _has_bg;
  std::string _glyph;
  std::string _body_glyph;
  std::string _sep;
};

/* ======================================================================
 *  StyleSheet -- collection of named rules, applies to TermStyle
 * ====================================================================== */
class StyleSheet {
 public:
  static const int MAX_RULES = 48;

  StyleSheet();
  StyleSheet(const StyleSheet& other);
  StyleSheet& operator=(const StyleSheet& other);
  ~StyleSheet();

  /* CSS-like selector access: ss["h1"].fg("#FF6B9D").bold(); */
  Rule& operator[](const std::string& selector);

  /* table-specific rule: ss.table().border_preset(BORDER_ROUNDED).zebra(true);
   */
  TableRule& table();
  const TableRule& table() const;

  /* global width override(applied to all elements) */
  StyleSheet& width(int w);

  /* apply all rules to a TermStyle */
  void apply(TermStyle& ts) const;

  /* apply all rules + table style + callouts to TermWriter */
  void apply(TermStyle& ts, TermWriter& w) const;

  /* apply table style only */
  void apply_table(TermWriter& w) const;

  /* apply callout overrides to TermWriter */
  void apply_callouts(TermWriter& w) const;

  /* presets — returns a pre-configured StyleSheet */
  static StyleSheet dracula();
  static StyleSheet nord();
  static StyleSheet monokai();
  static StyleSheet solarized();

 private:
  struct Entry {
    std::string name;
    Rule rule;
    bool used;
    Entry() : used(false) {}
  };

  Entry _rules[MAX_RULES];
  int _rule_count;
  int _global_width;
  bool _has_global_width;
  TableRule _table_rule;

  const Rule* _find(const std::string& name) const;
  void _apply_rule(const std::string& name, ElemStyle& es) const;
};

/* ======================================================================
 *  Theme macros — header-file "config" syntax
 *
 *  These macros let you define a theme as a standalone function
 *  that creates and returns a StyleSheet. Example:
 *
 *    LIBCPP_THEME_BEGIN(my_dark_theme)
 *      RULE("h1",   fg("#FF6B9D").bg("#1A0A1F").bold().center().sep("═"))
 *      RULE("h2",   fg("#82CFFF").bold().glyph("▸ ").sep("━"))
 *      RULE("body", fg("#D0D0DA"))
 *      WIDTH(72)
 *    LIBCPP_THEME_END
 *
 *  Then use:  my_dark_theme().apply(ts);
 *
 * ====================================================================== */

#define LIBCPP_THEME_BEGIN(name)     \
  inline libcpp::StyleSheet name() { \
    libcpp::StyleSheet _ss;

#define RULE(sel, props) _ss[sel].props;

#define CALLOUT_RULE(name, props) _ss["callout." name].props;

#define TABLE_RULE(props) _ss.table().props;

#define WIDTH(w) _ss.width(w);

#define LIBCPP_THEME_END \
  return _ss;            \
  }

} /* namespace libcpp */

#endif /* LIBCPP_TERM_STYLESHEET_HPP */
