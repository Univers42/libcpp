#include "Database.hpp"
#include "Database_utils.hpp"
#include <iostream>
#include <iomanip>

// Demo 1: Query and filter operations
void demo_queries() {
    std::cout << "\n╔═══════════════════════════════════════╗\n";
    std::cout << "║  DEMO 1: Query & Filter Operations    ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n\n";

    Database db = Generator::createSampleEmployees(10);

    std::cout << "All Employees:\n";
    std::cout << db.render(StylePresets::corporate());

    // Filter active employees (manual loop to avoid std::function/lambda)
    std::vector<Row> filtered;
    const std::vector<Row>& allRows = db.table().rows();
    for (size_t i = 0; i < allRows.size(); ++i) {
        const Row& r = allRows[i];
        if (r.getValue("Active") == "true") filtered.push_back(r);
    }

    std::cout << "\nActive Employees Only (" << filtered.size() << " found):\n";

    Database activeDb;
    const std::vector<Column>& dbCols = db.table().columns();
    for (size_t i = 0; i < dbCols.size(); ++i) {
        const Column& col = dbCols[i];
        activeDb.addColumn(col.name(), col.type(), col.alignment());
    }
    for (size_t i = 0; i < filtered.size(); ++i) {
        std::map<std::string, std::string> data;
        const Row& row = filtered[i];
        const std::map<std::string, std::string>& rd = row.data();
        for (std::map<std::string, std::string>::const_iterator it = rd.begin(); it != rd.end(); ++it) {
            data[it->first] = it->second;
        }
        activeDb.addRow(data);
    }

    std::cout << activeDb.render(StylePresets::ocean());
}

// Demo 2: Aggregation functions
void demo_aggregation() {
    std::cout << "\n╔═══════════════════════════════════════╗\n";
    std::cout << "║  DEMO 2: Aggregation Functions        ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n\n";

    Database db = Generator::createSampleEmployees(10);

    // Calculate salary statistics
    double totalSalary = Query::sum(db.table(), "Salary");
    double avgSalary = Query::average(db.table(), "Salary");
    double minSalary = Query::min(db.table(), "Salary");
    double maxSalary = Query::max(db.table(), "Salary");

    Database statsDb;
    statsDb.addColumn("Metric", ColumnType::STRING, Alignment::LEFT);
    statsDb.addColumn("Value", ColumnType::STRING, Alignment::RIGHT);

    {
        std::map<std::string,std::string> r;
        r["Metric"] = "Total Payroll"; r["Value"] = "$" + (static_cast<std::ostringstream&>(std::ostringstream() << (int)totalSalary)).str();
        statsDb.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Metric"] = "Average Salary"; r["Value"] = "$" + (static_cast<std::ostringstream&>(std::ostringstream() << (int)avgSalary)).str();
        statsDb.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Metric"] = "Minimum Salary"; r["Value"] = "$" + (static_cast<std::ostringstream&>(std::ostringstream() << (int)minSalary)).str();
        statsDb.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Metric"] = "Maximum Salary"; r["Value"] = "$" + (static_cast<std::ostringstream&>(std::ostringstream() << (int)maxSalary)).str();
        statsDb.addRow(r);
    }

    RenderConfig cfg = StylePresets::fire();
    cfg.showFooter = true;
    cfg.footerText = "💰 Financial Summary Report";
    cfg.footerStyle.bold = true;

    std::cout << statsDb.render(cfg);
}

