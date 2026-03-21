/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   database.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:29:43 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_DATA_DATABASE_HPP
# define LIBCPP_DATA_DATABASE_HPP

# include <string>
# include <vector>
# include <map>
# include <fstream>

namespace libcpp
{
namespace data
{

/* ── ColumnType / Alignment enums ──────────────────────────────────────── */

namespace ColType
{
	enum Type { STRING = 0, INTEGER, DOUBLE, DATE, BOOLEAN };
}

namespace Align
{
	enum Type { LEFT = 0, CENTER, RIGHT };
}

/* ── DbColumn ──────────────────────────────────────────────────────────── */

class DbColumn
{
public:
	DbColumn();
	DbColumn(const std::string& name,
			 ColType::Type type = ColType::STRING,
			 Align::Type align = Align::LEFT);
	DbColumn(const DbColumn& o);
	DbColumn& operator=(const DbColumn& o);
	~DbColumn();

	const std::string& name() const;
	ColType::Type      type() const;
	Align::Type        alignment() const;
	std::size_t        width() const;
	void               set_width(std::size_t w);

	char        align_char() const;
	std::string format_value(const std::string& value) const;

private:
	std::string  _name;
	ColType::Type _type;
	Align::Type   _align;
	std::size_t   _width;
};

/* ── DbRow — map-based record ──────────────────────────────────────────── */

class DbRow
{
public:
	DbRow();
	DbRow(const DbRow& o);
	DbRow& operator=(const DbRow& o);
	~DbRow();

	void        set_value(const std::string& col, const std::string& val);
	std::string get_value(const std::string& col) const;

	const std::map<std::string, std::string>& data() const;

private:
	std::map<std::string, std::string> _data;
};

/* ── DbTable — column + row container ──────────────────────────────────── */

class DbTable
{
public:
	DbTable();
	DbTable(const DbTable& o);
	DbTable& operator=(const DbTable& o);
	~DbTable();

	void add_column(const DbColumn& col);
	void add_row(const DbRow& row);
	void clear_rows();

	const std::vector<DbColumn>& columns() const;
	std::vector<DbColumn>&       columns();
	const std::vector<DbRow>&    rows() const;
	std::vector<DbRow>&          rows();
	std::size_t column_count() const;
	std::size_t row_count() const;

	void calculate_widths();

private:
	std::vector<DbColumn> _columns;
	std::vector<DbRow>    _rows;
};

/* ── Database — high-level orchestrator ────────────────────────────────── */

class Database
{
public:
	Database();
	Database(const Database& o);
	Database& operator=(const Database& o);
	~Database();

	void load_csv(const std::string& path, bool has_header = true);

	void add_column(const std::string& name,
					ColType::Type type = ColType::STRING,
					Align::Type align = Align::LEFT);
	void add_row(const std::map<std::string, std::string>& data);

	DbTable&       table();
	const DbTable& table() const;

	std::vector<DbRow> where(const std::string& column,
							 const std::string& value) const;
	std::size_t count() const;

	void sort(const std::string& column, bool ascending = true);
	Database limit(std::size_t max_rows) const;

private:
	DbTable _table;
};

/* ── Export utilities ──────────────────────────────────────────────────── */

void export_csv(const DbTable& table, const std::string& path);
std::string export_html(DbTable& table, const std::string& title = "Data Table");
std::string export_markdown(DbTable& table);

/* ── Query helpers ─────────────────────────────────────────────────────── */

double db_sum(const DbTable& table, const std::string& column);
double db_average(const DbTable& table, const std::string& column);
double db_min(const DbTable& table, const std::string& column);
double db_max(const DbTable& table, const std::string& column);

DbTable db_sort_by(const DbTable& table, const std::string& column,
				   bool ascending = true);
DbTable db_limit(const DbTable& table, std::size_t max_rows);

} /* namespace data */
} /* namespace libcpp */

#endif /* LIBCPP_DATA_DATABASE_HPP */
