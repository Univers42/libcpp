/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   csv.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 22:06:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/data/csv.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <sstream>

namespace libcpp {
namespace data {

/* ── type inference ─────────────────────────────────────────────────────── */

static std::string to_lower(const std::string& s) {
  std::string r(s);
  for (std::string::size_type i = 0; i < r.size(); ++i)
    if (r[i] >= 'A' && r[i] <= 'Z') r[i] = static_cast<char>(r[i] + 32);
  return r;
}

static bool is_integer(const std::string& s) {
  if (s.empty()) return false;
  std::string::size_type start = (s[0] == '-' || s[0] == '+') ? 1 : 0;
  if (start >= s.size()) return false;
  for (std::string::size_type i = start; i < s.size(); ++i)
    if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
  return true;
}

static bool is_double(const std::string& s) {
  if (s.empty()) return false;
  char* end = 0;
  std::strtod(s.c_str(), &end);
  return end != 0 && end == s.c_str() + s.size();
}

static bool is_date(const std::string& s) {
  return (s.find('-') != std::string::npos ||
          s.find('/') != std::string::npos) &&
         s.size() >= 8 && s.size() <= 10;
}

DataType::Type infer_type(const std::string& value) {
  if (value.empty()) return DataType::EMPTY;
  std::string low = to_lower(value);
  if (low == "true" || low == "false" || low == "yes" || low == "no" ||
      low == "1" || low == "0")
    return DataType::BOOLEAN;
  if (is_integer(value)) return DataType::INTEGER;
  if (is_double(value)) return DataType::DOUBLE;
  if (is_date(value)) return DataType::DATE;
  return DataType::STRING;
}

/* ── CsvRow ─────────────────────────────────────────────────────────────── */

CsvRow::CsvRow() {}

CsvRow::CsvRow(const std::vector<std::string>& values) : _values(values) {}

CsvRow::CsvRow(const CsvRow& o) : _values(o._values), _col_map(o._col_map) {}

CsvRow& CsvRow::operator=(const CsvRow& o) {
  if (this != &o) {
    _values = o._values;
    _col_map = o._col_map;
  }
  return *this;
}

CsvRow::~CsvRow() {}

std::size_t CsvRow::size() const { return _values.size(); }
bool CsvRow::empty() const { return _values.empty(); }

const std::string& CsvRow::operator[](std::size_t i) const {
  static const std::string blank;
  return (i < _values.size()) ? _values[i] : blank;
}

std::string& CsvRow::operator[](std::size_t i) {
  if (i >= _values.size()) _values.resize(i + 1);
  return _values[i];
}

const std::string& CsvRow::get(const std::string& col) const {
  static const std::string blank;
  std::map<std::string, std::size_t>::const_iterator it = _col_map.find(col);
  return (it != _col_map.end() && it->second < _values.size())
             ? _values[it->second]
             : blank;
}

void CsvRow::set(const std::string& col, const std::string& val) {
  std::map<std::string, std::size_t>::const_iterator it = _col_map.find(col);
  if (it != _col_map.end()) {
    if (it->second >= _values.size()) _values.resize(it->second + 1);
    _values[it->second] = val;
  }
}

void CsvRow::set_column_map(const std::map<std::string, std::size_t>& m) {
  _col_map = m;
}

const std::vector<std::string>& CsvRow::values() const { return _values; }

int CsvRow::get_int(std::size_t i, int def) const {
  return (i < _values.size()) ? std::atoi(_values[i].c_str()) : def;
}

double CsvRow::get_double(std::size_t i, double def) const {
  return (i < _values.size()) ? std::atof(_values[i].c_str()) : def;
}

bool CsvRow::get_bool(std::size_t i, bool def) const {
  if (i >= _values.size()) return def;
  std::string low = to_lower(_values[i]);
  return (low == "true" || low == "yes" || low == "1");
}

/* ── CsvOptions ─────────────────────────────────────────────────────────── */

CsvOptions::CsvOptions()
    : delimiter(','),
      quote('"'),
      escape('\\'),
      has_header(true),
      trim_whitespace(true),
      skip_empty_lines(true),
      strict_quotes(false),
      skip_lines(0) {}

CsvOptions CsvOptions::rfc4180() {
  CsvOptions o;
  o.delimiter = ',';
  o.quote = '"';
  o.escape = '"';
  o.has_header = true;
  return o;
}

CsvOptions CsvOptions::tsv() {
  CsvOptions o;
  o.delimiter = '\t';
  return o;
}

/* ── CsvParser ──────────────────────────────────────────────────────────── */

CsvParser::CsvParser() : _opt() {}
CsvParser::CsvParser(const CsvOptions& opt) : _opt(opt) {}
CsvParser::CsvParser(const CsvParser& o) : _opt(o._opt) {}
CsvParser& CsvParser::operator=(const CsvParser& o) {
  if (this != &o) _opt = o._opt;
  return *this;
}
CsvParser::~CsvParser() {}

std::string CsvParser::trim(const std::string& s) {
  std::string::size_type start = s.find_first_not_of(" \t\r\n");
  if (start == std::string::npos) return "";
  std::string::size_type end = s.find_last_not_of(" \t\r\n");
  return s.substr(start, end - start + 1);
}

std::vector<std::string> CsvParser::parse_line(const std::string& line) const {
  std::vector<std::string> fields;
  std::string field;
  bool in_quotes = false;
  bool escape_next = false;

  for (std::string::size_type i = 0; i < line.size(); ++i) {
    char c = line[i];
    if (escape_next) {
      field += c;
      escape_next = false;
      continue;
    }
    if (c == _opt.escape && i + 1 < line.size()) {
      if (line[i + 1] == _opt.quote) {
        escape_next = true;
        continue;
      }
    }
    if (c == _opt.quote) {
      if (_opt.strict_quotes) {
        in_quotes = !in_quotes;
      } else {
        if (in_quotes && i + 1 < line.size() && line[i + 1] == _opt.quote) {
          field += _opt.quote;
          ++i;
        } else
          in_quotes = !in_quotes;
      }
      continue;
    }
    if (c == _opt.delimiter && !in_quotes) {
      if (_opt.trim_whitespace) field = trim(field);
      fields.push_back(field);
      field.clear();
      continue;
    }
    field += c;
  }
  if (_opt.trim_whitespace) field = trim(field);
  fields.push_back(field);
  return fields;
}

/* ── CsvDocument ────────────────────────────────────────────────────────── */

CsvDocument::CsvDocument() {}

CsvDocument::CsvDocument(const CsvDocument& o)
    : _rows(o._rows),
      _headers(o._headers),
      _col_map(o._col_map),
      _error(o._error) {}

CsvDocument& CsvDocument::operator=(const CsvDocument& o) {
  if (this != &o) {
    _rows = o._rows;
    _headers = o._headers;
    _col_map = o._col_map;
    _error = o._error;
  }
  return *this;
}

CsvDocument::~CsvDocument() {}

bool CsvDocument::load(const std::string& path, const CsvOptions& opt) {
  std::ifstream ifs(path.c_str());
  if (!ifs.is_open()) {
    _error = "Cannot open file: " + path;
    return false;
  }
  return load_stream(ifs, opt);
}

bool CsvDocument::load_stream(std::istream& in, const CsvOptions& opt) {
  _rows.clear();
  _headers.clear();
  _col_map.clear();
  _error.clear();
  CsvParser parser(opt);
  std::string line;

  for (std::size_t i = 0; i < opt.skip_lines; ++i)
    if (!std::getline(in, line)) return true;

  if (opt.has_header) {
    if (!std::getline(in, line)) {
      _error = "Empty file or missing header";
      return false;
    }
    _headers = parser.parse_line(line);
    for (std::size_t i = 0; i < _headers.size(); ++i) _col_map[_headers[i]] = i;
  }

  while (std::getline(in, line)) {
    if (opt.skip_empty_lines && line.empty()) continue;
    std::vector<std::string> fields = parser.parse_line(line);
    if (!opt.has_header && _headers.empty()) {
      for (std::size_t i = 0; i < fields.size(); ++i) {
        std::ostringstream oss;
        oss << i;
        std::string h = std::string("Column") + oss.str();
        _headers.push_back(h);
        _col_map[h] = i;
      }
    }
    CsvRow row(fields);
    row.set_column_map(_col_map);
    _rows.push_back(row);
  }
  return true;
}

bool CsvDocument::save(const std::string& path, const CsvOptions& opt) const {
  std::ofstream ofs(path.c_str());
  if (!ofs.is_open()) return false;
  return save_stream(ofs, opt);
}

bool CsvDocument::save_stream(std::ostream& out, const CsvOptions& opt) const {
  if (opt.has_header && !_headers.empty()) {
    for (std::size_t i = 0; i < _headers.size(); ++i) {
      out << _escape_field(_headers[i], opt);
      if (i + 1 < _headers.size()) out << opt.delimiter;
    }
    out << "\n";
  }
  for (std::size_t r = 0; r < _rows.size(); ++r) {
    for (std::size_t i = 0; i < _rows[r].size(); ++i) {
      out << _escape_field(_rows[r][i], opt);
      if (i + 1 < _rows[r].size()) out << opt.delimiter;
    }
    out << "\n";
  }
  return true;
}

std::string CsvDocument::to_string(const CsvOptions& opt) const {
  std::ostringstream oss;
  save_stream(oss, opt);
  return oss.str();
}

const std::vector<CsvRow>& CsvDocument::rows() const { return _rows; }
std::vector<CsvRow>& CsvDocument::rows() { return _rows; }
const std::vector<std::string>& CsvDocument::headers() const {
  return _headers;
}
std::size_t CsvDocument::row_count() const { return _rows.size(); }
std::size_t CsvDocument::column_count() const { return _headers.size(); }
const std::string& CsvDocument::error() const { return _error; }

int CsvDocument::get_column_index(const std::string& name) const {
  std::map<std::string, std::size_t>::const_iterator it = _col_map.find(name);
  return (it != _col_map.end()) ? static_cast<int>(it->second) : -1;
}

void CsvDocument::add_row(const CsvRow& r) { _rows.push_back(r); }

void CsvDocument::remove_row(std::size_t index) {
  if (index < _rows.size())
    _rows.erase(_rows.begin() + static_cast<std::ptrdiff_t>(index));
}

void CsvDocument::add_column(const std::string& name) {
  _headers.push_back(name);
  _col_map[name] = _headers.size() - 1;
}

void CsvDocument::set_headers(const std::vector<std::string>& h) {
  _headers = h;
  _col_map.clear();
  for (std::size_t i = 0; i < h.size(); ++i) _col_map[h[i]] = i;
}

void CsvDocument::clear() {
  _rows.clear();
  _headers.clear();
  _col_map.clear();
  _error.clear();
}

namespace {
struct RowCmpAsc {
  std::size_t _col;
  RowCmpAsc(std::size_t c) : _col(c) {}
  bool operator()(const CsvRow& a, const CsvRow& b) const {
    if (_col >= a.size() || _col >= b.size()) return false;
    return a[_col] < b[_col];
  }
};
struct RowCmpDesc {
  std::size_t _col;
  RowCmpDesc(std::size_t c) : _col(c) {}
  bool operator()(const CsvRow& a, const CsvRow& b) const {
    if (_col >= a.size() || _col >= b.size()) return false;
    return a[_col] > b[_col];
  }
};
}  // namespace

void CsvDocument::sort_by(const std::string& column, bool ascending) {
  int idx = get_column_index(column);
  if (idx < 0) return;
  std::size_t col = static_cast<std::size_t>(idx);
  if (ascending)
    std::sort(_rows.begin(), _rows.end(), RowCmpAsc(col));
  else
    std::sort(_rows.begin(), _rows.end(), RowCmpDesc(col));
}

std::string CsvDocument::_escape_field(const std::string& f,
                                       const CsvOptions& opt) const {
  bool needs = (f.find(opt.delimiter) != std::string::npos ||
                f.find(opt.quote) != std::string::npos ||
                f.find('\n') != std::string::npos);
  if (!needs) return f;
  std::string esc;
  esc += opt.quote;
  for (std::string::size_type i = 0; i < f.size(); ++i) {
    if (f[i] == opt.quote) {
      esc += opt.quote;
      esc += opt.quote;
    } else
      esc += f[i];
  }
  esc += opt.quote;
  return esc;
}

/* ── Free filter/aggregate helpers ──────────────────────────────────────── */

CsvDocument csv_where(const CsvDocument& doc, const std::string& column,
                      const std::string& value) {
  CsvDocument result;
  result.set_headers(doc.headers());
  int ci = doc.get_column_index(column);
  if (ci < 0) return result;
  for (std::size_t i = 0; i < doc.rows().size(); ++i)
    if (doc.rows()[i][static_cast<std::size_t>(ci)] == value)
      result.add_row(doc.rows()[i]);
  return result;
}

CsvDocument csv_limit(const CsvDocument& doc, std::size_t mx) {
  CsvDocument result;
  result.set_headers(doc.headers());
  for (std::size_t i = 0; i < doc.rows().size() && i < mx; ++i)
    result.add_row(doc.rows()[i]);
  return result;
}

CsvDocument csv_skip(const CsvDocument& doc, std::size_t n) {
  CsvDocument result;
  result.set_headers(doc.headers());
  for (std::size_t i = n; i < doc.rows().size(); ++i)
    result.add_row(doc.rows()[i]);
  return result;
}

double csv_sum(const CsvDocument& doc, const std::string& column) {
  int idx = doc.get_column_index(column);
  if (idx < 0) return 0.0;
  double total = 0.0;
  for (std::size_t i = 0; i < doc.rows().size(); ++i)
    total += doc.rows()[i].get_double(static_cast<std::size_t>(idx), 0.0);
  return total;
}

double csv_average(const CsvDocument& doc, const std::string& column) {
  if (doc.row_count() == 0) return 0.0;
  return csv_sum(doc, column) / static_cast<double>(doc.row_count());
}

double csv_min(const CsvDocument& doc, const std::string& column) {
  int idx = doc.get_column_index(column);
  if (idx < 0) return 0.0;
  bool found = false;
  double m = 0.0;
  for (std::size_t i = 0; i < doc.rows().size(); ++i) {
    double v = doc.rows()[i].get_double(static_cast<std::size_t>(idx), 0.0);
    if (!found || v < m) {
      m = v;
      found = true;
    }
  }
  return found ? m : 0.0;
}

double csv_max(const CsvDocument& doc, const std::string& column) {
  int idx = doc.get_column_index(column);
  if (idx < 0) return 0.0;
  bool found = false;
  double mx = 0.0;
  for (std::size_t i = 0; i < doc.rows().size(); ++i) {
    double v = doc.rows()[i].get_double(static_cast<std::size_t>(idx), 0.0);
    if (!found || v > mx) {
      mx = v;
      found = true;
    }
  }
  return found ? mx : 0.0;
}

} /* namespace data */
} /* namespace libcpp */
