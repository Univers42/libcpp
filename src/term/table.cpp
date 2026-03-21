/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 20:43:53 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/term/table.hpp"
#include "libcpp/term/style.hpp"
#include <sstream>
#include <cstring>

namespace libcpp
{

/* ── box-drawing glyphs ────────────────────────────────────────────────── */

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

/* rounded */
static const char R_TL[] = "\xe2\x95\xad";
static const char R_TR[] = "\xe2\x95\xae";
static const char R_BL[] = "\xe2\x95\xb0";
static const char R_BR[] = "\xe2\x95\xaf";

/* ══════════════════════════════════════════════════════════════════════════
 *  TableStyle
 * ═════════════════════════════════════════════════════════════════════════ */

TableStyle::TableStyle()
	: heavy(false), rounded(false), border_color(128, 128, 128),
	  header_fg(255, 255, 255), header_bg(0, 0, 0), header_font(FONT_BOLD),
	  cell_fg(204, 204, 204), pad(1), show_row_numbers(false) {}

TableStyle::TableStyle(const TableStyle& o)
	: heavy(o.heavy), rounded(o.rounded), border_color(o.border_color),
	  header_fg(o.header_fg), header_bg(o.header_bg),
	  header_font(o.header_font), cell_fg(o.cell_fg), pad(o.pad),
	  show_row_numbers(o.show_row_numbers) {}

TableStyle& TableStyle::operator=(const TableStyle& o)
{
	if (this != &o)
	{
		heavy = o.heavy; rounded = o.rounded; border_color = o.border_color;
		header_fg = o.header_fg; header_bg = o.header_bg;
		header_font = o.header_font; cell_fg = o.cell_fg; pad = o.pad;
		show_row_numbers = o.show_row_numbers;
	}
	return *this;
}

TableStyle::~TableStyle() {}

/* ══════════════════════════════════════════════════════════════════════════
 *  Table
 * ═════════════════════════════════════════════════════════════════════════ */

Table::Table() : _ncols(0), _nrows(0), _has_header(false)
{
	std::memset(_col_align, 0, sizeof(_col_align));
	std::memset(_col_widths, 0, sizeof(_col_widths));
}

Table::Table(int ncols) : _ncols(ncols), _nrows(0), _has_header(false)
{
	std::memset(_col_align, 0, sizeof(_col_align));
	std::memset(_col_widths, 0, sizeof(_col_widths));
}

Table::Table(const Table& o)
	: _ncols(o._ncols), _nrows(o._nrows), _has_header(o._has_header),
	  _footer(o._footer), _title(o._title), _style(o._style)
{
	for (int c = 0; c < MAX_COLS; ++c)
	{
		_headers[c] = o._headers[c];
		_col_align[c] = o._col_align[c];
		_col_widths[c] = o._col_widths[c];
	}
	for (int r = 0; r < MAX_ROWS; ++r)
		for (int c = 0; c < MAX_COLS; ++c)
			_rows[r][c] = o._rows[r][c];
}

Table& Table::operator=(const Table& o)
{
	if (this != &o)
	{
		_ncols = o._ncols; _nrows = o._nrows; _has_header = o._has_header;
		_footer = o._footer; _title = o._title; _style = o._style;
		for (int c = 0; c < MAX_COLS; ++c)
		{
			_headers[c] = o._headers[c];
			_col_align[c] = o._col_align[c];
			_col_widths[c] = o._col_widths[c];
		}
		for (int r = 0; r < MAX_ROWS; ++r)
			for (int c = 0; c < MAX_COLS; ++c)
				_rows[r][c] = o._rows[r][c];
	}
	return *this;
}

Table::~Table() {}

/* ── building ──────────────────────────────────────────────────────────── */

void Table::set_cols(int n)
{
	_ncols = (n > MAX_COLS) ? MAX_COLS : n;
}

void Table::header(const std::string* cells, int n)
{
	int cnt = (n > _ncols) ? _ncols : n;
	for (int i = 0; i < cnt; ++i)
		_headers[i] = cells[i];
	_has_header = true;
}

void Table::row(const std::string* cells, int n)
{
	if (_nrows >= MAX_ROWS) return;
	int cnt = (n > _ncols) ? _ncols : n;
	for (int i = 0; i < cnt; ++i)
		_rows[_nrows][i] = cells[i];
	++_nrows;
}

void Table::set_footer(const std::string& text) { _footer = text; }
void Table::set_title(const std::string& title)  { _title = title; }
void Table::set_style(const TableStyle& s)       { _style = s; }
void Table::set_col_align(int col, int align)
{
	if (col >= 0 && col < MAX_COLS)
		_col_align[col] = align;
}

void Table::header1(const std::string& a)
{ std::string h[1] = {a}; header(h, 1); }
void Table::header2(const std::string& a, const std::string& b)
{ std::string h[2] = {a, b}; header(h, 2); }
void Table::header3(const std::string& a, const std::string& b, const std::string& c)
{ std::string h[3] = {a, b, c}; header(h, 3); }
void Table::row1(const std::string& a)
{ std::string r[1] = {a}; row(r, 1); }
void Table::row2(const std::string& a, const std::string& b)
{ std::string r[2] = {a, b}; row(r, 2); }
void Table::row3(const std::string& a, const std::string& b, const std::string& c)
{ std::string r[3] = {a, b, c}; row(r, 3); }

int Table::col_count() const  { return _ncols; }
int Table::row_count() const  { return _nrows; }
bool Table::has_header() const { return _has_header; }

/* ── rendering internals ───────────────────────────────────────────────── */

void Table::_compute_widths() const
{
	int* w = const_cast<int*>(_col_widths);
	for (int c = 0; c < _ncols; ++c)
	{
		w[c] = 0;
		if (_has_header)
		{
			int hw = TermUtils::vis_len(_headers[c]);
			if (hw > w[c]) w[c] = hw;
		}
		for (int r = 0; r < _nrows; ++r)
		{
			int rw = TermUtils::vis_len(_rows[r][c]);
			if (rw > w[c]) w[c] = rw;
		}
		w[c] += _style.pad * 2;
	}
}

std::string Table::_render_border_line(const char* left, const char* mid,
	const char* right, const char* horiz) const
{
	std::ostringstream out;
	out << TermUtils::apply_fg(_style.border_color);
	out << left;
	for (int c = 0; c < _ncols; ++c)
	{
		for (int i = 0; i < _col_widths[c]; ++i)
			out << horiz;
		if (c + 1 < _ncols)
			out << mid;
	}
	out << right;
	out << TermUtils::reset() << "\n";
	return out.str();
}

std::string Table::_align_cell(const std::string& text, int width, int align) const
{
	int vl = TermUtils::vis_len(text);
	int total_pad = width - vl;
	if (total_pad < 0) total_pad = 0;

	std::string out;
	if (align == ALIGN_CENTER)
	{
		int left = total_pad / 2;
		int right = total_pad - left;
		out = TermUtils::spaces(left) + text + TermUtils::spaces(right);
	}
	else if (align == ALIGN_RIGHT)
	{
		out = TermUtils::spaces(total_pad) + text;
	}
	else
	{
		out = text + TermUtils::spaces(total_pad);
	}
	return out;
}

std::string Table::_render_row(const std::string* cells, bool is_header) const
{
	std::ostringstream out;
	const char* v = _style.heavy ? H_V : L_V;

	out << TermUtils::apply_fg(_style.border_color) << v << TermUtils::reset();
	for (int c = 0; c < _ncols; ++c)
	{
		int content_w = _col_widths[c] - _style.pad * 2;
		if (content_w < 0) content_w = 0;

		std::string pad = TermUtils::spaces(_style.pad);

		if (is_header)
			out << TermUtils::apply_fg(_style.header_fg)
				<< TermUtils::apply_font(_style.header_font);
		else
			out << TermUtils::apply_fg(_style.cell_fg);

		out << pad << _align_cell(cells[c], content_w, _col_align[c]) << pad;
		out << TermUtils::reset();
		out << TermUtils::apply_fg(_style.border_color) << v << TermUtils::reset();
	}
	out << "\n";
	return out.str();
}

std::string Table::render() const
{
	if (_ncols <= 0) return "";

	_compute_widths();

	const char* h   = _style.heavy ? H_H : L_H;
	const char* tl  = _style.heavy ? H_TL : (_style.rounded ? R_TL : L_TL);
	const char* tr  = _style.heavy ? H_TR : (_style.rounded ? R_TR : L_TR);
	const char* bl  = _style.heavy ? H_BL : (_style.rounded ? R_BL : L_BL);
	const char* br  = _style.heavy ? H_BR : (_style.rounded ? R_BR : L_BR);
	const char* lt  = _style.heavy ? H_LT : L_LT;
	const char* rt  = _style.heavy ? H_RT : L_RT;
	const char* td  = _style.heavy ? H_TD : L_TD;
	const char* tu  = _style.heavy ? H_TU : L_TU;

	std::ostringstream out;

	/* title */
	if (!_title.empty())
		out << "\n " << _title << "\n";

	/* top border */
	out << _render_border_line(tl, td, tr, h);

	/* header */
	if (_has_header)
	{
		out << _render_row(_headers, true);
		out << _render_border_line(lt, _style.heavy ? "\xe2\x95\x8b" : "\xe2\x94\xbc", rt, h);
	}

	/* data rows */
	for (int r = 0; r < _nrows; ++r)
		out << _render_row(_rows[r], false);

	/* bottom border */
	out << _render_border_line(bl, tu, br, h);

	/* footer */
	if (!_footer.empty())
		out << " " << _footer << "\n";

	return out.str();
}

} /* namespace libcpp */