// Demo 3: Sorting and transformations
void demo_sorting() {
    std::cout << "\n╔═══════════════════════════════════════╗\n";
    std::cout << "║  DEMO 3: Sorting & Transformations    ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n\n";

    Database db = Generator::createSampleEmployees(8);

    std::cout << "Sorted by Salary (Descending):\n";
    Table sorted = Transform::sortBy(db.table(), "Salary", false);

    Database sortedDb;
    for (size_t i = 0; i < sorted.columns().size(); ++i) {
        const Column& col = sorted.columns()[i];
        sortedDb.addColumn(col.name(), col.type(), col.alignment());
    }
    for (size_t i = 0; i < sorted.rows().size(); ++i) {
        std::map<std::string,std::string> data;
        const Row& row = sorted.rows()[i];
        const std::map<std::string,std::string>& rd = row.data();
        for (std::map<std::string,std::string>::const_iterator it = rd.begin(); it != rd.end(); ++it) {
            data[it->first] = it->second;
        }
        sortedDb.addRow(data);
    }

    std::cout << sortedDb.render(StylePresets::matrix());

    // Select specific columns
    std::cout << "\nName and Salary Only:\n";
    Table selected = Transform::selectColumns(sorted, std::vector<std::string>(1, "Name"));
    // better to build vector properly:
    std::vector<std::string> cols;
    cols.push_back("Name"); cols.push_back("Salary");
    selected = Transform::selectColumns(sorted, cols);

    Database selectedDb;
    for (size_t i = 0; i < selected.columns().size(); ++i) {
        const Column& col = selected.columns()[i];
        selectedDb.addColumn(col.name(), col.type(), col.alignment());
    }
    for (size_t i = 0; i < selected.rows().size(); ++i) {
        std::map<std::string,std::string> data;
        const Row& row = selected.rows()[i];
        const std::map<std::string,std::string>& rd = row.data();
        for (std::map<std::string,std::string>::const_iterator it = rd.begin(); it != rd.end(); ++it) {
            data[it->first] = it->second;
        }
        selectedDb.addRow(data);
    }

    std::cout << selectedDb.render(StylePresets::newspaper());
}

// Demo 4: Grouping operations
void demo_grouping() {
    std::cout << "\n╔═══════════════════════════════════════╗\n";
    std::cout << "║  DEMO 4: Grouping Operations          ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n\n";

    Database db = Generator::createSampleEmployees(10);
    std::map<std::string, std::vector<Row> > groups = Transform::groupBy(db.table(), "Dept");

    std::cout << "Employees grouped by Department:\n\n";

    for (std::map<std::string, std::vector<Row> >::const_iterator git = groups.begin(); git != groups.end(); ++git) {
        const std::string& deptName = git->first;
        const std::vector<Row>& vec = git->second;

        std::cout << "Department: " << deptName << " (" << vec.size() << " employees)\n";

        Database deptDb;
        deptDb.addColumn("Name", ColumnType::STRING, Alignment::LEFT);
        deptDb.addColumn("Salary", ColumnType::INTEGER, Alignment::RIGHT);
        deptDb.addColumn("Active", ColumnType::BOOLEAN, Alignment::CENTER);

        for (size_t r = 0; r < vec.size(); ++r) {
            std::map<std::string,std::string> rowMap;
            rowMap["Name"] = vec[r].getValue("Name");
            rowMap["Salary"] = vec[r].getValue("Salary");
            rowMap["Active"] = vec[r].getValue("Active");
            deptDb.addRow(rowMap);
        }

        RenderConfig cfg;
        cfg.boxChars = Unicode::BoxChars::rounded();
        cfg.padding = 1;

        std::cout << deptDb.render(cfg) << "\n";
    }
}

// Demo 5: Statistical analysis
void demo_statistics() {
    std::cout << "\n╔═══════════════════════════════════════╗\n";
    std::cout << "║  DEMO 5: Statistical Analysis         ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n\n";

    Database db = Generator::createSampleEmployees(10);
    Statistics::ColumnStats stats = Statistics::analyze(db.table(), "Salary");
    Database statsDb = Statistics::createStatsTable(stats, "Employee Salaries");

    RenderConfig cfg = StylePresets::elegant();
    cfg.showFooter = true;
    cfg.footerText = "📊 Statistical Analysis Report";
    cfg.footerStyle.foreground = Style::Color::BrightMagenta();

    std::cout << statsDb.render(cfg);
}

