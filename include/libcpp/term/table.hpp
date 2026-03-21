/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   table.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 20:43:53 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_TERM_TABLE_HPP
# define LIBCPP_TERM_TABLE_HPP

# include <string>
# include <vector>
# include "libcpp/term/color.hpp"

namespace libcpp
{

/*
** TableStyle — configuration for table rendering
*/
struct TableStyle
{
	bool heavy;
	bool rounded;
	Srgb border_color;
	Srgb header_fg;
	Srgb header_bg;
	int  header_font;
	Srgb cell_fg;
	int  pad;
	bool show_row_numbers;

	TableStyle();
	TableStyle(const TableStyle& other);
	TableStyle& operator=(const TableStyle& other);
	~TableStyle();
};

/*
** Table — generic Unicode table renderer
** Orthodox Canonical Form compliant.
**
** Limits: MAX_COLS=16, MAX_ROWS=256
*/
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
	void set_col_align(int col, int align);

	/* convenience overloads for 1-3 columns */
	void header1(const std::string& a);
	void header2(const std::string& a, const std::string& b);
	void header3(const std::string& a, const std::string& b, const std::string& c);
	void row1(const std::string& a);
	void row2(const std::string& a, const std::string& b);
	void row3(const std::string& a, const std::string& b, const std::string& c);

	/* ── render ────────────────────────────────────────────────── */
	std::string render() const;
	std::string to_csv(char delim) const;

	/* ── info ──────────────────────────────────────────────────── */
	int col_count() const;
	int row_count() const;
	bool has_header() const;
	void clear();

private:
	int         _ncols;
	int         _nrows;
	bool        _has_header;
	std::string _headers[MAX_COLS];
	std::string _rows[MAX_ROWS][MAX_COLS];
	int         _col_align[MAX_COLS];
	int         _col_widths[MAX_COLS];
	std::string _footer;
	std::string _title;
	TableStyle  _style;

	void _compute_widths() const;
	std::string _render_border_line(const char* left, const char* mid,
		const char* right, const char* horiz) const;
	std::string _render_row(const std::string* cells, bool is_header) const;
	std::string _align_cell(const std::string& text, int width, int align) const;
};

} /* namespace libcpp */

#endif /* LIBCPP_TERM_TABLE_HPP */
