/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/22 15:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/term/table.hpp"
#include "libcpp/term/style.hpp"
#include <sstream>
#include <cstring>

namespace libcpp
{

/* ── box-drawing character sets ──────────────────────────────────────── */

/* heavy */
static const char H_H[]  = "\xe2\x94\x81";
static const char H_V[]  = "\xe2\x94\x83";
static const char H_TL[] = "\xe2\x94\x8f";
static const char H_TR[] = "\xe2\x94\x93";
static const char H_BL[] = "\xe2\x94\x97";
static const char H_BR[] = "\xe2\x94\x9b";
static const char H_LT[] = "\xe2\x94\xa3";
static const char H_RT[] = "\xe2\x94\xab";
static const char H_TD[] = "\xe2\x94\xb3";
static const char H_TU[] = "\xe2\x94\xbb";
static const char H_X[]  = "\xe2\x95\x8b";

/* light */
static const char L_H[]  = "\xe2\x94\x80";
static const char L_V[]  = "\xe2\x94\x82";
static const char L_TL[] = "\xe2\x94\x8c";
static const char L_TR[] = "\xe2\x94\x90";
static const char L_BL[] = "\xe2\x94\x94";
static const char L_BR[] = "\xe2\x94\x98";
static const char L_LT[] = "\xe2\x94\x9c";
static const char L_RT[] = "\xe2\x94\xa4";
static const char L_TD[] = "\xe2\x94\xac";
static const char L_TU[] = "\xe2\x94\xb4";
static const char L_X[]  = "\xe2\x94\xbc";

/* double */
static const char D_H[]  = "\xe2\x95\x90";
static const char D_V[]  = "\xe2\x95\x91";
static const char D_TL[] = "\xe2\x95\x94";
static const char D_TR[] = "\xe2\x95\x97";
static const char D_BL[] = "\xe2\x95\x9a";
static const char D_BR[] = "\xe2\x95\x9d";
static const char D_LT[] = "\xe2\x95\xa0";
static const char D_RT[] = "\xe2\x95\xa3";
static const char D_TD[] = "\xe2\x95\xa6";
static const char D_TU[] = "\xe2\x95\xa9";
static const char D_X[]  = "\xe2\x95\xac";

/* rounded (same as light except corners) */
static const char R_TL[] = "\xe2\x95\xad";
static const char R_TR[] = "\xe2\x95\xae";
static const char R_BL[] = "\xe2\x95\xb0";
static const char R_BR[] = "\xe2\x95\xaf";

/* ═══════════════════════════════════════════════════════════════════════
 *  CellStyle
 * ══════════════════════════════════════════════════════════════════════ */

CellStyle::CellStyle()
	: fg(255,255,255), bg(0,0,0), font(0),
	  has_fg(false), has_bg(false), has_font(false) {}

CellStyle::CellStyle(const CellStyle& o)
	: fg(o.fg), bg(o.bg), font(o.font),
	  has_fg(o.has_fg), has_bg(o.has_bg), has_font(o.has_font) {}

CellStyle& CellStyle::operator=(const CellStyle& o)
{
	if (this != &o)
	{
		fg = o.fg; bg = o.bg; font = o.font;
		has_fg = o.has_fg; has_bg = o.has_bg; has_font = o.has_font;
	}
	return *this;
}

CellStyle::~CellStyle() {}

/* ═══════════════════════════════════════════════════════════════════════
 *  TableStyle
 * ══════════════════════════════════════════════════════════════════════ */

TableStyle::TableStyle()
	: border(BORDER_LIGHT), border_color(100, 100, 110),
	  header_fg(255, 255, 255), header_bg(40, 44, 52), header_font(FONT_BOLD),
	  header_separator(true), show_header(true), header_has_bg(false),
	  cell_fg(204, 204, 210), cell_bg(0, 0, 0), cell_alt_bg(22, 24, 30),
	  zebra(false), cell_has_bg(false),
	  title_fg(255, 255, 255), title_bg(40, 44, 52), title_font(FONT_BOLD),
	  title_has_bg(false),
	  footer_fg(140, 140, 150), footer_font(FONT_DIM),
	  pad(1), outer_margin(0), max_col_width(0),
	  show_row_numbers(false), row_separator(false), compact(false) {}

TableStyle::TableStyle(const TableStyle& o)
	: border(o.border), border_color(o.border_color),
	  header_fg(o.header_fg), header_bg(o.header_bg), header_font(o.header_font),
	  header_separator(o.header_separator), show_header(o.show_header),
	  header_has_bg(o.header_has_bg),
	  cell_fg(o.cell_fg), cell_bg(o.cell_bg), cell_alt_bg(o.cell_alt_bg),
	  zebra(o.zebra), cell_has_bg(o.cell_has_bg),
	  title_fg(o.title_fg), title_bg(o.title_bg), title_font(o.title_font),
	  title_has_bg(o.title_has_bg),
	  footer_fg(o.footer_fg), footer_font(o.footer_font),
	  pad(o.pad), outer_margin(o.outer_margin), max_col_width(o.max_col_width),
	  show_row_numbers(o.show_row_numbers), row_separator(o.row_separator),
	  compact(o.compact), focus_style(o.focus_style) {}

TableStyle& TableStyle::operator=(const TableStyle& o)
{
	if (this != &o)
	{
		border = o.border; border_color = o.border_color;
		header_fg = o.header_fg; header_bg = o.header_bg;
		header_font = o.header_font; header_separator = o.header_separator;
		show_header = o.show_header; header_has_bg = o.header_has_bg;
		cell_fg = o.cell_fg; cell_bg = o.cell_bg; cell_alt_bg = o.cell_alt_bg;
		zebra = o.zebra; cell_has_bg = o.cell_has_bg;
		title_fg = o.title_fg; title_bg = o.title_bg; title_font = o.title_font;
		title_has_bg = o.title_has_bg;
		footer_fg = o.footer_fg; footer_font = o.footer_font;
		pad = o.pad; outer_margin = o.outer_margin;
		max_col_width = o.max_col_width;
		show_row_numbers = o.show_row_numbers; row_separator = o.row_separator;
		compact = o.compact; focus_style = o.focus_style;
	}
	return *this;
}

TableStyle::~TableStyle() {}

/* ── presets ──────────────────────────────────────────────────────────── */

TableStyle TableStyle::minimal()
{
	TableStyle s;
	s.border = BORDER_NONE;
	s.header_separator = true;
	s.pad = 1;
	s.header_font = FONT_BOLD | FONT_DIM;
	s.compact = true;
	return s;
}

TableStyle TableStyle::modern()
{
	TableStyle s;
	s.border = BORDER_ROUNDED;
	s.border_color = Srgb(80, 90, 120);
	s.header_fg = Srgb(180, 210, 255);
	s.header_bg = Srgb(30, 35, 50);
	s.header_has_bg = true;
	s.header_font = FONT_BOLD;
	s.zebra = true;
	s.cell_bg = Srgb(18, 20, 28);
	s.cell_alt_bg = Srgb(25, 28, 38);
	s.cell_has_bg = true;
	s.pad = 1;
	return s;
}

TableStyle TableStyle::heavy_style()
{
	TableStyle s;
	s.border = BORDER_HEAVY;
	s.border_color = Srgb(160, 120, 200);
	s.header_fg = Srgb(255, 200, 100);
	s.header_bg = Srgb(40, 20, 60);
	s.header_has_bg = true;
	s.header_font = FONT_BOLD;
	s.pad = 1;
	s.row_separator = true;
	return s;
}

TableStyle TableStyle::matrix()
{
	TableStyle s;
	s.border = BORDER_LIGHT;
	s.border_color = Srgb(0, 120, 0);
	s.header_fg = Srgb(0, 255, 0);
	s.cell_fg = Srgb(0, 200, 0);
	s.header_font = FONT_BOLD;
	s.show_row_numbers = true;
	s.pad = 1;
	return s;
}

TableStyle TableStyle::postman()
{
	TableStyle s;
	s.border = BORDER_HEAVY;
	s.border_color = Srgb(141, 110, 200);
	s.header_fg = Srgb(183, 160, 220);
	s.header_bg = Srgb(30, 20, 45);
	s.header_has_bg = true;
	s.header_font = FONT_BOLD;
	s.title_fg = Srgb(219, 180, 255);
	s.title_bg = Srgb(30, 20, 45);
	s.title_font = FONT_BOLD;
	s.title_has_bg = true;
	s.cell_fg = Srgb(253, 253, 253);
	s.zebra = false;
	s.pad = 1;
	s.row_separator = false;
	s.focus_style.fg = Srgb(114, 200, 130);
	s.focus_style.has_fg = true;
	s.focus_style.font = FONT_BOLD;
	s.focus_style.has_font = true;
	return s;
}

/* ═══════════════════════════════════════════════════════════════════════
 *  Table
 * ══════════════════════════════════════════════════════════════════════ */

Table::Table() : _ncols(0), _nrows(0), _has_header(false), _has_custom_style(false)
{
	std::memset(_col_align, 0, sizeof(_col_align));
	std::memset(_col_widths, 0, sizeof(_col_widths));
	std::memset(_forced_widths, 0, sizeof(_forced_widths));
	std::memset(_highlight, 0, sizeof(_highlight));
}

Table::Table(int ncols) : _ncols(ncols), _nrows(0), _has_header(false), _has_custom_style(false)
{
	std::memset(_col_align, 0, sizeof(_col_align));
	std::memset(_col_widths, 0, sizeof(_col_widths));
	std::memset(_forced_widths, 0, sizeof(_forced_widths));
	std::memset(_highlight, 0, sizeof(_highlight));
}

Table::Table(const Table& o)
	: _ncols(o._ncols), _nrows(o._nrows), _has_header(o._has_header),
	  _has_custom_style(o._has_custom_style),
	  _footer(o._footer), _title(o._title), _style(o._style)
{
	for (int c = 0; c < MAX_COLS; ++c)
	{
		_headers[c] = o._headers[c];
		_col_align[c] = o._col_align[c];
		_col_widths[c] = o._col_widths[c];
		_forced_widths[c] = o._forced_widths[c];
	}
	for (int r = 0; r < MAX_ROWS; ++r)
	{
		_highlight[r] = o._highlight[r];
		for (int c = 0; c < MAX_COLS; ++c)
			_rows[r][c] = o._rows[r][c];
	}
}

Table& Table::operator=(const Table& o)
{
	if (this != &o)
	{
		_ncols = o._ncols; _nrows = o._nrows; _has_header = o._has_header;
		_has_custom_style = o._has_custom_style;
		_footer = o._footer; _title = o._title; _style = o._style;
		for (int c = 0; c < MAX_COLS; ++c)
		{
			_headers[c] = o._headers[c];
			_col_align[c] = o._col_align[c];
			_col_widths[c] = o._col_widths[c];
			_forced_widths[c] = o._forced_widths[c];
		}
		for (int r = 0; r < MAX_ROWS; ++r)
		{
			_highlight[r] = o._highlight[r];
			for (int c = 0; c < MAX_COLS; ++c)
				_rows[r][c] = o._rows[r][c];
		}
	}
	return *this;
}

Table::~Table() {}

/* ── building ─────────────────────────────────────────────────────────── */

void Table::set_cols(int n) { _ncols = (n > MAX_COLS) ? MAX_COLS : n; }

void Table::header(const std::string* cells, int n)
{
	int cnt = (n > _ncols) ? _ncols : n;
	for (int i = 0; i < cnt; ++i) _headers[i] = cells[i];
	_has_header = true;
}

void Table::row(const std::string* cells, int n)
{
	if (_nrows >= MAX_ROWS) return;
	int cnt = (n > _ncols) ? _ncols : n;
	for (int i = 0; i < cnt; ++i) _rows[_nrows][i] = cells[i];
	++_nrows;
}

void Table::set_footer(const std::string& text) { _footer = text; }
void Table::set_title(const std::string& title)  { _title = title; }
void Table::set_style(const TableStyle& s)       { _style = s; _has_custom_style = true; }
bool Table::has_custom_style() const              { return _has_custom_style; }

void Table::set_col_align(int col, int align)
{
	if (col >= 0 && col < MAX_COLS) _col_align[col] = align;
}

void Table::set_col_width(int col, int width)
{
	if (col >= 0 && col < MAX_COLS) _forced_widths[col] = width;
}

void Table::highlight_row(int row)
{
	if (row >= 0 && row < MAX_ROWS) _highlight[row] = true;
}

void Table::highlight_last_row()
{
	if (_nrows > 0) _highlight[_nrows - 1] = true;
}

/* convenience overloads */
void Table::header1(const std::string& a)
{ std::string h[1] = {a}; header(h, 1); }
void Table::header2(const std::string& a, const std::string& b)
{ std::string h[2] = {a, b}; header(h, 2); }
void Table::header3(const std::string& a, const std::string& b, const std::string& c)
{ std::string h[3] = {a, b, c}; header(h, 3); }
void Table::header4(const std::string& a, const std::string& b,
                    const std::string& c, const std::string& d)
{ std::string h[4] = {a, b, c, d}; header(h, 4); }
void Table::header5(const std::string& a, const std::string& b,
                    const std::string& c, const std::string& d,
                    const std::string& e)
{ std::string h[5] = {a, b, c, d, e}; header(h, 5); }
void Table::header6(const std::string& a, const std::string& b,
                    const std::string& c, const std::string& d,
                    const std::string& e, const std::string& f)
{ std::string h[6] = {a, b, c, d, e, f}; header(h, 6); }

void Table::row1(const std::string& a)
{ std::string r[1] = {a}; row(r, 1); }
void Table::row2(const std::string& a, const std::string& b)
{ std::string r[2] = {a, b}; row(r, 2); }
void Table::row3(const std::string& a, const std::string& b, const std::string& c)
{ std::string r[3] = {a, b, c}; row(r, 3); }
void Table::row4(const std::string& a, const std::string& b,
                 const std::string& c, const std::string& d)
{ std::string r[4] = {a, b, c, d}; row(r, 4); }
void Table::row5(const std::string& a, const std::string& b,
                 const std::string& c, const std::string& d,
                 const std::string& e)
{ std::string r[5] = {a, b, c, d, e}; row(r, 5); }
void Table::row6(const std::string& a, const std::string& b,
                 const std::string& c, const std::string& d,
                 const std::string& e, const std::string& f)
{ std::string r[6] = {a, b, c, d, e, f}; row(r, 6); }

int  Table::col_count() const  { return _ncols; }
int  Table::row_count() const  { return _nrows; }
bool Table::has_header() const { return _has_header; }
void Table::clear() { _nrows = 0; _has_header = false; std::memset(_highlight, 0, sizeof(_highlight)); }

std::string Table::to_csv(char delim) const
{
	std::string out;
	if (_has_header)
	{
		for (int c = 0; c < _ncols; ++c) { if (c) out += delim; out += _headers[c]; }
		out += '\n';
	}
	for (int r = 0; r < _nrows; ++r)
	{
		for (int c = 0; c < _ncols; ++c) { if (c) out += delim; out += _rows[r][c]; }
		out += '\n';
	}
	return out;
}

/* ── internal rendering ───────────────────────────────────────────────── */

std::string Table::_margin() const
{
	return TermUtils::spaces(_style.outer_margin);
}

void Table::_compute_widths() const
{
	int* w = const_cast<int*>(_col_widths);
	for (int c = 0; c < _ncols; ++c)
	{
		if (_forced_widths[c] > 0)
		{
			w[c] = _forced_widths[c];
			continue;
		}
		w[c] = 0;
		if (_has_header && _style.show_header)
		{
			int hw = TermUtils::vis_len(_headers[c]);
			if (hw > w[c]) w[c] = hw;
		}
		for (int r = 0; r < _nrows; ++r)
		{
			int rw = TermUtils::vis_len(_rows[r][c]);
			if (_style.max_col_width > 0 && rw > _style.max_col_width)
				rw = _style.max_col_width;
			if (rw > w[c]) w[c] = rw;
		}
		w[c] += _style.pad * 2;
	}
}

int Table::_total_width() const
{
	int tw = 0;
	for (int c = 0; c < _ncols; ++c) tw += _col_widths[c];
	if (_style.border != BORDER_NONE)
		tw += _ncols + 1; /* vertical bars */
	return tw;
}

Table::BoxChars Table::_resolve_box() const
{
	BoxChars bc;
	switch (_style.border)
	{
	case BORDER_HEAVY:
		bc.h = H_H; bc.v = H_V;
		bc.tl = H_TL; bc.tr = H_TR; bc.bl = H_BL; bc.br = H_BR;
		bc.lt = H_LT; bc.rt = H_RT; bc.td = H_TD; bc.tu = H_TU;
		bc.cross = H_X;
		break;
	case BORDER_DOUBLE:
		bc.h = D_H; bc.v = D_V;
		bc.tl = D_TL; bc.tr = D_TR; bc.bl = D_BL; bc.br = D_BR;
		bc.lt = D_LT; bc.rt = D_RT; bc.td = D_TD; bc.tu = D_TU;
		bc.cross = D_X;
		break;
	case BORDER_ROUNDED:
		bc.h = L_H; bc.v = L_V;
		bc.tl = R_TL; bc.tr = R_TR; bc.bl = R_BL; bc.br = R_BR;
		bc.lt = L_LT; bc.rt = L_RT; bc.td = L_TD; bc.tu = L_TU;
		bc.cross = L_X;
		break;
	case BORDER_ASCII:
		bc.h = "-"; bc.v = "|";
		bc.tl = "+"; bc.tr = "+"; bc.bl = "+"; bc.br = "+";
		bc.lt = "+"; bc.rt = "+"; bc.td = "+"; bc.tu = "+";
		bc.cross = "+";
		break;
	case BORDER_NONE:
		bc.h = " "; bc.v = " ";
		bc.tl = " "; bc.tr = " "; bc.bl = " "; bc.br = " ";
		bc.lt = " "; bc.rt = " "; bc.td = " "; bc.tu = " ";
		bc.cross = " ";
		break;
	default: /* BORDER_LIGHT */
		bc.h = L_H; bc.v = L_V;
		bc.tl = L_TL; bc.tr = L_TR; bc.bl = L_BL; bc.br = L_BR;
		bc.lt = L_LT; bc.rt = L_RT; bc.td = L_TD; bc.tu = L_TU;
		bc.cross = L_X;
		break;
	}
	return bc;
}

std::string Table::_render_hline(const BoxChars& bc,
	const char* left, const char* mid, const char* right) const
{
	std::ostringstream out;
	out << _margin();
	out << TermUtils::apply_fg(_style.border_color);
	out << left;
	for (int c = 0; c < _ncols; ++c)
	{
		for (int i = 0; i < _col_widths[c]; ++i) out << bc.h;
		out << ((c + 1 < _ncols) ? mid : right);
	}
	out << TermUtils::reset() << "\n";
	return out.str();
}

std::string Table::_truncate(const std::string& text, int max_w) const
{
	if (max_w <= 0 || TermUtils::vis_len(text) <= max_w)
		return text;
	/* simple byte-level truncation with ellipsis */
	std::string out;
	int vis = 0;
	for (std::size_t i = 0; i < text.size() && vis < max_w - 1; ++i)
	{
		unsigned char ch = (unsigned char)text[i];
		out += text[i];
		if (ch < 0x80 || (ch & 0xC0) != 0x80) ++vis;
	}
	out += "\xe2\x80\xa6"; /* … */
	return out;
}

std::string Table::_align_cell(const std::string& text, int width, int align) const
{
	int vl = TermUtils::vis_len(text);
	int total_pad = width - vl;
	if (total_pad < 0) total_pad = 0;
	if (align == ALIGN_CENTER)
	{
		int l = total_pad / 2;
		return TermUtils::spaces(l) + text + TermUtils::spaces(total_pad - l);
	}
	if (align == ALIGN_RIGHT)
		return TermUtils::spaces(total_pad) + text;
	return text + TermUtils::spaces(total_pad);
}

std::string Table::_render_row(const BoxChars& bc,
	const std::string* cells, bool is_header, int row_idx) const
{
	std::ostringstream out;
	bool hl = (!is_header && row_idx >= 0 && row_idx < MAX_ROWS && _highlight[row_idx]);
	bool use_alt = (!is_header && _style.zebra && (row_idx % 2 == 1));

	out << _margin();
	out << TermUtils::apply_fg(_style.border_color) << bc.v << TermUtils::reset();

	for (int c = 0; c < _ncols; ++c)
	{
		int content_w = _col_widths[c] - _style.pad * 2;
		if (content_w < 0) content_w = 0;
		std::string pad_s = TermUtils::spaces(_style.pad);

		std::string cell_text = cells[c];
		if (_style.max_col_width > 0)
			cell_text = _truncate(cell_text, _style.max_col_width);

		/* background */
		if (is_header && _style.header_has_bg)
			out << TermUtils::apply_bg(_style.header_bg);
		else if (hl && _style.focus_style.has_bg)
			out << TermUtils::apply_bg(_style.focus_style.bg);
		else if (_style.cell_has_bg)
			out << TermUtils::apply_bg(use_alt ? _style.cell_alt_bg : _style.cell_bg);

		/* foreground + font */
		if (hl && _style.focus_style.has_fg)
			out << TermUtils::apply_fg(_style.focus_style.fg);
		else if (is_header)
			out << TermUtils::apply_fg(_style.header_fg);
		else
			out << TermUtils::apply_fg(_style.cell_fg);

		if (hl && _style.focus_style.has_font)
			out << TermUtils::apply_font(_style.focus_style.font);
		else if (is_header)
			out << TermUtils::apply_font(_style.header_font);

		out << pad_s << _align_cell(cell_text, content_w, _col_align[c]) << pad_s;
		out << TermUtils::reset();
		out << TermUtils::apply_fg(_style.border_color) << bc.v << TermUtils::reset();
	}
	out << "\n";
	return out.str();
}

std::string Table::_render_title_bar(const BoxChars& bc) const
{
	int tw = _total_width();
	int inner = tw - 2; /* minus left+right border chars */
	if (inner < 1) inner = 1;

	std::ostringstream out;
	std::string rst = TermUtils::reset();

	/* top border of title */
	out << _render_hline(bc, bc.tl, bc.h, bc.tr);

	/* title content line */
	out << _margin();
	out << TermUtils::apply_fg(_style.border_color) << bc.v;
	if (_style.title_has_bg)
		out << TermUtils::apply_bg(_style.title_bg);
	out << TermUtils::apply_fg(_style.title_fg);
	out << TermUtils::apply_font(_style.title_font);
	out << " ";

	int title_vis = TermUtils::vis_len(_title);
	int pad_right = inner - title_vis - 1;
	if (pad_right < 0) pad_right = 0;
	out << _title << TermUtils::spaces(pad_right);
	out << rst;
	out << TermUtils::apply_fg(_style.border_color) << bc.v << rst << "\n";

	/* separator between title and table content */
	out << _render_hline(bc, bc.lt, bc.td, bc.rt);

	return out.str();
}

std::string Table::_render_footer_bar(const BoxChars& bc) const
{
	int tw = _total_width();
	int inner = tw - 2;
	if (inner < 1) inner = 1;

	std::ostringstream out;
	std::string rst = TermUtils::reset();

	/* separator */
	out << _render_hline(bc, bc.lt, bc.tu, bc.rt);

	/* footer line */
	out << _margin();
	out << TermUtils::apply_fg(_style.border_color) << bc.v;
	out << TermUtils::apply_fg(_style.footer_fg);
	out << TermUtils::apply_font(_style.footer_font);
	out << " ";

	int fvis = TermUtils::vis_len(_footer);
	int pad_right = inner - fvis - 1;
	if (pad_right < 0) pad_right = 0;
	out << _footer << TermUtils::spaces(pad_right);
	out << rst;
	out << TermUtils::apply_fg(_style.border_color) << bc.v << rst << "\n";

	/* bottom border */
	out << _render_hline(bc, bc.bl, bc.h, bc.br);

	return out.str();
}

/* ── render ────────────────────────────────────────────────────────────── */

std::string Table::render() const
{
	if (_ncols <= 0) return "";

	_compute_widths();
	BoxChars bc = _resolve_box();
	std::ostringstream out;

	bool has_title = !_title.empty();
	bool has_footer = !_footer.empty();

	/* title bar */
	if (has_title)
	{
		out << _render_title_bar(bc);
	}
	else if (!_style.compact)
	{
		out << _render_hline(bc, bc.tl, bc.td, bc.tr);
	}

	/* header */
	if (_has_header && _style.show_header)
	{
		out << _render_row(bc, _headers, true, -1);
		if (_style.header_separator)
			out << _render_hline(bc, bc.lt, bc.cross, bc.rt);
	}

	/* data rows */
	for (int r = 0; r < _nrows; ++r)
	{
		out << _render_row(bc, _rows[r], false, r);
		if (_style.row_separator && r + 1 < _nrows)
			out << _render_hline(bc, bc.lt, bc.cross, bc.rt);
	}

	/* footer bar */
	if (has_footer)
	{
		out << _render_footer_bar(bc);
	}
	else if (!_style.compact)
	{
		out << _render_hline(bc, bc.bl, bc.tu, bc.br);
	}

	return out.str();
}

} /* namespace libcpp */
