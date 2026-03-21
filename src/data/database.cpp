/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   database.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:29:42 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/data/database.hpp"
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <stdexcept>

namespace libcpp
{
namespace data
{

/* ── helpers ────────────────────────────────────────────────────────────── */

static std::string s_trim(const std::string& s)
{
	std::string::size_type a = s.find_first_not_of(" \t\r\n");
	if (a == std::string::npos) return "";
	std::string::size_type b = s.find_last_not_of(" \t\r\n");
	return s.substr(a, b - a + 1);
}

static std::size_t vis_width(const std::string& s)
{
	/* simplified: counts bytes, ignoring multi-byte */
	return s.size();
}

/* ── DbColumn ───────────────────────────────────────────────────────────── */

DbColumn::DbColumn()
	: _name(), _type(ColType::STRING), _align(Align::LEFT), _width(0) {}

DbColumn::DbColumn(const std::string& name, ColType::Type type, Align::Type align)
	: _name(name), _type(type), _align(align), _width(0) {}

DbColumn::DbColumn(const DbColumn& o)
	: _name(o._name), _type(o._type), _align(o._align), _width(o._width) {}

DbColumn& DbColumn::operator=(const DbColumn& o)
{
	if (this != &o)
	{
		_name = o._name;
		_type = o._type;
		_align = o._align;
		_width = o._width;
	}
	return *this;
}

DbColumn::~DbColumn() {}

const std::string& DbColumn::name() const   { return _name; }
ColType::Type      DbColumn::type() const    { return _type; }
Align::Type        DbColumn::alignment() const { return _align; }
std::size_t        DbColumn::width() const   { return _width; }
void               DbColumn::set_width(std::size_t w) { _width = w; }

char DbColumn::align_char() const
{
	if (_align == Align::CENTER) return 'c';
	if (_align == Align::RIGHT)  return 'r';
	return 'l';
}

std::string DbColumn::format_value(const std::string& value) const
{
	if (_type == ColType::BOOLEAN)
		return (value == "1" || value == "true") ? "yes" : "no";
	return value;
}

/* ── DbRow ──────────────────────────────────────────────────────────────── */

DbRow::DbRow() {}
DbRow::DbRow(const DbRow& o) : _data(o._data) {}
DbRow& DbRow::operator=(const DbRow& o)
{ if (this != &o) _data = o._data; return *this; }
DbRow::~DbRow() {}

void DbRow::set_value(const std::string& col, const std::string& val)
{ _data[col] = val; }

std::string DbRow::get_value(const std::string& col) const
{
	std::map<std::string, std::string>::const_iterator it = _data.find(col);
	return (it != _data.end()) ? it->second : "";
}

bool DbRow::has_key(const std::string& col) const
{ return _data.find(col) != _data.end(); }

const std::map<std::string, std::string>& DbRow::data() const { return _data; }

/* ── DbTable ────────────────────────────────────────────────────────────── */

DbTable::DbTable() {}
DbTable::DbTable(const DbTable& o) : _columns(o._columns), _rows(o._rows) {}
DbTable& DbTable::operator=(const DbTable& o)
{
	if (this != &o) { _columns = o._columns; _rows = o._rows; }
	return *this;
}
DbTable::~DbTable() {}

void DbTable::add_column(const DbColumn& c) { _columns.push_back(c); }
void DbTable::add_row(const DbRow& r)       { _rows.push_back(r); }
void DbTable::remove_row(std::size_t index)
{
	if (index < _rows.size())
		_rows.erase(_rows.begin() + static_cast<std::ptrdiff_t>(index));
}
void DbTable::clear_rows()                   { _rows.clear(); }

const std::vector<DbColumn>& DbTable::columns() const { return _columns; }
std::vector<DbColumn>&       DbTable::columns()       { return _columns; }
const std::vector<DbRow>&    DbTable::rows() const     { return _rows; }
std::vector<DbRow>&          DbTable::rows()           { return _rows; }
std::size_t DbTable::column_count() const { return _columns.size(); }
std::size_t DbTable::row_count() const    { return _rows.size(); }

void DbTable::calculate_widths()
{
	for (std::size_t c = 0; c < _columns.size(); ++c)
	{
		std::size_t mx = vis_width(_columns[c].name());
		for (std::size_t r = 0; r < _rows.size(); ++r)
		{
			std::string val = _columns[c].format_value(
				_rows[r].get_value(_columns[c].name()));
			std::size_t w = vis_width(val);
			if (w > mx) mx = w;
		}
		_columns[c].set_width(mx);
	}
}

/* ── Database ───────────────────────────────────────────────────────────── */

Database::Database() {}
Database::Database(const Database& o) : _table(o._table) {}
Database& Database::operator=(const Database& o)
{ if (this != &o) _table = o._table; return *this; }
Database::~Database() {}

void Database::load_csv(const std::string& path, bool has_header)
{
	std::ifstream ifs(path.c_str());
	if (!ifs.is_open())
		throw std::runtime_error("Cannot open file: " + path);

	std::string line;
	bool first = true;
	std::vector<std::string> headers;

	while (std::getline(ifs, line))
	{
		/* Simple CSV split */
		std::vector<std::string> fields;
		std::string field;
		bool in_q = false;
		for (std::string::size_type i = 0; i < line.size(); ++i)
		{
			char c = line[i];
			if (c == '"')
			{
				if (in_q && i + 1 < line.size() && line[i + 1] == '"')
				{ field += '"'; ++i; }
				else in_q = !in_q;
			}
			else if (c == ',' && !in_q)
			{ fields.push_back(s_trim(field)); field.clear(); }
			else
				field += c;
		}
		fields.push_back(s_trim(field));

		if (first && has_header)
		{
			headers = fields;
			for (std::size_t i = 0; i < headers.size(); ++i)
				_table.add_column(DbColumn(headers[i]));
			first = false;
		}
		else
		{
			if (headers.empty())
			{
				for (std::size_t i = 0; i < fields.size(); ++i)
				{
					std::ostringstream oss;
					oss << (i + 1);
					std::string h = std::string("Column") + oss.str();
					headers.push_back(h);
					_table.add_column(DbColumn(h));
				}
			}
			DbRow row;
			for (std::size_t i = 0; i < fields.size() && i < headers.size(); ++i)
				row.set_value(headers[i], fields[i]);
			_table.add_row(row);
			first = false;
		}
	}
}

void Database::add_column(const std::string& name, ColType::Type type,
						  Align::Type align)
{
	_table.add_column(DbColumn(name, type, align));
}

void Database::add_row(const std::map<std::string, std::string>& data)
{
	DbRow row;
	for (std::map<std::string, std::string>::const_iterator it = data.begin();
		 it != data.end(); ++it)
		row.set_value(it->first, it->second);
	_table.add_row(row);
}

void Database::remove_row(std::size_t index) { _table.remove_row(index); }

void Database::update_cell(std::size_t row, const std::string& column,
					   const std::string& value)
{
	std::vector<DbRow>& rows = _table.rows();
	if (row < rows.size())
		rows[row].set_value(column, value);
}

DbTable&       Database::table()       { return _table; }
const DbTable& Database::table() const { return _table; }

std::vector<DbRow> Database::where(const std::string& column,
								   const std::string& value) const
{
	std::vector<DbRow> results;
	const std::vector<DbRow>& rows = _table.rows();
	for (std::size_t i = 0; i < rows.size(); ++i)
		if (rows[i].get_value(column) == value)
			results.push_back(rows[i]);
	return results;
}

std::size_t Database::count() const { return _table.row_count(); }

void Database::sort(const std::string& column, bool ascending)
{
	_table = db_sort_by(_table, column, ascending);
}

Database Database::limit(std::size_t max_rows) const
{
	Database result;
	const std::vector<DbColumn>& cols = _table.columns();
	for (std::size_t i = 0; i < cols.size(); ++i)
		result.add_column(cols[i].name(), cols[i].type(), cols[i].alignment());
	const std::vector<DbRow>& all = _table.rows();
	std::size_t n = all.size() < max_rows ? all.size() : max_rows;
	for (std::size_t i = 0; i < n; ++i)
	{
		std::map<std::string, std::string> data;
		for (std::size_t c = 0; c < cols.size(); ++c)
			data[cols[c].name()] = all[i].get_value(cols[c].name());
		result.add_row(data);
	}
	return result;
}

/* ── Export utilities ───────────────────────────────────────────────────── */

void export_csv(const DbTable& table, const std::string& path)
{
	std::ofstream ofs(path.c_str());
	if (!ofs.is_open())
		throw std::runtime_error(std::string("Cannot create file: ") + path);

	const std::vector<DbColumn>& cols = table.columns();
	for (std::size_t i = 0; i < cols.size(); ++i)
	{
		ofs << cols[i].name();
		if (i + 1 < cols.size()) ofs << ",";
	}
	ofs << "\n";

	const std::vector<DbRow>& rows = table.rows();
	for (std::size_t r = 0; r < rows.size(); ++r)
	{
		for (std::size_t i = 0; i < cols.size(); ++i)
		{
			std::string val = rows[r].get_value(cols[i].name());
			if (val.find(',') != std::string::npos
				|| val.find('"') != std::string::npos)
				val = "\"" + val + "\"";
			ofs << val;
			if (i + 1 < cols.size()) ofs << ",";
		}
		ofs << "\n";
	}
}

std::string export_html(DbTable& table, const std::string& title)
{
	std::ostringstream html;
	html << "<!DOCTYPE html>\n<html>\n<head>\n";
	html << "<title>" << title << "</title>\n";
	html << "<style>\n";
	html << "table { border-collapse: collapse; width: 100%; }\n";
	html << "th, td { border: 1px solid #ddd; padding: 12px; }\n";
	html << "th { background-color: #4CAF50; color: white; }\n";
	html << "</style>\n</head>\n<body>\n";
	html << "<h1>" << title << "</h1>\n<table>\n<thead>\n<tr>\n";

	const std::vector<DbColumn>& cols = table.columns();
	for (std::size_t i = 0; i < cols.size(); ++i)
		html << "<th>" << cols[i].name() << "</th>\n";
	html << "</tr>\n</thead>\n<tbody>\n";

	const std::vector<DbRow>& rows = table.rows();
	for (std::size_t r = 0; r < rows.size(); ++r)
	{
		html << "<tr>\n";
		for (std::size_t c = 0; c < cols.size(); ++c)
			html << "<td>" << rows[r].get_value(cols[c].name()) << "</td>\n";
		html << "</tr>\n";
	}
	html << "</tbody>\n</table>\n</body>\n</html>";
	return html.str();
}

std::string export_markdown(DbTable& table)
{
	table.calculate_widths();
	std::ostringstream md;
	const std::vector<DbColumn>& cols = table.columns();

	/* Header */
	for (std::size_t i = 0; i < cols.size(); ++i)
		md << "| " << cols[i].name() << " ";
	md << "|\n";

	/* Separator */
	for (std::size_t i = 0; i < cols.size(); ++i)
	{
		std::size_t w = cols[i].width() + 2;
		md << "|";
		for (std::size_t j = 0; j < w; ++j)
			md << "-";
	}
	md << "|\n";

	/* Rows */
	const std::vector<DbRow>& rows = table.rows();
	for (std::size_t r = 0; r < rows.size(); ++r)
	{
		for (std::size_t c = 0; c < cols.size(); ++c)
			md << "| " << rows[r].get_value(cols[c].name()) << " ";
		md << "|\n";
	}
	return md.str();
}

/* ── Query helpers ──────────────────────────────────────────────────────── */

double db_sum(const DbTable& table, const std::string& column)
{
	double total = 0.0;
	const std::vector<DbRow>& rows = table.rows();
	for (std::size_t i = 0; i < rows.size(); ++i)
		total += std::atof(rows[i].get_value(column).c_str());
	return total;
}

double db_average(const DbTable& table, const std::string& column)
{
	if (table.row_count() == 0) return 0.0;
	return db_sum(table, column) / static_cast<double>(table.row_count());
}

double db_min(const DbTable& table, const std::string& column)
{
	bool found = false;
	double m = 0.0;
	const std::vector<DbRow>& rows = table.rows();
	for (std::size_t i = 0; i < rows.size(); ++i)
	{
		double v = std::atof(rows[i].get_value(column).c_str());
		if (!found || v < m) { m = v; found = true; }
	}
	return found ? m : 0.0;
}

double db_max(const DbTable& table, const std::string& column)
{
	bool found = false;
	double mx = 0.0;
	const std::vector<DbRow>& rows = table.rows();
	for (std::size_t i = 0; i < rows.size(); ++i)
	{
		double v = std::atof(rows[i].get_value(column).c_str());
		if (!found || v > mx) { mx = v; found = true; }
	}
	return found ? mx : 0.0;
}

/* ── sort comparator (namespace scope for C++98) ────────────────────────── */

struct DbRowComparator
{
	std::string column;
	bool ascending;
	DbRowComparator(const std::string& c, bool a) : column(c), ascending(a) {}
	bool operator()(const DbRow& a, const DbRow& b) const
	{
		std::string va = a.get_value(column);
		std::string vb = b.get_value(column);
		return ascending ? (va < vb) : (va > vb);
	}
};

DbTable db_sort_by(const DbTable& table, const std::string& column,
				   bool ascending)
{
	DbTable result;
	const std::vector<DbColumn>& cols = table.columns();
	for (std::size_t i = 0; i < cols.size(); ++i)
		result.add_column(cols[i]);
	std::vector<DbRow> rows = table.rows();
	std::sort(rows.begin(), rows.end(), DbRowComparator(column, ascending));
	for (std::size_t i = 0; i < rows.size(); ++i)
		result.add_row(rows[i]);
	return result;
}

DbTable db_limit(const DbTable& table, std::size_t max_rows)
{
	DbTable result;
	const std::vector<DbColumn>& cols = table.columns();
	for (std::size_t i = 0; i < cols.size(); ++i)
		result.add_column(cols[i]);
	const std::vector<DbRow>& rows = table.rows();
	for (std::size_t i = 0; i < rows.size() && i < max_rows; ++i)
		result.add_row(rows[i]);
	return result;
}

} /* namespace data */
} /* namespace libcpp */