// Demo 6: Export formats
void demo_export() {
    std::cout << "\n╔═══════════════════════════════════════╗\n";
    std::cout << "║  DEMO 6: Export Formats               ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n\n";

    Database db = Generator::createSampleSales(5);

    // Export to CSV
    Export::toCsv(db.table(), "sales_export.csv");
    std::cout << "✅ Exported to CSV: sales_export.csv\n\n";

    // Export to Markdown
    std::cout << "Markdown Output:\n";
    std::cout << "```markdown\n";
    std::cout << Export::toMarkdown(db.table());
    std::cout << "```\n\n";

    // HTML export (just show it's available)
    std::cout << "✅ HTML export available via Export::toHtml()\n";
}

// Demo 7: Style presets showcase
void demo_style_presets() {
    std::cout << "\n╔═══════════════════════════════════════╗\n";
    std::cout << "║  DEMO 7: Style Presets Showcase       ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n\n";

    Database db;
    db.addColumn("Style", ColumnType::STRING, Alignment::LEFT);
    db.addColumn("Description", ColumnType::STRING, Alignment::LEFT);

    {
        std::map<std::string,std::string> r;
        r["Style"] = "Matrix"; r["Description"] = "Hacker aesthetic";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Style"] = "Ocean"; r["Description"] = "Cool and calm";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Style"] = "Fire"; r["Description"] = "Hot and energetic";
        db.addRow(r);
    }

    std::cout << "Matrix Style:\n";
    std::cout << db.render(StylePresets::matrix()) << "\n";

    std::cout << "Ocean Style:\n";
    std::cout << db.render(StylePresets::ocean()) << "\n";

    std::cout << "Fire Style:\n";
    std::cout << db.render(StylePresets::fire()) << "\n";

    std::cout << "Corporate Style:\n";
    std::cout << db.render(StylePresets::corporate()) << "\n";
}

// Demo 8: Limit and pagination
void demo_pagination() {
    std::cout << "\n╔═══════════════════════════════════════╗\n";
    std::cout << "║  DEMO 8: Pagination & Limiting        ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n\n";

    Database db = Generator::createSampleEmployees(10);

    std::cout << "Top 5 Employees:\n";
    Table limited = Transform::limit(db.table(), 5);

    Database limitedDb;
    const std::vector<Column>& limitedCols = limited.columns();
    for (size_t i = 0; i < limitedCols.size(); ++i) {
        limitedDb.addColumn(limitedCols[i].name(), limitedCols[i].type(), limitedCols[i].alignment());
    }
    const std::vector<Row>& limitedRows = limited.rows();
    for (size_t r = 0; r < limitedRows.size(); ++r) {
        std::map<std::string,std::string> data;
        const std::map<std::string,std::string>& rd = limitedRows[r].data();
        for (std::map<std::string,std::string>::const_iterator it = rd.begin(); it != rd.end(); ++it) {
            data[it->first] = it->second;
        }
        limitedDb.addRow(data);
    }

    RenderConfig cfg = StylePresets::elegant();
    cfg.showFooter = true;
    cfg.footerText = "Showing 5 of 10 results";

    std::cout << limitedDb.render(cfg);
}

