/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/22 15:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_TERM_TABLE_HPP
# define LIBCPP_TERM_TABLE_HPP

# include <string>
# include <vector>
# include "libcpp/term/color.hpp"

namespace libcpp
{

/* ======================================================================
 *  BorderPreset — predefined box-drawing character sets
 * ====================================================================== */
enum BorderPreset
{
	BORDER_LIGHT   = 0,
	BORDER_HEAVY   = 1,
	BORDER_DOUBLE  = 2,
	BORDER_ROUNDED = 3,
	BORDER_ASCII   = 4,
	BORDER_NONE    = 5
};

/* ======================================================================
 *  CellStyle — per-cell/row override for focus/highlight
 * ====================================================================== */
struct CellStyle
{
	Srgb fg;
	Srgb bg;
	int  font;
	bool has_fg;
	bool has_bg;
	bool has_font;

	CellStyle();
	CellStyle(const CellStyle& o);
	CellStyle& operator=(const CellStyle& o);
	~CellStyle();
};

/* ======================================================================
 *  TableStyle — full customization for table rendering
 * ====================================================================== */
struct TableStyle
{
	/* border */
	BorderPreset border;
	Srgb         border_color;

	/* header */
	Srgb         header_fg;
	Srgb         header_bg;
	int          header_font;
	bool         header_separator;
	bool         show_header;
	bool         header_has_bg;

	/* cells */
	Srgb         cell_fg;
	Srgb         cell_bg;
	Srgb         cell_alt_bg;
	bool         zebra;
	bool         cell_has_bg;

	/* title & footer */
	Srgb         title_fg;
	Srgb         title_bg;
	int          title_font;
	bool         title_has_bg;
	Srgb         footer_fg;
	int          footer_font;

	/* layout */
	int          pad;
	int          outer_margin;
	int          max_col_width;
	bool         show_row_numbers;
	bool         row_separator;
	bool         compact;

	/* focus / highlight */
	CellStyle    focus_style;

	/* ── OCF ─────────────────────────────────────────────────── */
	TableStyle();
	TableStyle(const TableStyle& other);
	TableStyle& operator=(const TableStyle& other);
	~TableStyle();

	/* ── preset builders ─────────────────────────────────────── */
	static TableStyle minimal();
	static TableStyle modern();
	static TableStyle heavy_style();
	static TableStyle matrix();
	static TableStyle postman();
};

/* ======================================================================
 *  Table — powerful Unicode table renderer
 * ====================================================================== */
class Table
{
public:
	static const int MAX_COLS = 16;
	static const int MAX_ROWS = 256;

	/* ── OCF ───────────────────────────────────────────────────── */
	Table();
	explicit Table(int ncols);
	Table(const Table& other);
	Table& operator=(const Table& other);
	~Table();

	/* ── building ──────────────────────────────────────────────── */
	void set_cols(int n);
	void header(const std::string* cells, int n);
	void row(const std::string* cells, int n);
	void set_footer(const std::string& text);
	void set_title(const std::string& title);
	void set_style(const TableStyle& s);
	bool has_custom_style() const;
	void set_col_align(int col, int align);
	void set_col_width(int col, int width);
	void highlight_row(int row);
	void highlight_last_row();

	/* convenience overloads 1-6 cols */
	void header1(const std::string& a);
	void header2(const std::string& a, const std::string& b);
	void header3(const std::string& a, const std::string& b, const std::string& c);
	void header4(const std::string& a, const std::string& b,
	             const std::string& c, const std::string& d);
	void header5(const std::string& a, const std::string& b,
	             const std::string& c, const std::string& d,
	             const std::string& e);
	void header6(const std::string& a, const std::string& b,
	             const std::string& c, const std::string& d,
	             const std::string& e, const std::string& f);

	void row1(const std::string& a);
	void row2(const std::string& a, const std::string& b);
	void row3(const std::string& a, const std::string& b, const std::string& c);
	void row4(const std::string& a, const std::string& b,
	          const std::string& c, const std::string& d);
	void row5(const std::string& a, const std::string& b,
	          const std::string& c, const std::string& d,
	          const std::string& e);
	void row6(const std::string& a, const std::string& b,
	          const std::string& c, const std::string& d,
	          const std::string& e, const std::string& f);

	/* ── render ────────────────────────────────────────────────── */
	std::string render() const;
	std::string to_csv(char delim = ',') const;

	/* ── info ──────────────────────────────────────────────────── */
	int col_count() const;
	int row_count() const;
	bool has_header() const;
	void clear();

private:
	int         _ncols;
	int         _nrows;
	bool        _has_header;
	bool        _has_custom_style;
	std::string _headers[MAX_COLS];
	std::string _rows[MAX_ROWS][MAX_COLS];
	int         _col_align[MAX_COLS];
	int         _col_widths[MAX_COLS];
	int         _forced_widths[MAX_COLS];
	bool        _highlight[MAX_ROWS];
	std::string _footer;
	std::string _title;
	TableStyle  _style;

	struct BoxChars
	{
		const char* h;
		const char* v;
		const char* tl;
		const char* tr;
		const char* bl;
		const char* br;
		const char* lt;
		const char* rt;
		const char* td;
		const char* tu;
		const char* cross;
	};

	void _compute_widths() const;
	BoxChars _resolve_box() const;
	std::string _render_hline(const BoxChars& bc,
		const char* left, const char* mid, const char* right) const;
	std::string _render_row(const BoxChars& bc,
		const std::string* cells, bool is_header, int row_idx) const;
	std::string _render_title_bar(const BoxChars& bc) const;
	std::string _render_footer_bar(const BoxChars& bc) const;
	std::string _align_cell(const std::string& text, int width, int align) const;
	std::string _truncate(const std::string& text, int max_w) const;
	std::string _margin() const;
	int _total_width() const;
};

} /* namespace libcpp */

#endif /* LIBCPP_TERM_TABLE_HPP */
