/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   csv.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:09:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_DATA_CSV_HPP
# define LIBCPP_DATA_CSV_HPP

# include <string>
# include <vector>
# include <map>
# include <fstream>

namespace libcpp
{
namespace data
{

/* ── DataType ──────────────────────────────────────────────────────────── */

namespace DataType
{
	enum Type { STRING = 0, INTEGER, DOUBLE, BOOLEAN, DATE, EMPTY };
}

DataType::Type infer_type(const std::string& value);

/* ── CsvRow — a single row of field values ─────────────────────────────── */

class CsvRow
{
public:
	CsvRow();
	CsvRow(const std::vector<std::string>& values);
	CsvRow(const CsvRow& o);
	CsvRow& operator=(const CsvRow& o);
	~CsvRow();

	std::size_t size() const;
	bool        empty() const;

	const std::string& operator[](std::size_t index) const;
	std::string&       operator[](std::size_t index);

	const std::string& get(const std::string& col) const;
	void               set(const std::string& col, const std::string& val);

	void set_column_map(const std::map<std::string, std::size_t>& m);
	const std::vector<std::string>& values() const;

	int    get_int(std::size_t i, int def = 0) const;
	double get_double(std::size_t i, double def = 0.0) const;
	bool   get_bool(std::size_t i, bool def = false) const;

private:
	std::vector<std::string>            _values;
	std::map<std::string, std::size_t>  _col_map;
};

/* ── CsvOptions — parser configuration ─────────────────────────────────── */

struct CsvOptions
{
	char        delimiter;
	char        quote;
	char        escape;
	bool        has_header;
	bool        trim_whitespace;
	bool        skip_empty_lines;
	bool        strict_quotes;
	std::size_t skip_lines;

	CsvOptions();
	static CsvOptions rfc4180();
	static CsvOptions tsv();
};

/* ── CsvParser — line-level field splitter ─────────────────────────────── */

class CsvParser
{
public:
	CsvParser();
	CsvParser(const CsvOptions& opt);
	CsvParser(const CsvParser& o);
	CsvParser& operator=(const CsvParser& o);
	~CsvParser();

	std::vector<std::string> parse_line(const std::string& line) const;

	static std::string trim(const std::string& s);

private:
	CsvOptions _opt;
};

/* ── CsvDocument — full CSV in memory ──────────────────────────────────── */

class CsvDocument
{
public:
	CsvDocument();
	CsvDocument(const CsvDocument& o);
	CsvDocument& operator=(const CsvDocument& o);
	~CsvDocument();

	bool load(const std::string& path, const CsvOptions& opt = CsvOptions());
	bool load_stream(std::istream& in, const CsvOptions& opt = CsvOptions());
	bool save(const std::string& path, const CsvOptions& opt = CsvOptions()) const;
	bool save_stream(std::ostream& out, const CsvOptions& opt = CsvOptions()) const;
	std::string to_string(const CsvOptions& opt = CsvOptions()) const;

	/* Accessors */
	const std::vector<CsvRow>&         rows() const;
	std::vector<CsvRow>&               rows();
	const std::vector<std::string>&    headers() const;
	std::size_t                        row_count() const;
	std::size_t                        column_count() const;
	int  get_column_index(const std::string& name) const;
	const std::string& error() const;

	/* Mutation */
	void add_row(const CsvRow& r);
	void add_column(const std::string& name);
	void set_headers(const std::vector<std::string>& h);
	void clear();

private:
	std::vector<CsvRow>                _rows;
	std::vector<std::string>           _headers;
	std::map<std::string, std::size_t> _col_map;
	std::string                        _error;

	std::string _escape_field(const std::string& f, const CsvOptions& opt) const;
};

/* ── Free filter/aggregate helpers ─────────────────────────────────────── */

CsvDocument csv_where(const CsvDocument& doc, const std::string& column,
					  const std::string& value);
CsvDocument csv_limit(const CsvDocument& doc, std::size_t max_rows);
CsvDocument csv_skip(const CsvDocument& doc, std::size_t n);

double csv_sum(const CsvDocument& doc, const std::string& column);
double csv_average(const CsvDocument& doc, const std::string& column);
double csv_min(const CsvDocument& doc, const std::string& column);
double csv_max(const CsvDocument& doc, const std::string& column);

} /* namespace data */
} /* namespace libcpp */

#endif /* LIBCPP_DATA_CSV_HPP */