// Demo 9: Complex queries
void demo_complex_queries() {
    std::cout << "\n╔═══════════════════════════════════════╗\n";
    std::cout << "║  DEMO 9: Complex Query Examples       ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n\n";

    Database db = Generator::createSampleEmployees(10);

    // Find high-paid active employees
    std::vector<Row> resultsVec;
    const std::vector<Row>& all = db.table().rows();
    for (size_t i = 0; i < all.size(); ++i) {
        const Row& r = all[i];
        int salary = atoi(r.getValue("Salary").c_str());
        bool active = (r.getValue("Active") == "true");
        if (salary > 60000 && active) resultsVec.push_back(r);
    }

    std::cout << "High-Paid Active Employees (Salary > $60k):\n";

    Database resultDb;
    resultDb.addColumn("Name", ColumnType::STRING, Alignment::LEFT);
    resultDb.addColumn("Dept", ColumnType::STRING, Alignment::LEFT);
    resultDb.addColumn("Salary", ColumnType::INTEGER, Alignment::RIGHT);

    for (size_t i = 0; i < resultsVec.size(); ++i) {
        std::map<std::string,std::string> r;
        r["Name"] = resultsVec[i].getValue("Name");
        r["Dept"] = resultsVec[i].getValue("Dept");
        r["Salary"] = resultsVec[i].getValue("Salary");
        resultDb.addRow(r);
    }

    RenderConfig cfg = StylePresets::fire();
    cfg.showFooter = true;
    {
        std::ostringstream oss;
        oss << "🎯 " << resultsVec.size() << " matching records found";
        cfg.footerText = oss.str();
    }

    std::cout << resultDb.render(cfg);
}

// Demo 10: Unicode showcase
void demo_unicode_features() {
    std::cout << "\n╔═══════════════════════════════════════╗\n";
    std::cout << "║  DEMO 10: Unicode Features            ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n\n";

    Database db;
    db.addColumn("Icon", ColumnType::STRING, Alignment::CENTER);
    db.addColumn("Category", ColumnType::STRING, Alignment::LEFT);
    db.addColumn("Status", ColumnType::STRING, Alignment::CENTER);

    {
        std::map<std::string,std::string> r;
        r["Icon"]="🎨"; r["Category"]="Design"; r["Status"]="✓";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Icon"]="💻"; r["Category"]="Development"; r["Status"]="✓";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Icon"]="📊"; r["Category"]="Analytics"; r["Status"]="⚠";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Icon"]="🚀"; r["Category"]="Deployment"; r["Status"]="⏳";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Icon"]="🔒"; r["Category"]="Security"; r["Status"]="✗";
        db.addRow(r);
    }

    RenderConfig cfg;
    cfg.boxChars = Unicode::BoxChars::rounded();
    cfg.headerStyle.foreground = Style::Color::BrightMagenta();
    cfg.headerStyle.bold = true;
    cfg.padding = 2;
    cfg.showFooter = true;
    cfg.footerText = "🌟 Project Status Dashboard";
    cfg.footerStyle.foreground = Style::Color::BrightYellow();

    std::cout << db.render(cfg);
}

int main() {
    std::cout << "\n";
    std::cout << "╔═════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                             ║\n";
    std::cout << "║     🚀 Advanced Database System - Feature Demonstration     ║\n";
    std::cout << "║                                                             ║\n";
    std::cout << "╚═════════════════════════════════════════════════════════════╝\n";

    demo_queries();
    demo_aggregation();
    demo_sorting();
    demo_grouping();
    demo_statistics();
    demo_export();
    demo_style_presets();
    demo_pagination();
    demo_complex_queries();
    demo_unicode_features();

    std::cout << "\n";
    std::cout << "╔═════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  ✨ All advanced demos completed successfully!              ║\n";
    std::cout << "╚═════════════════════════════════════════════════════════════╝\n\n";

    std::cout << "📝 Features Demonstrated:\n";
    std::cout << "   • Query and filtering\n";
    std::cout << "   • Aggregation functions (sum, avg, min, max)\n";
    std::cout << "   • Sorting and transformations\n";
    std::cout << "   • Grouping operations\n";
    std::cout << "   • Statistical analysis\n";
    std::cout << "   • Export to CSV, HTML, Markdown\n";
    std::cout << "   • Multiple style presets\n";
    std::cout << "   • Pagination and limiting\n";
    std::cout << "   • Complex query predicates\n";
    std::cout << "   • Full Unicode support\n\n";

    return 0;
}

// Compilation:
// g++ -std=c++11 -o advanced_demo advanced_demo.cpp
// ./advanced_demo