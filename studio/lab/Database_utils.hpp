/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Database_utils.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 02:34:49 by marvin            #+#    #+#             */
/*   Updated: 2025/12/24 02:34:49 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DATABASE_UTILITIES_HPP
#define DATABASE_UTILITIES_HPP

#include "Database.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <cstdlib>
#include <sstream>

// ============================================================================
// ADVANCED QUERY UTILITIES
// ============================================================================

namespace Query {

    // Predicate-based filtering (template to accept function pointers or functors)
    template <typename Pred>
    std::vector<Row> filter(const Table& table, Pred pred) {
        std::vector<Row> results;
        const std::vector<Row>& rows = table.rows();
        for (size_t i = 0; i < rows.size(); ++i) {
            if (pred(rows[i])) {
                results.push_back(rows[i]);
            }
        }
        return results;
    }

    // Column-based operations
    template<typename T, typename Converter>
    std::vector<T> selectColumn(const Table& table, const std::string& columnName, Converter converter) {
        std::vector<T> values;
        const std::vector<Row>& rows = table.rows();
        for (size_t i = 0; i < rows.size(); ++i) {
            values.push_back(converter(rows[i].getValue(columnName)));
        }
        return values;
    }

    // Simple converters
    inline double toDouble(const std::string& s) { return s.empty() ? 0.0 : atof(s.c_str()); }
    inline int toInt(const std::string& s) { return s.empty() ? 0 : atoi(s.c_str()); }

    // Aggregate functions
    double sum(const Table& table, const std::string& columnName) {
        std::vector<double> values = selectColumn<double>(table, columnName, &toDouble);
        return std::accumulate(values.begin(), values.end(), 0.0);
    }

    double average(const Table& table, const std::string& columnName) {
        if (table.rowCount() == 0) return 0.0;
        return sum(table, columnName) / static_cast<double>(table.rowCount());
    }

    double min(const Table& table, const std::string& columnName) {
        std::vector<double> values = selectColumn<double>(table, columnName, &toDouble);
        return values.empty() ? 0.0 : *std::min_element(values.begin(), values.end());
    }

    double max(const Table& table, const std::string& columnName) {
        std::vector<double> values = selectColumn<double>(table, columnName, &toDouble);
        return values.empty() ? 0.0 : *std::max_element(values.begin(), values.end());
    }

    template <typename Pred>
    size_t count(const Table& table, Pred pred) {
        return filter(table, pred).size();
    }

} // namespace Query

// ============================================================================
// TABLE TRANSFORMATION UTILITIES
// ============================================================================

namespace Transform {

    // Comparator functor (defined in namespace scope to avoid local-type issues)
    struct RowComparator {
        std::string column;
        bool ascending;
        RowComparator(const std::string& c, bool a) : column(c), ascending(a) {}
        bool operator()(const Row& a, const Row& b) const {
            std::string valA = a.getValue(column);
            std::string valB = b.getValue(column);
            return ascending ? (valA < valB) : (valA > valB);
        }
    };

    // Sort table by column
    Table sortBy(const Table& table, const std::string& columnName, bool ascending = true) {
        Table sorted;
        // copy columns
        const std::vector<Column>& tcols = table.columns();
        for (size_t i = 0; i < tcols.size(); ++i) {
            sorted.addColumn(tcols[i]);
        }

        std::vector<Row> rows = table.rows();
        std::sort(rows.begin(), rows.end(), RowComparator(columnName, ascending));

        for (size_t i = 0; i < rows.size(); ++i) {
            sorted.addRow(rows[i]);
        }

        return sorted;
    }

    // Group by column value
    std::map<std::string, std::vector<Row> > groupBy(const Table& table, const std::string& columnName) {
        std::map<std::string, std::vector<Row> > groups;
        const std::vector<Row>& rows = table.rows();
        for (size_t i = 0; i < rows.size(); ++i) {
            std::string key = rows[i].getValue(columnName);
            groups[key].push_back(rows[i]);
        }
        return groups;
    }

    // Select specific columns
    Table selectColumns(const Table& table, const std::vector<std::string>& columnNames) {
        Table result;

        // Add only selected columns
        const std::vector<Column>& tcols = table.columns();
        for (size_t i = 0; i < tcols.size(); ++i) {
            const Column& col = tcols[i];
            if (std::find(columnNames.begin(), columnNames.end(), col.name()) != columnNames.end()) {
                result.addColumn(col);
            }
        }

        // Add rows with only selected columns
        const std::vector<Row>& rows = table.rows();
        for (size_t r = 0; r < rows.size(); ++r) {
            Row newRow;
            for (size_t c = 0; c < columnNames.size(); ++c) {
                newRow.setValue(columnNames[c], rows[r].getValue(columnNames[c]));
            }
            result.addRow(newRow);
        }

        return result;
    }

    // Limit number of rows
    Table limit(const Table& table, size_t maxRows) {
        Table result;
        const std::vector<Column>& tcols = table.columns();
        for (size_t i = 0; i < tcols.size(); ++i) {
            result.addColumn(tcols[i]);
        }

        const std::vector<Row>& rows = table.rows();
        size_t cnt = 0;
        for (size_t i = 0; i < rows.size(); ++i) {
            if (cnt++ >= maxRows) break;
            result.addRow(rows[i]);
        }

        return result;
    }
}

// ============================================================================
// DATA GENERATION UTILITIES
// ============================================================================

namespace Generator {

    // Generate sample data
    Database createSampleEmployees(size_t count = 10) {
        Database db;

        db.addColumn(std::string("ID"), ColumnType::INTEGER, Alignment::RIGHT);
        db.addColumn(std::string("Name"), ColumnType::STRING, Alignment::LEFT);
        db.addColumn(std::string("Dept"), ColumnType::STRING, Alignment::LEFT);
        db.addColumn(std::string("Salary"), ColumnType::INTEGER, Alignment::RIGHT);
        db.addColumn(std::string("Active"), ColumnType::BOOLEAN, Alignment::CENTER);

        const char* namesArr[] = {"Alice", "Bob", "Charlie", "Diana", "Eve", "Frank", "Grace", "Henry", "Iris", "Jack"};
        const size_t NAMES_N = sizeof(namesArr) / sizeof(namesArr[0]);
        std::vector<std::string> names(namesArr, namesArr + NAMES_N);

        const char* deptsArr[] = {"Engineering", "Marketing", "Sales", "HR", "Finance"};
        const size_t DEPTS_N = sizeof(deptsArr) / sizeof(deptsArr[0]);
        std::vector<std::string> depts(deptsArr, deptsArr + DEPTS_N);

        for (size_t i = 0; i < count && i < names.size(); ++i) {
            std::map<std::string, std::string> row;
            std::ostringstream oss;
            oss << (i + 1);
            row["ID"] = oss.str();
            row["Name"] = names[i];
            row["Dept"] = depts[i % depts.size()];
            {
                std::ostringstream s2;
                s2 << (50000 + (i * 5000));
                row["Salary"] = s2.str();
            }
            row["Active"] = ((i % 3) != 0) ? "true" : "false";
            db.addRow(row);
        }

        return db;
    }

    // Generate sample sales data
    Database createSampleSales(size_t count = 8) {
        Database db;

        db.addColumn(std::string("Month"), ColumnType::STRING, Alignment::LEFT);
        db.addColumn(std::string("Product"), ColumnType::STRING, Alignment::LEFT);
        db.addColumn(std::string("Units"), ColumnType::INTEGER, Alignment::RIGHT);
        db.addColumn(std::string("Revenue"), ColumnType::STRING, Alignment::RIGHT);

        const char* monthsArr[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug"};
        const size_t MONTHS_N = sizeof(monthsArr) / sizeof(monthsArr[0]);
        std::vector<std::string> months(monthsArr, monthsArr + MONTHS_N);

        const char* productsArr[] = {"Widget A", "Widget B", "Widget C"};
        const size_t PROD_N = sizeof(productsArr) / sizeof(productsArr[0]);
        std::vector<std::string> products(productsArr, productsArr + PROD_N);

        for (size_t i = 0; i < count && i < months.size(); ++i) {
            std::map<std::string, std::string> row;
            row["Month"] = months[i];
            row["Product"] = products[i % products.size()];
            {
                std::ostringstream s1;
                s1 << (100 + (i * 25));
                row["Units"] = s1.str();
            }
            {
                std::ostringstream s2;
                s2 << "$" << ((100 + i * 25) * 50);
                row["Revenue"] = s2.str();
            }
            db.addRow(row);
        }

        return db;
    }
}

// ============================================================================
// STYLE PRESETS
// ============================================================================

namespace StylePresets {
    RenderConfig matrix() {
        RenderConfig cfg;
        cfg.boxChars = Unicode::BoxChars::heavy();
        cfg.headerStyle.foreground = Style::Color::BrightGreen();
        cfg.headerStyle.bold = true;
        cfg.cellStyle.foreground = Style::Color::Green();
        cfg.padding = 1;
        return cfg;
    }

    RenderConfig ocean() {
        RenderConfig cfg;
        cfg.boxChars = Unicode::BoxChars::rounded();
        cfg.headerStyle.foreground = Style::Color::BrightCyan();
        cfg.headerStyle.bold = true;
        cfg.cellStyle.foreground = Style::Color::Cyan();
        cfg.padding = 2;
        return cfg;
    }

    RenderConfig fire() {
        RenderConfig cfg;
        cfg.boxChars = Unicode::BoxChars::doubleLine();
        cfg.headerStyle.foreground = Style::Color::BrightRed();
        cfg.headerStyle.bold = true;
        cfg.cellStyle.foreground = Style::Color::BrightYellow();
        cfg.padding = 1;
        return cfg;
    }

    RenderConfig corporate() {
        RenderConfig cfg;
        cfg.boxChars = Unicode::BoxChars::doubleLine();
        cfg.headerStyle.foreground = Style::Color::BrightWhite();
        // set background code without C++11 brace-init
        Style::Color bg = Style::Color::Default();
        bg.code = 44;
        cfg.headerStyle.background = bg;
        cfg.headerStyle.bold = true;
        cfg.padding = 2;
        cfg.showFooter = false;
        return cfg;
    }

    RenderConfig newspaper() {
        RenderConfig cfg;
        cfg.headerStyle.foreground = Style::Color::Default();
        cfg.headerStyle.bold = true;
        cfg.headerStyle.underline = true;
        cfg.cellStyle.foreground = Style::Color::Default();
        cfg.padding = 1;
        return cfg;
    }

    // Add an 'elegant' alias that returns the RenderConfig elegant preset
    RenderConfig elegant() {
        return RenderConfig::elegant();
    }
}

// ============================================================================
// EXPORT UTILITIES
// ============================================================================

namespace Export {
    // Export to CSV
    void toCsv(const Table& table, const std::string& path) {
        std::ofstream file;
        file.open(path.c_str());
        if (!file.is_open()) {
            throw std::runtime_error(std::string("Cannot create file: ") + path);
        }

        const std::vector<Column>& cols = table.columns();
        for (size_t i = 0; i < cols.size(); ++i) {
            file << cols[i].name();
            if (i < cols.size() - 1) file << ",";
        }
        file << "\n";

        const std::vector<Row>& rows = table.rows();
        for (size_t r = 0; r < rows.size(); ++r) {
            for (size_t i = 0; i < cols.size(); ++i) {
                std::string value = rows[r].getValue(cols[i].name());
                if (value.find(',') != std::string::npos || value.find('"') != std::string::npos) {
                    value = "\"" + value + "\"";
                }
                file << value;
                if (i < cols.size() - 1) file << ",";
            }
            file << "\n";
        }

        file.close();
    }

    // Export to HTML
    std::string toHtml(Table& table, const std::string& title = "Data Table") {
        std::ostringstream html;

        html << "<!DOCTYPE html>\n<html>\n<head>\n";
        html << "<title>" << title << "</title>\n";
        html << "<style>\n";
        html << "body { font-family: Arial, sans-serif; margin: 20px; }\n";
        html << "table { border-collapse: collapse; width: 100%; }\n";
        html << "th, td { border: 1px solid #ddd; padding: 12px; text-align: left; }\n";
        html << "th { background-color: #4CAF50; color: white; font-weight: bold; }\n";
        html << "tr:nth-child(even) { background-color: #f2f2f2; }\n";
        html << "tr:hover { background-color: #ddd; }\n";
        html << "</style>\n</head>\n<body>\n";
        html << "<h1>" << title << "</h1>\n";
        html << "<table>\n<thead>\n<tr>\n";

        const std::vector<Column>& cols = table.columns();
        for (size_t i = 0; i < cols.size(); ++i) {
            html << "<th>" << cols[i].name() << "</th>\n";
        }
        html << "</tr>\n</thead>\n<tbody>\n";

        const std::vector<Row>& rows = table.rows();
        for (size_t r = 0; r < rows.size(); ++r) {
            html << "<tr>\n";
            for (size_t c = 0; c < cols.size(); ++c) {
                html << "<td>" << rows[r].getValue(cols[c].name()) << "</td>\n";
            }
            html << "</tr>\n";
        }

        html << "</tbody>\n</table>\n</body>\n</html>";
        return html.str();
    }

    // Export to Markdown
    std::string toMarkdown(Table& table) {
        table.calculateColumnWidths();
        std::ostringstream md;

        const std::vector<Column>& cols = table.columns();

        // Header row
        for (size_t i = 0; i < cols.size(); ++i) {
            md << "| " << Unicode::pad(cols[i].name(), cols[i].width(), 'l') << " ";
        }
        md << "|\n";

        // Separator
        for (size_t i = 0; i < cols.size(); ++i) {
            md << "|" << std::string(cols[i].width() + 2, '-');
        }
        md << "|\n";

        // Data rows
        const std::vector<Row>& rows = table.rows();
        for (size_t r = 0; r < rows.size(); ++r) {
            for (size_t c = 0; c < cols.size(); ++c) {
                std::string value = rows[r].getValue(cols[c].name());
                md << "| " << Unicode::pad(value, cols[c].width(), cols[c].getAlignChar()) << " ";
            }
            md << "|\n";
        }

        return md.str();
    }
}

// ============================================================================
// STATISTICS UTILITIES
// ============================================================================

namespace Statistics {
    struct ColumnStats {
        double min;
        double max;
        double mean;
        double median;
        double stdDev;
        size_t count;
    };

    ColumnStats analyze(const Table& table, const std::string& columnName) {
        ColumnStats stats;
        stats.min = stats.max = stats.mean = stats.median = stats.stdDev = 0.0;
        stats.count = 0;

        std::vector<double> values = Query::selectColumn<double>(table, columnName, &Query::toDouble);

        if (values.empty()) return stats;

        stats.count = values.size();
        stats.min = *std::min_element(values.begin(), values.end());
        stats.max = *std::max_element(values.begin(), values.end());
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        stats.mean = sum / static_cast<double>(values.size());

        std::vector<double> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        stats.median = sorted[sorted.size() / 2];

        double variance = 0.0;
        for (size_t i = 0; i < values.size(); ++i) {
            double v = values[i];
            variance += (v - stats.mean) * (v - stats.mean);
        }
        stats.stdDev = std::sqrt(variance / values.size());

        return stats;
    }

    Database createStatsTable(const ColumnStats& stats, const std::string& columnName) {
        Database db;

        db.addColumn(std::string("Statistic"), ColumnType::STRING, Alignment::LEFT);
        db.addColumn(std::string("Value"), ColumnType::STRING, Alignment::RIGHT);

        std::map<std::string, std::string> row;

        row.clear();
        row["Statistic"] = "Column";
        row["Value"] = columnName;
        db.addRow(row);

        row.clear();
        row["Statistic"] = "Count";
        { std::ostringstream oss; oss << stats.count; row["Value"] = oss.str(); }
        db.addRow(row);

        row.clear();
        row["Statistic"] = "Min";
        { std::ostringstream oss; oss << stats.min; row["Value"] = oss.str(); }
        db.addRow(row);

        row.clear();
        row["Statistic"] = "Max";
        { std::ostringstream oss; oss << stats.max; row["Value"] = oss.str(); }
        db.addRow(row);

        row.clear();
        row["Statistic"] = "Mean";
        { std::ostringstream oss; oss << stats.mean; row["Value"] = oss.str(); }
        db.addRow(row);

        row.clear();
        row["Statistic"] = "Median";
        { std::ostringstream oss; oss << stats.median; row["Value"] = oss.str(); }
        db.addRow(row);

        row.clear();
        row["Statistic"] = "Std Dev";
        { std::ostringstream oss; oss << stats.stdDev; row["Value"] = oss.str(); }
        db.addRow(row);

        return db;
    }
}

#endif // DATABASE_UTILITIES_HPP