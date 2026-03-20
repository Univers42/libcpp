/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   csv.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 01:58:35 by marvin            #+#    #+#             */
/*   Updated: 2025/12/24 01:58:35 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CSV_HPP
#define CSV_HPP

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <cstring>

// ============================================================================
// TYPE DETECTION AND INFERENCE
// ============================================================================

namespace CSV {

namespace DataType {
    enum Type { STRING = 0, INTEGER = 1, DOUBLE = 2, BOOLEAN = 3, DATE = 4, EMPTY = 5 };
}

class TypeInference {
public:
    static DataType::Type inferType(const std::string& value) {
        if (value.empty()) return DataType::EMPTY;
        std::string lower = toLower(value);
        if (lower == "true" || lower == "false" ||
            lower == "yes" || lower == "no" ||
            lower == "1" || lower == "0") {
            return DataType::BOOLEAN;
        }
        if (isInteger(value)) return DataType::INTEGER;
        if (isDouble(value)) return DataType::DOUBLE;
        if (isDate(value)) return DataType::DATE;
        return DataType::STRING;
    }
    
    static bool isInteger(const std::string& s) {
        if (s.empty()) return false;
        size_t start = (s[0] == '-' || s[0] == '+') ? 1 : 0;
        if (start >= s.length()) return false;
        for (size_t i = start; i < s.length(); ++i) {
            if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
        }
        return true;
    }
    
    static bool isDouble(const std::string& s) {
        if (s.empty()) return false;
        char* endptr = NULL;
        double v = strtod(s.c_str(), &endptr);
        (void)v;
        return (endptr != NULL) && (endptr == s.c_str() + s.length());
    }
    
    static bool isDate(const std::string& s) {
        return (s.find('-') != std::string::npos || s.find('/') != std::string::npos)
               && s.length() >= 8 && s.length() <= 10;
    }

private:
    static std::string toLower(const std::string& s) {
        std::string result = s;
        for (size_t i = 0; i < result.size(); ++i) result[i] = static_cast<char>(::tolower(result[i]));
        return result;
    }
};

// ============================================================================
// CSV ROW
// ============================================================================

class Row {
public:
    Row() {}
    explicit Row(const std::vector<std::string>& values) : _values(values) {}
    
    size_t size() const { return _values.size(); }
    bool empty() const { return _values.empty(); }
    
    const std::string& operator[](size_t index) const {
        static const std::string empty;
        return (index < _values.size()) ? _values[index] : empty;
    }
    
    std::string& operator[](size_t index) {
        if (index >= _values.size()) _values.resize(index + 1);
        return _values[index];
    }
    
    const std::string& get(const std::string& columnName) const {
        std::map<std::string, size_t>::const_iterator it = _columnMap.find(columnName);
        static const std::string empty;
        return (it != _columnMap.end()) ? _values[it->second] : empty;
    }
    
    void set(const std::string& columnName, const std::string& value) {
        std::map<std::string, size_t>::const_iterator it = _columnMap.find(columnName);
        if (it != _columnMap.end()) _values[it->second] = value;
    }
    
    void setColumnMap(const std::map<std::string, size_t>& columnMap) { _columnMap = columnMap; }
    const std::vector<std::string>& values() const { return _values; }
    
    int getInt(size_t index, int defaultValue = 0) const {
        if (index >= _values.size()) return defaultValue;
        return atoi(_values[index].c_str());
    }
    
    double getDouble(size_t index, double defaultValue = 0.0) const {
        if (index >= _values.size()) return defaultValue;
        return atof(_values[index].c_str());
    }
    
    bool getBool(size_t index, bool defaultValue = false) const {
        if (index >= _values.size()) return defaultValue;
        std::string lower = _values[index];
        for (size_t i = 0; i < lower.size(); ++i) lower[i] = static_cast<char>(::tolower(lower[i]));
        return (lower == "true" || lower == "yes" || lower == "1");
    }

private:
    std::vector<std::string> _values;
    std::map<std::string, size_t> _columnMap;
};

// Add namespace-scope comparator so std::sort can accept it (C++98-safe)
struct RowComparator {
	int idx;
	bool asc;
	RowComparator(int i = 0, bool a = true) : idx(i), asc(a) {}
	bool operator()(const Row& a, const Row& b) const {
		const std::string& va = (a.size() > (size_t)idx) ? a[idx] : std::string();
		const std::string& vb = (b.size() > (size_t)idx) ? b[idx] : std::string();
		return asc ? (va < vb) : (va > vb);
	}
};

// ============================================================================
// CSV PARSER - Options (C++98-safe)
// ============================================================================

class Parser {
public:
    struct Options {
        char delimiter;
        char quote;
        char escape;
        bool hasHeader;
        bool trimWhitespace;
        bool skipEmptyLines;
        bool strictQuotes;
        size_t skipLines;

        Options()
            : delimiter(','), quote('"'), escape('\\'), hasHeader(true),
              trimWhitespace(true), skipEmptyLines(true), strictQuotes(false), skipLines(0) {}
        
        static Options RFC4180() {
            Options opt;
            opt.delimiter = ',';
            opt.quote = '"';
            opt.escape = '"';
            opt.hasHeader = true;
            return opt;
        }
        
        static Options TSV() {
            Options opt;
            opt.delimiter = '\t';
            return opt;
        }
        
        static Options Excel() {
            Options opt;
            opt.delimiter = ',';
            opt.quote = '"';
            opt.escape = '"';
            opt.hasHeader = true;
            return opt;
        }
    };
    
    Options options;
    Parser(const Options& opt = Options()) : options(opt) {}
    
    std::vector<std::string> parseLine(const std::string& line) const {
        std::vector<std::string> fields;
        std::string field;
        bool inQuotes = false;
        bool escapeNext = false;
        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];
            if (escapeNext) { field += c; escapeNext = false; continue; }
            if (c == options.escape && i + 1 < line.length()) {
                if (line[i + 1] == options.quote) { escapeNext = true; continue; }
            }
            if (c == options.quote) {
                if (options.strictQuotes) { inQuotes = !inQuotes; }
                else {
                    if (inQuotes && i + 1 < line.length() && line[i + 1] == options.quote) { field += options.quote; ++i; }
                    else inQuotes = !inQuotes;
                }
                continue;
            }
            if (c == options.delimiter && !inQuotes) {
                if (options.trimWhitespace) field = trim(field);
                fields.push_back(field); field.clear(); continue;
            }
            field += c;
        }
        if (options.trimWhitespace) field = trim(field);
        fields.push_back(field);
        return fields;
    }
    
    static std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }
};

// ============================================================================
// CSV DOCUMENT - main
// ============================================================================

class Document {
public:
    Document() {}
    
    bool load(const std::string& filename, const Parser::Options& options = Parser::Options()) {
        std::ifstream file;
        file.open(filename.c_str());
        if (!file.is_open()) { _error = "Cannot open file: " + filename; return false; }
        return loadFromStream(file, options);
    }
    
    bool loadFromStream(std::istream& stream, const Parser::Options& options = Parser::Options()) {
        _rows.clear(); _headers.clear(); _columnMap.clear(); _error.clear();
        Parser parser(options);
        std::string line;
        size_t lineNum = 0;
        for (size_t i = 0; i < options.skipLines; ++i) { if (!std::getline(stream, line)) return true; }
        if (options.hasHeader) {
            if (!std::getline(stream, line)) { _error = "Empty file or missing header"; return false; }
            _headers = parser.parseLine(line);
            for (size_t i = 0; i < _headers.size(); ++i) _columnMap[_headers[i]] = i;
        }
        while (std::getline(stream, line)) {
            ++lineNum;
            if (options.skipEmptyLines && line.empty()) continue;
            std::vector<std::string> fields = parser.parseLine(line);
            if (!options.hasHeader && _headers.empty()) {
                for (size_t i = 0; i < fields.size(); ++i) {
                    std::ostringstream oss; oss << (i);
                    std::string h = std::string("Column") + oss.str();
                    _headers.push_back(h);
                    _columnMap[h] = i;
                }
            }
            Row row(fields);
            row.setColumnMap(_columnMap);
            _rows.push_back(row);
        }
        return true;
    }
    
    bool save(const std::string& filename, const Parser::Options& options = Parser::Options()) const {
        std::ofstream file;
        file.open(filename.c_str());
        if (!file.is_open()) return false;
        return saveToStream(file, options);
    }
    
    bool saveToStream(std::ostream& stream, const Parser::Options& options = Parser::Options()) const {
        if (options.hasHeader && !_headers.empty()) {
            for (size_t i = 0; i < _headers.size(); ++i) {
                stream << escapeField(_headers[i], options);
                if (i < _headers.size() - 1) stream << options.delimiter;
            }
            stream << "\n";
        }
        for (size_t ri = 0; ri < _rows.size(); ++ri) {
            const Row& row = _rows[ri];
            for (size_t i = 0; i < row.size(); ++i) {
                stream << escapeField(row[i], options);
                if (i < row.size() - 1) stream << options.delimiter;
            }
            stream << "\n";
        }
        return true;
    }

    // Accessors
    const std::vector<Row>& rows() const { return _rows; }
    std::vector<Row>& rows() { return _rows; }
    const std::vector<std::string>& headers() const { return _headers; }
    size_t rowCount() const { return _rows.size(); }
    size_t columnCount() const { return _headers.size(); }
    const std::string& error() const { return _error; }

    int getColumnIndex(const std::string& name) const {
        std::map<std::string, size_t>::const_iterator it = _columnMap.find(name);
        return (it != _columnMap.end()) ? static_cast<int>(it->second) : -1;
    }

    void addRow(const Row& row) { _rows.push_back(row); }
    void addColumn(const std::string& name) { _headers.push_back(name); _columnMap[name] = _headers.size() - 1; } // fixed _Headers -> _headers
    void setHeaders(const std::vector<std::string>& headers) { _headers = headers; _columnMap.clear(); for (size_t i = 0; i < headers.size(); ++i) _columnMap[headers[i]] = i; }
    void clear() { _rows.clear(); _headers.clear(); _columnMap.clear(); _error.clear(); }

private:
    std::vector<Row> _rows;
    std::vector<std::string> _headers;
    std::map<std::string, size_t> _columnMap;
    mutable std::string _error;

    std::string escapeField(const std::string& field, const Parser::Options& options) const {
        bool needsQuotes = (field.find(options.delimiter) != std::string::npos ||
                           field.find(options.quote) != std::string::npos ||
                           field.find('\n') != std::string::npos);
        if (!needsQuotes) return field;
        std::string escaped;
        escaped += options.quote;
        for (size_t i = 0; i < field.size(); ++i) {
            char c = field[i];
            if (c == options.quote) { escaped += options.quote; escaped += options.quote; }
            else escaped += c;
        }
        escaped += options.quote;
        return escaped;
    }
};

// ============================================================================
// FILTERING AND TRANSFORMATION (C++98-safe implementations)
// ============================================================================

class Filter {
public:
	// Filter by column value
	static Document where(const Document& doc, const std::string& column, const std::string& value) {
		Document result;
		result.setHeaders(doc.headers());
		int colIndex = doc.getColumnIndex(column);
		if (colIndex < 0) return result; // fixed missing parentheses
		for (size_t i = 0; i < doc.rows().size(); ++i) {
			if (doc.rows()[i][colIndex] == value) result.addRow(doc.rows()[i]);
		}
		return result;
	}

    static Document select(const Document& doc, const std::vector<std::string>& columns) {
        Document result; result.setHeaders(columns);
        std::vector<int> indices;
        for (size_t i = 0; i < columns.size(); ++i) indices.push_back(doc.getColumnIndex(columns[i]));
        for (size_t r = 0; r < doc.rows().size(); ++r) {
            std::vector<std::string> newValues;
            for (size_t j = 0; j < indices.size(); ++j) {
                int idx = indices[j];
                newValues.push_back(idx >= 0 ? doc.rows()[r][idx] : std::string());
            }
            Row newRow(newValues);
            result.addRow(newRow);
        }
        return result;
    }

    static Document sortBy(const Document& doc, const std::string& column, bool ascending = true) {
        Document result; result.setHeaders(doc.headers());
        int colIndex = doc.getColumnIndex(column);
        if (colIndex < 0) return doc;
        std::vector<Row> sortedRows = doc.rows();
        std::sort(sortedRows.begin(), sortedRows.end(), RowComparator(colIndex, ascending)); // use RowComparator
        for (size_t i = 0; i < sortedRows.size(); ++i) result.addRow(sortedRows[i]);
        return result;
    }

    static Document limit(const Document& doc, size_t maxRows) {
        Document result; result.setHeaders(doc.headers());
        for (size_t i = 0; i < doc.rows().size() && i < maxRows; ++i) result.addRow(doc.rows()[i]);
        return result;
    }

    static Document skip(const Document& doc, size_t skipCount) {
        Document result; result.setHeaders(doc.headers());
        for (size_t i = skipCount; i < doc.rows().size(); ++i) result.addRow(doc.rows()[i]);
        return result;
    }

    static std::map<std::string, Document> groupBy(const Document& doc, const std::string& column) {
        std::map<std::string, Document> groups;
        int colIndex = doc.getColumnIndex(column);
        if (colIndex < 0) return groups;
        for (size_t i = 0; i < doc.rows().size(); ++i) {
            std::string key = doc.rows()[i][colIndex];
            if (groups.find(key) == groups.end()) { groups[key] = Document(); groups[key].setHeaders(doc.headers()); }
            groups[key].addRow(doc.rows()[i]);
        }
        return groups;
    }
};

// ============================================================================
// AGGREGATION (C++98-safe implementations used by tests)
// ============================================================================

namespace Aggregate {
    // count non-empty values
    size_t countValues(const Document& doc, const std::string& column) {
        int idx = doc.getColumnIndex(column);
        if (idx < 0) return 0;
        size_t cnt = 0;
        for (size_t i = 0; i < doc.rows().size(); ++i) {
            if (!doc.rows()[i][idx].empty()) ++cnt;
        }
        return cnt;
    }

    double sum(const Document& doc, const std::string& column) {
        int idx = doc.getColumnIndex(column);
        if (idx < 0) return 0.0;
        double total = 0.0;
        for (size_t i = 0; i < doc.rows().size(); ++i) total += doc.rows()[i].getDouble(idx, 0.0);
        return total;
    }

    double average(const Document& doc, const std::string& column) {
        size_t cnt = countValues(doc, column);
        if (cnt == 0) return 0.0;
        return sum(doc, column) / static_cast<double>(cnt);
    }

    double min(const Document& doc, const std::string& column) {
        int idx = doc.getColumnIndex(column);
        if (idx < 0) return 0.0;
        bool found = false;
        double m = 0.0;
        for (size_t i = 0; i < doc.rows().size(); ++i) {
            double v = doc.rows()[i].getDouble(idx, 0.0);
            if (!found || v < m) { m = v; found = true; }
        }
        return found ? m : 0.0;
    }

    double max(const Document& doc, const std::string& column) {
        int idx = doc.getColumnIndex(column);
        if (idx < 0) return 0.0;
        bool found = false;
        double M = 0.0;
        for (size_t i = 0; i < doc.rows().size(); ++i) {
            double v = doc.rows()[i].getDouble(idx, 0.0);
            if (!found || v > M) { M = v; found = true; }
        }
        return found ? M : 0.0;
    }

    double stddev(const Document& doc, const std::string& column) {
        size_t cnt = countValues(doc, column);
        if (cnt == 0) return 0.0;
        double avg = average(doc, column);
        double var = 0.0;
        int idx = doc.getColumnIndex(column);
        for (size_t i = 0; i < doc.rows().size(); ++i) {
            double v = doc.rows()[i].getDouble(idx, 0.0);
            double d = v - avg;
            var += d * d;
        }
        return std::sqrt(var / static_cast<double>(cnt));
    }
}

} // namespace CSV

#endif // CSV_HPP