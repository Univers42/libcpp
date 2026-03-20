/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stylesheet.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/22 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/term/stylesheet.hpp"
#include "libcpp/term/writer.hpp"

namespace libcpp
{

/* ======================================================================
 *  TableRule -- OCF
 * ====================================================================== */

TableRule::TableRule()
	: _set(0), _border(BORDER_LIGHT), _border_color(100,100,110),
	  _header_fg(255,255,255), _header_bg(40,44,52), _header_font(FONT_BOLD),
	  _header_has_bg(false), _show_header(true), _header_sep(true),
	  _cell_fg(200,200,210), _cell_bg(0,0,0), _cell_alt_bg(22,24,30),
	  _zebra(false), _cell_has_bg(false),
	  _title_fg(255,255,255), _title_bg(40,44,52), _title_font(FONT_BOLD),
	  _title_has_bg(false), _footer_fg(140,140,150), _footer_font(FONT_DIM),
	  _pad(1), _outer_margin(0), _max_col_width(0),
	  _show_row_nums(false), _row_separator(false), _compact(false),
	  _focus_fg(255,255,255), _focus_bg(60,60,80), _focus_font(FONT_BOLD) {}

TableRule::TableRule(const TableRule& o)
	: _set(o._set), _base(o._base), _border(o._border),
	  _border_color(o._border_color), _header_fg(o._header_fg),
	  _header_bg(o._header_bg), _header_font(o._header_font),
	  _header_has_bg(o._header_has_bg), _show_header(o._show_header),
	  _header_sep(o._header_sep), _cell_fg(o._cell_fg), _cell_bg(o._cell_bg),
	  _cell_alt_bg(o._cell_alt_bg), _zebra(o._zebra),
	  _cell_has_bg(o._cell_has_bg), _title_fg(o._title_fg),
	  _title_bg(o._title_bg), _title_font(o._title_font),
	  _title_has_bg(o._title_has_bg), _footer_fg(o._footer_fg),
	  _footer_font(o._footer_font), _pad(o._pad),
	  _outer_margin(o._outer_margin), _max_col_width(o._max_col_width),
	  _show_row_nums(o._show_row_nums), _row_separator(o._row_separator),
	  _compact(o._compact), _focus_fg(o._focus_fg), _focus_bg(o._focus_bg),
	  _focus_font(o._focus_font) {}

TableRule& TableRule::operator=(const TableRule& o)
{
	if (this != &o)
	{
		_set = o._set; _base = o._base; _border = o._border;
		_border_color = o._border_color;
		_header_fg = o._header_fg; _header_bg = o._header_bg;
		_header_font = o._header_font; _header_has_bg = o._header_has_bg;
		_show_header = o._show_header; _header_sep = o._header_sep;
		_cell_fg = o._cell_fg; _cell_bg = o._cell_bg;
		_cell_alt_bg = o._cell_alt_bg; _zebra = o._zebra;
		_cell_has_bg = o._cell_has_bg;
		_title_fg = o._title_fg; _title_bg = o._title_bg;
		_title_font = o._title_font; _title_has_bg = o._title_has_bg;
		_footer_fg = o._footer_fg; _footer_font = o._footer_font;
		_pad = o._pad; _outer_margin = o._outer_margin;
		_max_col_width = o._max_col_width;
		_show_row_nums = o._show_row_nums; _row_separator = o._row_separator;
		_compact = o._compact;
		_focus_fg = o._focus_fg; _focus_bg = o._focus_bg;
		_focus_font = o._focus_font;
	}
	return *this;
}

TableRule::~TableRule() {}

/* ── border ───────────────────────────────────────────────────────────── */
TableRule& TableRule::border_preset(BorderPreset bp) { _border = bp; _set |= T_BORDER_PRESET; return *this; }
TableRule& TableRule::border_color(const std::string& hex) { _border_color = Srgb::from_hex(hex); _set |= T_BORDER_COLOR; return *this; }
TableRule& TableRule::border_color(const Srgb& c) { _border_color = c; _set |= T_BORDER_COLOR; return *this; }

/* ── header ───────────────────────────────────────────────────────────── */
TableRule& TableRule::header_fg(const std::string& hex) { _header_fg = Srgb::from_hex(hex); _set |= T_HEADER_FG; return *this; }
TableRule& TableRule::header_fg(const Srgb& c) { _header_fg = c; _set |= T_HEADER_FG; return *this; }
TableRule& TableRule::header_bg(const std::string& hex) { _header_bg = Srgb::from_hex(hex); _set |= T_HEADER_BG | T_HEADER_HAS_BG; _header_has_bg = true; return *this; }
TableRule& TableRule::header_bg(const Srgb& c) { _header_bg = c; _set |= T_HEADER_BG | T_HEADER_HAS_BG; _header_has_bg = true; return *this; }
TableRule& TableRule::header_font(int f) { _header_font = f; _set |= T_HEADER_FONT; return *this; }
TableRule& TableRule::header_has_bg(bool v) { _header_has_bg = v; _set |= T_HEADER_HAS_BG; return *this; }
TableRule& TableRule::show_header(bool v) { _show_header = v; _set |= T_SHOW_HEADER; return *this; }
TableRule& TableRule::header_separator(bool v) { _header_sep = v; _set |= T_HEADER_SEP; return *this; }

/* ── cells ────────────────────────────────────────────────────────────── */
TableRule& TableRule::cell_fg(const std::string& hex) { _cell_fg = Srgb::from_hex(hex); _set |= T_CELL_FG; return *this; }
TableRule& TableRule::cell_fg(const Srgb& c) { _cell_fg = c; _set |= T_CELL_FG; return *this; }
TableRule& TableRule::cell_bg(const std::string& hex) { _cell_bg = Srgb::from_hex(hex); _set |= T_CELL_BG | T_CELL_HAS_BG; _cell_has_bg = true; return *this; }
TableRule& TableRule::cell_bg(const Srgb& c) { _cell_bg = c; _set |= T_CELL_BG | T_CELL_HAS_BG; _cell_has_bg = true; return *this; }
TableRule& TableRule::cell_alt_bg(const std::string& hex) { _cell_alt_bg = Srgb::from_hex(hex); _set |= T_CELL_ALT_BG; return *this; }
TableRule& TableRule::cell_alt_bg(const Srgb& c) { _cell_alt_bg = c; _set |= T_CELL_ALT_BG; return *this; }
TableRule& TableRule::zebra(bool v) { _zebra = v; _set |= T_ZEBRA; return *this; }
TableRule& TableRule::cell_has_bg(bool v) { _cell_has_bg = v; _set |= T_CELL_HAS_BG; return *this; }

/* ── title & footer ───────────────────────────────────────────────────── */
TableRule& TableRule::title_fg(const std::string& hex) { _title_fg = Srgb::from_hex(hex); _set |= T_TITLE_FG; return *this; }
TableRule& TableRule::title_fg(const Srgb& c) { _title_fg = c; _set |= T_TITLE_FG; return *this; }
TableRule& TableRule::title_bg(const std::string& hex) { _title_bg = Srgb::from_hex(hex); _set |= T_TITLE_BG | T_TITLE_HAS_BG; _title_has_bg = true; return *this; }
TableRule& TableRule::title_bg(const Srgb& c) { _title_bg = c; _set |= T_TITLE_BG | T_TITLE_HAS_BG; _title_has_bg = true; return *this; }
TableRule& TableRule::title_font(int f) { _title_font = f; _set |= T_TITLE_FONT; return *this; }
TableRule& TableRule::title_has_bg(bool v) { _title_has_bg = v; _set |= T_TITLE_HAS_BG; return *this; }
TableRule& TableRule::footer_fg(const std::string& hex) { _footer_fg = Srgb::from_hex(hex); _set |= T_FOOTER_FG; return *this; }
TableRule& TableRule::footer_fg(const Srgb& c) { _footer_fg = c; _set |= T_FOOTER_FG; return *this; }
TableRule& TableRule::footer_font(int f) { _footer_font = f; _set |= T_FOOTER_FONT; return *this; }

/* ── layout ───────────────────────────────────────────────────────────── */
TableRule& TableRule::pad(int p) { _pad = p; _set |= T_PAD; return *this; }
TableRule& TableRule::outer_margin(int m) { _outer_margin = m; _set |= T_OUTER_MARGIN; return *this; }
TableRule& TableRule::max_col_width(int w) { _max_col_width = w; _set |= T_MAX_COL_WIDTH; return *this; }
TableRule& TableRule::show_row_numbers(bool v) { _show_row_nums = v; _set |= T_SHOW_ROW_NUMS; return *this; }
TableRule& TableRule::row_separator(bool v) { _row_separator = v; _set |= T_ROW_SEPARATOR; return *this; }
TableRule& TableRule::compact(bool v) { _compact = v; _set |= T_COMPACT; return *this; }

/* ── focus style ──────────────────────────────────────────────────────── */
TableRule& TableRule::focus_fg(const std::string& hex) { _focus_fg = Srgb::from_hex(hex); _set |= T_FOCUS_FG; return *this; }
TableRule& TableRule::focus_fg(const Srgb& c) { _focus_fg = c; _set |= T_FOCUS_FG; return *this; }
TableRule& TableRule::focus_bg(const std::string& hex) { _focus_bg = Srgb::from_hex(hex); _set |= T_FOCUS_BG; return *this; }
TableRule& TableRule::focus_bg(const Srgb& c) { _focus_bg = c; _set |= T_FOCUS_BG; return *this; }
TableRule& TableRule::focus_font(int f) { _focus_font = f; _set |= T_FOCUS_FONT; return *this; }

/* ── presets ──────────────────────────────────────────────────────────── */
TableRule& TableRule::preset_minimal() { _base = TableStyle::minimal(); _set |= T_PRESET; return *this; }
TableRule& TableRule::preset_modern()  { _base = TableStyle::modern();  _set |= T_PRESET; return *this; }
TableRule& TableRule::preset_heavy()   { _base = TableStyle::heavy_style(); _set |= T_PRESET; return *this; }
TableRule& TableRule::preset_matrix()  { _base = TableStyle::matrix();  _set |= T_PRESET; return *this; }
TableRule& TableRule::preset_postman() { _base = TableStyle::postman(); _set |= T_PRESET; return *this; }

/* ── has_values ───────────────────────────────────────────────────────── */
bool TableRule::has_values() const { return _set != 0; }

/* ── apply_to: merge declared props into a TableStyle ─────────────────── */
void TableRule::apply_to(TableStyle& ts) const
{
	/* if a preset was set, start from that base */
	if (_set & T_PRESET)      ts = _base;

	/* then override individual properties */
	if (_set & T_BORDER_PRESET)   ts.border           = _border;
	if (_set & T_BORDER_COLOR)    ts.border_color      = _border_color;
	if (_set & T_HEADER_FG)       ts.header_fg         = _header_fg;
	if (_set & T_HEADER_BG)       ts.header_bg         = _header_bg;
	if (_set & T_HEADER_FONT)     ts.header_font       = _header_font;
	if (_set & T_HEADER_HAS_BG)   ts.header_has_bg     = _header_has_bg;
	if (_set & T_SHOW_HEADER)     ts.show_header       = _show_header;
	if (_set & T_HEADER_SEP)      ts.header_separator   = _header_sep;
	if (_set & T_CELL_FG)         ts.cell_fg           = _cell_fg;
	if (_set & T_CELL_BG)         ts.cell_bg           = _cell_bg;
	if (_set & T_CELL_ALT_BG)     ts.cell_alt_bg       = _cell_alt_bg;
	if (_set & T_ZEBRA)           ts.zebra             = _zebra;
	if (_set & T_CELL_HAS_BG)     ts.cell_has_bg       = _cell_has_bg;
	if (_set & T_TITLE_FG)        ts.title_fg          = _title_fg;
	if (_set & T_TITLE_BG)        ts.title_bg          = _title_bg;
	if (_set & T_TITLE_FONT)      ts.title_font        = _title_font;
	if (_set & T_TITLE_HAS_BG)    ts.title_has_bg      = _title_has_bg;
	if (_set & T_FOOTER_FG)       ts.footer_fg         = _footer_fg;
	if (_set & T_FOOTER_FONT)     ts.footer_font       = _footer_font;
	if (_set & T_PAD)             ts.pad               = _pad;
	if (_set & T_OUTER_MARGIN)    ts.outer_margin      = _outer_margin;
	if (_set & T_MAX_COL_WIDTH)   ts.max_col_width     = _max_col_width;
	if (_set & T_SHOW_ROW_NUMS)   ts.show_row_numbers  = _show_row_nums;
	if (_set & T_ROW_SEPARATOR)   ts.row_separator     = _row_separator;
	if (_set & T_COMPACT)         ts.compact           = _compact;
	if (_set & T_FOCUS_FG)        { ts.focus_style.fg = _focus_fg; ts.focus_style.has_fg = true; }
	if (_set & T_FOCUS_BG)        { ts.focus_style.bg = _focus_bg; ts.focus_style.has_bg = true; }
	if (_set & T_FOCUS_FONT)      { ts.focus_style.font = _focus_font; ts.focus_style.has_font = true; }
}

/* ======================================================================
 *  Rule -- OCF
 * ====================================================================== */

Rule::Rule()
	: _set(0), _fg(), _bg(), _border(), _font(FONT_NONE),
	  _align(ALIGN_LEFT), _width(60), _pad_l(1), _pad_r(1), _pad_v(0),
	  _margin_l(0), _space_before(0), _space_after(0), _has_bg(false) {}

Rule::Rule(const Rule& o)
	: _set(o._set), _fg(o._fg), _bg(o._bg), _border(o._border),
	  _font(o._font), _align(o._align), _width(o._width),
	  _pad_l(o._pad_l), _pad_r(o._pad_r), _pad_v(o._pad_v),
	  _margin_l(o._margin_l), _space_before(o._space_before),
	  _space_after(o._space_after), _has_bg(o._has_bg),
	  _glyph(o._glyph), _body_glyph(o._body_glyph), _sep(o._sep) {}

Rule& Rule::operator=(const Rule& o)
{
	if (this != &o)
	{
		_set = o._set; _fg = o._fg; _bg = o._bg; _border = o._border;
		_font = o._font; _align = o._align; _width = o._width;
		_pad_l = o._pad_l; _pad_r = o._pad_r; _pad_v = o._pad_v;
		_margin_l = o._margin_l; _space_before = o._space_before;
		_space_after = o._space_after; _has_bg = o._has_bg;
		_glyph = o._glyph; _body_glyph = o._body_glyph; _sep = o._sep;
	}
	return *this;
}

Rule::~Rule() {}

/* ── color setters ────────────────────────────────────────────────────── */

Rule& Rule::fg(const std::string& hex) { _fg = Srgb::from_hex(hex); _set |= P_FG; return *this; }
Rule& Rule::fg(const Srgb& c)          { _fg = c; _set |= P_FG; return *this; }
Rule& Rule::bg(const std::string& hex) { _bg = Srgb::from_hex(hex); _set |= P_BG | P_HAS_BG; _has_bg = true; return *this; }
Rule& Rule::bg(const Srgb& c)          { _bg = c; _set |= P_BG | P_HAS_BG; _has_bg = true; return *this; }
Rule& Rule::border(const std::string& hex) { _border = Srgb::from_hex(hex); _set |= P_BORDER; return *this; }
Rule& Rule::border(const Srgb& c)          { _border = c; _set |= P_BORDER; return *this; }

/* ── font shortcuts ───────────────────────────────────────────────────── */

Rule& Rule::bold()      { _font |= FONT_BOLD;      _set |= P_FONT; return *this; }
Rule& Rule::italic()    { _font |= FONT_ITALIC;    _set |= P_FONT; return *this; }
Rule& Rule::dim()       { _font |= FONT_DIM;       _set |= P_FONT; return *this; }
Rule& Rule::underline() { _font |= FONT_UNDERLINE; _set |= P_FONT; return *this; }
Rule& Rule::strike()    { _font |= FONT_STRIKE;    _set |= P_FONT; return *this; }
Rule& Rule::blink()     { _font |= FONT_BLINK;     _set |= P_FONT; return *this; }
Rule& Rule::reverse()   { _font |= FONT_REVERSE;   _set |= P_FONT; return *this; }
Rule& Rule::font(int f) { _font = f;               _set |= P_FONT; return *this; }

/* ── alignment ────────────────────────────────────────────────────────── */

Rule& Rule::left()       { _align = ALIGN_LEFT;   _set |= P_ALIGN; return *this; }
Rule& Rule::center()     { _align = ALIGN_CENTER; _set |= P_ALIGN; return *this; }
Rule& Rule::right()      { _align = ALIGN_RIGHT;  _set |= P_ALIGN; return *this; }
Rule& Rule::align(Align a) { _align = a; _set |= P_ALIGN; return *this; }

/* ── dimensions ───────────────────────────────────────────────────────── */

Rule& Rule::width(int w)    { _width = w; _set |= P_WIDTH; return *this; }
Rule& Rule::pad(int l, int r)         { _pad_l = l; _pad_r = r; _set |= P_PAD_L | P_PAD_R; return *this; }
Rule& Rule::pad(int l, int r, int v)  { _pad_l = l; _pad_r = r; _pad_v = v; _set |= P_PAD_L | P_PAD_R | P_PAD_V; return *this; }
Rule& Rule::pad_left(int n)   { _pad_l = n;   _set |= P_PAD_L;   return *this; }
Rule& Rule::pad_right(int n)  { _pad_r = n;   _set |= P_PAD_R;   return *this; }
Rule& Rule::pad_vert(int n)   { _pad_v = n;   _set |= P_PAD_V;   return *this; }
Rule& Rule::margin_left(int n) { _margin_l = n; _set |= P_MARGIN_L; return *this; }
Rule& Rule::spacing(int b, int a) { _space_before = b; _space_after = a; _set |= P_SPACE_B | P_SPACE_A; return *this; }
Rule& Rule::space_before(int n) { _space_before = n; _set |= P_SPACE_B; return *this; }
Rule& Rule::space_after(int n)  { _space_after = n;  _set |= P_SPACE_A; return *this; }

/* ── decoration ───────────────────────────────────────────────────────── */

Rule& Rule::glyph(const std::string& g)      { _glyph = g;      _set |= P_GLYPH;      return *this; }
Rule& Rule::body_glyph(const std::string& g)  { _body_glyph = g; _set |= P_BODY_GLYPH; return *this; }
Rule& Rule::sep(const std::string& s)          { _sep = s;        _set |= P_SEP;        return *this; }
Rule& Rule::has_bg(bool v) { _has_bg = v; _set |= P_HAS_BG; return *this; }

/* ── apply_to: merge declared props into an ElemStyle ─────────────────── */

void Rule::apply_to(ElemStyle& es) const
{
	if (_set & P_FG)         es.fg     = _fg;
	if (_set & P_BG)         es.bg     = _bg;
	if (_set & P_BORDER)     es.border = _border;
	if (_set & P_FONT)       es.font   = _font;
	if (_set & P_ALIGN)      es.align  = _align;
	if (_set & P_WIDTH)      es.width  = _width;
	if (_set & P_PAD_L)      es.pad_l  = _pad_l;
	if (_set & P_PAD_R)      es.pad_r  = _pad_r;
	if (_set & P_PAD_V)      es.pad_v  = _pad_v;
	if (_set & P_MARGIN_L)   es.margin_l     = _margin_l;
	if (_set & P_SPACE_B)    es.space_before = _space_before;
	if (_set & P_SPACE_A)    es.space_after  = _space_after;
	if (_set & P_HAS_BG)     es.has_bg       = _has_bg;
	if (_set & P_GLYPH)      es.glyph        = _glyph;
	if (_set & P_BODY_GLYPH) es.body_glyph   = _body_glyph;
	if (_set & P_SEP)        es.sep_str      = _sep;
}

/* ======================================================================
 *  StyleSheet -- OCF
 * ====================================================================== */

StyleSheet::StyleSheet()
	: _rule_count(0), _global_width(0), _has_global_width(false) {}

StyleSheet::StyleSheet(const StyleSheet& o)
	: _rule_count(o._rule_count),
	  _global_width(o._global_width),
	  _has_global_width(o._has_global_width),
	  _table_rule(o._table_rule)
{
	for (int i = 0; i < _rule_count; ++i)
		_rules[i] = o._rules[i];
}

StyleSheet& StyleSheet::operator=(const StyleSheet& o)
{
	if (this != &o)
	{
		_rule_count = o._rule_count;
		_global_width = o._global_width;
		_has_global_width = o._has_global_width;
		_table_rule = o._table_rule;
		for (int i = 0; i < _rule_count; ++i)
			_rules[i] = o._rules[i];
	}
	return *this;
}

StyleSheet::~StyleSheet() {}

/* ── selector access ──────────────────────────────────────────────────── */

Rule& StyleSheet::operator[](const std::string& selector)
{
	/* find existing */
	for (int i = 0; i < _rule_count; ++i)
	{
		if (_rules[i].name == selector)
			return _rules[i].rule;
	}
	/* create new */
	if (_rule_count < MAX_RULES)
	{
		_rules[_rule_count].name = selector;
		_rules[_rule_count].used = true;
		return _rules[_rule_count++].rule;
	}
	/* overflow: return last (shouldn't happen with MAX=32) */
	return _rules[_rule_count - 1].rule;
}

StyleSheet& StyleSheet::width(int w)
{
	_global_width = w;
	_has_global_width = true;
	return *this;
}

/* ── table rule accessor ──────────────────────────────────────────────── */

TableRule& StyleSheet::table()             { return _table_rule; }
const TableRule& StyleSheet::table() const { return _table_rule; }

/* ── internal helpers ─────────────────────────────────────────────────── */

const Rule* StyleSheet::_find(const std::string& name) const
{
	for (int i = 0; i < _rule_count; ++i)
		if (_rules[i].name == name)
			return &_rules[i].rule;
	return 0;
}

void StyleSheet::_apply_rule(const std::string& name, ElemStyle& es) const
{
	const Rule* r = _find(name);
	if (r) r->apply_to(es);
	if (_has_global_width) es.width = _global_width;
}

/* ── apply all rules to TermStyle ─────────────────────────────────────── */

void StyleSheet::apply(TermStyle& ts) const
{
	_apply_rule("h1",      ts.h1_style);
	_apply_rule("h2",      ts.h2_style);
	_apply_rule("h3",      ts.h3_style);
	_apply_rule("body",    ts.body_style);
	_apply_rule("text",    ts.body_style);       /* alias */
	_apply_rule("quote",   ts.quote_style);
	_apply_rule("info",    ts.info_style);
	_apply_rule("warn",    ts.warn_style);
	_apply_rule("error",   ts.error_style);
	_apply_rule("success", ts.success_style);
	_apply_rule("danger",  ts.danger_style);
	_apply_rule("trace",   ts.trace_style);
	_apply_rule("sep",     ts.sep_style);
	_apply_rule("section", ts.section_style);
	_apply_rule("callout", ts.callout_style);
	_apply_rule("bullet",  ts.bullet_style);
	_apply_rule("ol",      ts.ol_style);
	_apply_rule("bold",      ts.bold_style);
	_apply_rule("italic",    ts.italic_style);
	_apply_rule("underline", ts.underline_style);
	_apply_rule("strike",    ts.strike_style);
	_apply_rule("dim",       ts.dim_style);
	_apply_rule("code",      ts.code_style);
}

/* ── apply all rules + table style ────────────────────────────────────── */

void StyleSheet::apply(TermStyle& ts, TermWriter& w) const
{
	apply(ts);
	apply_table(w);
	apply_callouts(w);
}

void StyleSheet::apply_table(TermWriter& w) const
{
	if (_table_rule.has_values())
		_table_rule.apply_to(w.table_style());
}

void StyleSheet::apply_callouts(TermWriter& w) const
{
	static const char PREFIX[] = "callout.";
	static const int PREFIX_LEN = 8;
	for (int i = 0; i < _rule_count; ++i)
	{
		if (_rules[i].name.compare(0, PREFIX_LEN, PREFIX) == 0)
		{
			std::string name = _rules[i].name.substr(PREFIX_LEN);
			ElemStyle* existing = w.get_callout(name);
			if (existing)
			{
				/* merge rule onto existing default callout */
				_rules[i].rule.apply_to(*existing);
			}
			else
			{
				/* create new callout from scratch */
				ElemStyle es;
				es.has_bg = true;
				es.width = 60;
				es.pad_l = 1;
				es.pad_r = 1;
				es.margin_l = 1;
				_rules[i].rule.apply_to(es);
				w.define_callout(name, es);
			}
		}
	}
}

/* ======================================================================
 *  Built-in theme presets
 * ====================================================================== */

StyleSheet StyleSheet::dracula()
{
	StyleSheet ss;

	ss["h1"]
		.fg("#FF79C6").bg("#282A36").border("#FF79C6")
		.bold().center()
		.pad(3, 3, 1).spacing(1, 1)
		.glyph("\xe2\x97\x86 ").sep("\xe2\x95\x90");           /* ◆  ═ */

	ss["h2"]
		.fg("#BD93F9").border("#6272A4")
		.bold()
		.pad(1, 1).spacing(1, 0)
		.glyph("\xe2\x96\xb8 ").sep("\xe2\x94\x81");           /* ▸  ━ */

	ss["h3"]
		.fg("#50FA7B").border("#44475A")
		.bold().italic()
		.pad(1, 1).spacing(1, 0)
		.glyph("\xe2\x97\x8f ").sep("\xe2\x95\x8c");           /* ●  ╌ */

	ss["body"].fg("#F8F8F2").pad_left(2);
	ss["quote"].fg("#6272A4").italic().glyph("\xe2\x94\x82 ").margin_left(3);
	ss["info"].fg("#8BE9FD").glyph("\xe2\x84\xb9  ");          /* ℹ  */
	ss["warn"].fg("#FFB86C").bold().glyph("\xe2\x9a\xa0  ");   /* ⚠  */
	ss["error"].fg("#FF5555").bold().glyph("\xe2\x9c\x97 ");   /* ✗  */
	ss["success"].fg("#50FA7B").glyph("\xe2\x9c\x94 ");        /* ✔  */
	ss["danger"].fg("#FF5555").bold().glyph("\xe2\x98\xa0  "); /* ☠  */
	ss["trace"].fg("#BD93F9").dim().glyph("\xe2\x97\x87 ");    /* ◇  */
	ss["sep"].border("#44475A").sep("\xe2\x94\x80");            /* ─  */

	/* bullet: ◆ (diamond) */
	ss["bullet"].fg("#BD93F9").glyph("\xe2\x97\x86 ").pad_left(3);
	/* ol: "N) " format */
	ss["ol"].fg("#BD93F9").pad_left(3).glyph(") ");

	ss["callout"].fg("#F8F8F2").bg("#282A36").border("#6272A4").has_bg(true);

	/* -- named callouts with Dracula palette -- */
	ss["callout.tip"].fg("#50FA7B").bg("#0C1E12").border("#50FA7B").glyph("\xe2\x9c\x94 ").has_bg(true);
	ss["callout.note"].fg("#8BE9FD").bg("#0C1223").border("#8BE9FD").glyph("\xe2\x84\xb9  ").has_bg(true);
	ss["callout.warning"].fg("#FFB86C").bg("#231C0A").border("#FFB86C").glyph("\xe2\x9a\xa0  ").has_bg(true);
	ss["callout.danger"].fg("#FF5555").bg("#260C0C").border("#FF5555").glyph("\xe2\x98\xa0  ").has_bg(true);
	ss["callout.important"].fg("#FF79C6").bg("#1F0A1F").border("#FF79C6").glyph("\xe2\x97\x86 ").has_bg(true);
	ss["callout.success"].fg("#50FA7B").bg("#0A200D").border("#50FA7B").glyph("\xe2\x9c\x94 ").has_bg(true);
	ss["callout.error"].fg("#FF5555").bg("#280A0A").border("#FF5555").glyph("\xe2\x9c\x97 ").has_bg(true);
	ss["callout.info"].fg("#8BE9FD").bg("#0A1923").border("#8BE9FD").glyph("\xe2\x84\xb9  ").has_bg(true);

	ss["bold"].fg("#F8F8F2").font(FONT_BOLD).pad_left(2);
	ss["italic"].fg("#BD93F9").font(FONT_ITALIC).pad_left(2);
	ss["underline"].fg("#8BE9FD").font(FONT_UNDERLINE).pad_left(2);
	ss["strike"].fg("#6272A4").font(FONT_STRIKE).pad_left(2);
	ss["dim"].fg("#6272A4").font(FONT_DIM).pad_left(2);
	ss["code"].fg("#F1FA8C").bg("#282A36").font(FONT_NONE).pad_left(2);

	ss.table()
		.border_preset(BORDER_ROUNDED)
		.border_color("#6272A4")
		.header_fg("#FF79C6").header_bg("#282A36").header_font(FONT_BOLD)
		.cell_fg("#F8F8F2").cell_bg("#282A36").cell_alt_bg("#2D303D")
		.zebra(true).cell_has_bg(true).header_has_bg(true)
		.title_fg("#FF79C6").title_bg("#282A36").title_has_bg(true)
		.footer_fg("#6272A4");

	ss.width(64);
	return ss;
}

StyleSheet StyleSheet::nord()
{
	StyleSheet ss;

	ss["h1"]
		.fg("#ECEFF4").bg("#2E3440").border("#88C0D0")
		.bold().center()
		.pad(3, 3, 1).spacing(1, 1)
		.glyph("\xe2\x97\x87 ").sep("\xe2\x95\x90");           /* ◇  ═ */

	ss["h2"]
		.fg("#81A1C1").border("#4C566A")
		.bold()
		.pad(1, 1).spacing(1, 0)
		.glyph("\xe2\x97\x86 ").sep("\xe2\x94\x81");           /* ◆  ━ */

	ss["h3"]
		.fg("#A3BE8C").border("#4C566A")
		.bold()
		.pad(1, 1).spacing(1, 0)
		.glyph("\xe2\x96\xb8 ").sep("\xe2\x95\x8c");           /* ▸  ╌ */

	ss["body"].fg("#D8DEE9").pad_left(2);
	ss["quote"].fg("#616E88").italic().glyph("\xe2\x94\x82 ").margin_left(3);
	ss["info"].fg("#88C0D0").glyph("\xe2\x84\xb9  ");
	ss["warn"].fg("#EBCB8B").bold().glyph("\xe2\x9a\xa0  ");
	ss["error"].fg("#BF616A").bold().glyph("\xe2\x9c\x97 ");
	ss["success"].fg("#A3BE8C").glyph("\xe2\x9c\x94 ");
	ss["danger"].fg("#BF616A").bold().glyph("\xe2\x98\xa0  ");
	ss["trace"].fg("#B48EAD").dim().glyph("\xe2\x97\x87 ");
	ss["sep"].border("#4C566A").sep("\xe2\x94\x80");

	/* bullet: ▸ (triangle) */
	ss["bullet"].fg("#88C0D0").glyph("\xe2\x96\xb8 ").pad_left(3);
	/* ol: "N. " format (default) */
	ss["ol"].fg("#88C0D0").pad_left(3).glyph(". ");

	ss["callout"].fg("#D8DEE9").bg("#2E3440").border("#4C566A").has_bg(true);

	/* -- named callouts with Nord palette (arctic/frost tones) -- */
	ss["callout.tip"].fg("#A3BE8C").bg("#0E1A0E").border("#A3BE8C").glyph("\xe2\x9c\x94 ").has_bg(true);
	ss["callout.note"].fg("#5E81AC").bg("#0C1220").border("#5E81AC").glyph("\xe2\x84\xb9  ").has_bg(true);
	ss["callout.warning"].fg("#EBCB8B").bg("#1C1A0C").border("#EBCB8B").glyph("\xe2\x9a\xa0  ").has_bg(true);
	ss["callout.danger"].fg("#BF616A").bg("#1E0C0E").border("#BF616A").glyph("\xe2\x98\xa0  ").has_bg(true);
	ss["callout.important"].fg("#B48EAD").bg("#1A0E1C").border("#B48EAD").glyph("\xe2\x97\x86 ").has_bg(true);
	ss["callout.success"].fg("#A3BE8C").bg("#0C1A0A").border("#A3BE8C").glyph("\xe2\x9c\x94 ").has_bg(true);
	ss["callout.error"].fg("#BF616A").bg("#200A0C").border("#BF616A").glyph("\xe2\x9c\x97 ").has_bg(true);
	ss["callout.info"].fg("#88C0D0").bg("#0A161E").border("#88C0D0").glyph("\xe2\x84\xb9  ").has_bg(true);

	ss["bold"].fg("#ECEFF4").font(FONT_BOLD).pad_left(2);
	ss["italic"].fg("#B48EAD").font(FONT_ITALIC).pad_left(2);
	ss["underline"].fg("#88C0D0").font(FONT_UNDERLINE).pad_left(2);
	ss["strike"].fg("#4C566A").font(FONT_STRIKE).pad_left(2);
	ss["dim"].fg("#616E88").font(FONT_DIM).pad_left(2);
	ss["code"].fg("#A3BE8C").bg("#3B4252").font(FONT_NONE).pad_left(2);

	ss.table()
		.border_preset(BORDER_LIGHT)
		.border_color("#4C566A")
		.header_fg("#88C0D0").header_bg("#2E3440").header_font(FONT_BOLD)
		.cell_fg("#D8DEE9").cell_bg("#2E3440").cell_alt_bg("#3B4252")
		.zebra(true).cell_has_bg(true).header_has_bg(true)
		.title_fg("#88C0D0").title_bg("#2E3440").title_has_bg(true)
		.footer_fg("#616E88");

	ss.width(64);
	return ss;
}

StyleSheet StyleSheet::monokai()
{
	StyleSheet ss;

	ss["h1"]
		.fg("#F92672").bg("#272822").border("#F92672")
		.bold().center()
		.pad(3, 3, 1).spacing(1, 1)
		.glyph("\xe2\x97\x86 ").sep("\xe2\x95\x90");

	ss["h2"]
		.fg("#66D9EF").border("#49483E")
		.bold()
		.pad(1, 1).spacing(1, 0)
		.glyph("\xe2\x9e\x9c ").sep("\xe2\x94\x81");           /* ➜  ━ */

	ss["h3"]
		.fg("#A6E22E").border("#49483E")
		.bold()
		.pad(1, 1).spacing(1, 0)
		.glyph("\xe2\x98\x85 ").sep("\xe2\x95\x8c");           /* ★  ╌ */

	ss["body"].fg("#F8F8F2").pad_left(2);
	ss["quote"].fg("#75715E").italic().glyph("\xe2\x94\x82 ").margin_left(3);
	ss["info"].fg("#66D9EF").glyph("\xe2\x84\xb9  ");
	ss["warn"].fg("#E6DB74").bold().glyph("\xe2\x9a\xa0  ");
	ss["error"].fg("#F92672").bold().glyph("\xe2\x9c\x97 ");
	ss["success"].fg("#A6E22E").glyph("\xe2\x9c\x94 ");
	ss["danger"].fg("#F92672").bold().glyph("\xe2\x98\xa0  ");
	ss["trace"].fg("#AE81FF").dim().glyph("\xe2\x97\x87 ");
	ss["sep"].border("#49483E").sep("\xe2\x94\x80");

	/* bullet: ★ (star) */
	ss["bullet"].fg("#FD971F").glyph("\xe2\x98\x85 ").pad_left(3);
	/* ol: "N- " format */
	ss["ol"].fg("#FD971F").pad_left(3).glyph("- ");

	ss["callout"].fg("#F8F8F2").bg("#272822").border("#49483E").has_bg(true);

	/* -- named callouts with Monokai palette (vibrant neon) -- */
	ss["callout.tip"].fg("#A6E22E").bg("#0E1A06").border("#A6E22E").glyph("\xe2\x9c\x94 ").has_bg(true);
	ss["callout.note"].fg("#66D9EF").bg("#061A22").border("#66D9EF").glyph("\xe2\x84\xb9  ").has_bg(true);
	ss["callout.warning"].fg("#E6DB74").bg("#1C1A06").border("#E6DB74").glyph("\xe2\x9a\xa0  ").has_bg(true);
	ss["callout.danger"].fg("#F92672").bg("#1E0610").border("#F92672").glyph("\xe2\x98\xa0  ").has_bg(true);
	ss["callout.important"].fg("#AE81FF").bg("#14061E").border("#AE81FF").glyph("\xe2\x97\x86 ").has_bg(true);
	ss["callout.success"].fg("#A6E22E").bg("#0A1A04").border("#A6E22E").glyph("\xe2\x9c\x94 ").has_bg(true);
	ss["callout.error"].fg("#F92672").bg("#200408").border("#F92672").glyph("\xe2\x9c\x97 ").has_bg(true);
	ss["callout.info"].fg("#66D9EF").bg("#04161E").border("#66D9EF").glyph("\xe2\x84\xb9  ").has_bg(true);

	ss["bold"].fg("#F8F8F2").font(FONT_BOLD).pad_left(2);
	ss["italic"].fg("#E6DB74").font(FONT_ITALIC).pad_left(2);
	ss["underline"].fg("#66D9EF").font(FONT_UNDERLINE).pad_left(2);
	ss["strike"].fg("#75715E").font(FONT_STRIKE).pad_left(2);
	ss["dim"].fg("#75715E").font(FONT_DIM).pad_left(2);
	ss["code"].fg("#A6E22E").bg("#2E2F26").font(FONT_NONE).pad_left(2);

	ss.table()
		.border_preset(BORDER_HEAVY)
		.border_color("#49483E")
		.header_fg("#F92672").header_bg("#272822").header_font(FONT_BOLD)
		.cell_fg("#F8F8F2").cell_bg("#272822").cell_alt_bg("#2E2F26")
		.zebra(true).cell_has_bg(true).header_has_bg(true)
		.title_fg("#F92672").title_bg("#272822").title_has_bg(true)
		.footer_fg("#75715E");

	ss.width(64);
	return ss;
}

StyleSheet StyleSheet::solarized()
{
	StyleSheet ss;

	ss["h1"]
		.fg("#CB4B16").bg("#002B36").border("#CB4B16")
		.bold().center()
		.pad(3, 3, 1).spacing(1, 1)
		.glyph("\xe2\x9e\x9c ").sep("\xe2\x95\x90");           /* ➜  ═ */

	ss["h2"]
		.fg("#268BD2").border("#073642")
		.bold()
		.pad(1, 1).spacing(1, 0)
		.glyph("\xe2\x97\x87 ").sep("\xe2\x94\x81");           /* ◇  ━ */

	ss["h3"]
		.fg("#859900").border("#073642")
		.bold().italic()
		.pad(1, 1).spacing(1, 0)
		.glyph("\xc2\xb7 ").sep("\xe2\x95\x8c");               /* ·  ╌ */

	ss["body"].fg("#839496").pad_left(2);
	ss["quote"].fg("#586E75").italic().glyph("\xe2\x94\x82 ").margin_left(3);
	ss["info"].fg("#2AA198").glyph("\xe2\x84\xb9  ");
	ss["warn"].fg("#B58900").bold().glyph("\xe2\x9a\xa0  ");
	ss["error"].fg("#DC322F").bold().glyph("\xe2\x9c\x97 ");
	ss["success"].fg("#859900").glyph("\xe2\x9c\x94 ");
	ss["danger"].fg("#DC322F").bold().glyph("\xe2\x98\xa0  ");
	ss["trace"].fg("#6C71C4").dim().glyph("\xe2\x97\x87 ");
	ss["sep"].border("#073642").sep("\xe2\x94\x80");

	/* bullet: ○ (circle) */
	ss["bullet"].fg("#2AA198").glyph("\xe2\x97\x8b ").pad_left(3);
	/* ol: "(N) " format */
	ss["ol"].fg("#2AA198").pad_left(3).body_glyph("(").glyph(") ");

	ss["callout"].fg("#839496").bg("#002B36").border("#073642").has_bg(true);

	/* -- named callouts with Solarized palette (warm/muted) -- */
	ss["callout.tip"].fg("#859900").bg("#001A00").border("#859900").glyph("\xe2\x9c\x94 ").has_bg(true);
	ss["callout.note"].fg("#268BD2").bg("#001828").border("#268BD2").glyph("\xe2\x84\xb9  ").has_bg(true);
	ss["callout.warning"].fg("#B58900").bg("#1A1400").border("#B58900").glyph("\xe2\x9a\xa0  ").has_bg(true);
	ss["callout.danger"].fg("#DC322F").bg("#1A0404").border("#DC322F").glyph("\xe2\x98\xa0  ").has_bg(true);
	ss["callout.important"].fg("#D33682").bg("#1A041A").border("#D33682").glyph("\xe2\x97\x86 ").has_bg(true);
	ss["callout.success"].fg("#859900").bg("#0A1800").border("#859900").glyph("\xe2\x9c\x94 ").has_bg(true);
	ss["callout.error"].fg("#DC322F").bg("#200204").border("#DC322F").glyph("\xe2\x9c\x97 ").has_bg(true);
	ss["callout.info"].fg("#2AA198").bg("#001A1A").border("#2AA198").glyph("\xe2\x84\xb9  ").has_bg(true);

	ss["bold"].fg("#FDF6E3").font(FONT_BOLD).pad_left(2);
	ss["italic"].fg("#6C71C4").font(FONT_ITALIC).pad_left(2);
	ss["underline"].fg("#2AA198").font(FONT_UNDERLINE).pad_left(2);
	ss["strike"].fg("#586E75").font(FONT_STRIKE).pad_left(2);
	ss["dim"].fg("#586E75").font(FONT_DIM).pad_left(2);
	ss["code"].fg("#B58900").bg("#073642").font(FONT_NONE).pad_left(2);

	ss.table()
		.border_preset(BORDER_DOUBLE)
		.border_color("#073642")
		.header_fg("#CB4B16").header_bg("#002B36").header_font(FONT_BOLD)
		.cell_fg("#839496").cell_bg("#002B36").cell_alt_bg("#073642")
		.zebra(true).cell_has_bg(true).header_has_bg(true)
		.title_fg("#CB4B16").title_bg("#002B36").title_has_bg(true)
		.footer_fg("#586E75");

	ss.width(64);
	return ss;
}

} /* namespace libcpp */
