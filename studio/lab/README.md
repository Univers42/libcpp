# C++ Database & Table Rendering System

A comprehensive, modular C++ library for managing data and rendering beautiful, customizable tables with full Unicode support and buffered output.

## 🌟 Key Features

### Core Capabilities
- **Repository Pattern**: Clean separation between data access and presentation
- **Unicode Aware**: Proper handling of UTF-8 characters, including emoji and international text
- **Buffered Rendering**: Fast, efficient output using ostringstream buffers
- **Type System**: Support for STRING, INTEGER, DOUBLE, DATE, and BOOLEAN types
- **Flexible Alignment**: Left, center, and right alignment per column
- **CSV Import/Export**: Load and save data from CSV files
- **Query System**: Filter, sort, group, and aggregate data

### Styling & Customization
- **Multiple Box Styles**: Normal, heavy, double-line, and rounded corners
- **ANSI Colors**: Full color support for headers, cells, and footers
- **Text Formatting**: Bold, italic, and underline support
- **Custom Padding**: Configurable cell padding
- **Footer Support**: Optional footers with custom text and styling
- **Style Presets**: Ready-to-use themes (Matrix, Ocean, Fire, Corporate, etc.)

### Advanced Features
- **Aggregation**: Sum, average, min, max, count operations
- **Statistical Analysis**: Mean, median, standard deviation calculations
- **Transformations**: Sort, group, select columns, limit rows
- **Export Formats**: CSV, HTML, Markdown
- **Predicate Queries**: Lambda-based filtering for complex queries
- **Modular Design**: Separate utilities for different concerns

## 📁 File Structure

```
database_system.hpp       # Core system (Column, Row, Table, Database, Renderer)
database_utilities.hpp    # Advanced utilities (Query, Transform, Export, etc.)
example_usage.cpp        # Basic examples and demonstrations
advanced_demo.cpp        # Advanced features showcase
```

## 🚀 Quick Start

### Basic Example

```cpp
#include "database_system.hpp"

int main() {
    Database db;
    
    // Define columns
    db.addColumn("Name", ColumnType::STRING, Alignment::LEFT);
    db.addColumn("Age", ColumnType::INTEGER, Alignment::RIGHT);
    db.addColumn("Active", ColumnType::BOOLEAN, Alignment::CENTER);
    
    // Add data
    db.addRow({{"Name", "Alice"}, {"Age", "25"}, {"Active", "true"}});
    db.addRow({{"Name", "Bob"}, {"Age", "30"}, {"Active", "false"}});
    
    // Render
    std::cout << db.render();
    
    return 0;
}
```

### Styled Example

```cpp
// Create elegant table
RenderConfig config = RenderConfig::elegant();
config.showFooter = true;
config.footerText = "Total: 2 records";
config.footerStyle.foreground = Style::Color::BrightYellow();

std::cout << db.render(config);
```

### Loading CSV

```cpp
Database db;
db.loadFromCsv("data.csv", true);  // true = has header row

// Customize column alignment after loading
db.table().columns()[2] = Column("Salary", ColumnType::DOUBLE, Alignment::RIGHT);

std::cout << db.render(StylePresets::corporate());
```

## 🎨 Styling System

### Box Characters

```cpp
// Light borders (default)
Unicode::BoxChars::BoxChars()

// Heavy borders
config.boxChars = Unicode::BoxChars::heavy();

// Double-line borders
config.boxChars = Unicode::BoxChars::doubleLine();

// Rounded corners
config.boxChars = Unicode::BoxChars::rounded();
```

### Colors

```cpp
Style::CellStyle style;
style.foreground = Style::Color::BrightCyan();
style.background = Style::Color{44};  // Cyan background
style.bold = true;
style.italic = true;
style.underline = true;

config.headerStyle = style;
```

### Available Colors
- Black, Red, Green, Yellow, Blue, Magenta, Cyan, White
- BrightBlack, BrightRed, BrightGreen, BrightYellow
- BrightBlue, BrightMagenta, BrightCyan, BrightWhite

### Style Presets

```cpp
StylePresets::matrix()      // Green hacker aesthetic
StylePresets::ocean()       // Cool cyan tones
StylePresets::fire()        // Hot red/yellow colors
StylePresets::corporate()   // Professional blue theme
StylePresets::newspaper()   // Minimal black & white
```

## 🔍 Query & Transform API

### Filtering

```cpp
#include "database_utilities.hpp"

// Simple filter by value
auto results = db.where("Department", "Engineering");

// Complex predicate-based filtering
auto filtered = Query::filter(db.table(), [](const Row& row) {
    int salary = std::stoi(row.getValue("Salary"));
    bool active = row.getValue("Active") == "true";
    return salary > 60000 && active;
});
```

### Aggregation

```cpp
double total = Query::sum(db.table(), "Salary");
double avg = Query::average(db.table(), "Salary");
double min = Query::min(db.table(), "Salary");
double max = Query::max(db.table(), "Salary");
size_t count = Query::count(db.table(), predicate);
```

### Sorting & Transforming

```cpp
// Sort by column
auto sorted = Transform::sortBy(db.table(), "Salary", false);  // descending

// Group by column
auto groups = Transform::groupBy(db.table(), "Department");

// Select specific columns
auto selected = Transform::selectColumns(db.table(), {"Name", "Salary"});

// Limit results
auto limited = Transform::limit(db.table(), 10);
```

### Statistical Analysis

```cpp
auto stats = Statistics::analyze(db.table(), "Salary");
// stats.min, stats.max, stats.mean, stats.median, stats.stdDev

auto statsTable = Statistics::createStatsTable(stats, "Salary Analysis");
std::cout << statsTable.render();
```

## 📤 Export Formats

### CSV Export

```cpp
Export::toCsv(db.table(), "output.csv");
```

### HTML Export

```cpp
std::string html = Export::toHtml(db.table(), "My Data Table");
std::ofstream file("output.html");
file << html;
file.close();
```

### Markdown Export

```cpp
std::cout << Export::toMarkdown(db.table());
```

## 🛠️ Modular Utilities

### Unicode Utilities

```cpp
// Calculate display width (handles multi-byte chars)
size_t width = Unicode::displayWidth("Hello 世界");

// Pad string to specific width with alignment
std::string padded = Unicode::pad("Text", 20, 'c');  // 'l', 'c', or 'r'
```

### Data Generation

```cpp
// Generate sample employee data
auto db = Generator::createSampleEmployees(10);

// Generate sample sales data
auto sales = Generator::createSampleSales(8);
```

## 🏗️ Architecture

### Component Hierarchy

```
Database
  └─ Table
       ├─ Columns (metadata + styling)
       └─ Rows (data records)

TableRenderer
  └─ RenderConfig (styling + box chars)
       └─ CellStyle (colors + formatting)
```

### Design Principles

1. **Separation of Concerns**: Data, logic, and presentation are separate
2. **Modular Design**: Small, focused utility functions
3. **Type Safety**: Column types enforce data semantics
4. **Performance**: Buffered output avoids byte-by-byte rendering
5. **Unicode First**: Proper multi-byte character handling throughout
6. **Customizable**: Every aspect can be styled and configured

## 📊 Column Types

```cpp
ColumnType::STRING    // Text data
ColumnType::INTEGER   // Whole numbers
ColumnType::DOUBLE    // Decimal numbers
ColumnType::DATE      // Date values
ColumnType::BOOLEAN   // True/false (renders as ✓/✗)
```

## ⚙️ Configuration Options

```cpp
RenderConfig config;

// Structure
config.boxChars = Unicode::BoxChars::heavy();
config.padding = 2;                    // Cell padding
config.autoWidth = true;               // Auto-calculate widths

// Headers
config.showHeader = true;
config.headerStyle.bold = true;
config.headerStyle.foreground = Style::Color::BrightCyan();

// Cells
config.cellStyle.foreground = Style::Color::White();

// Footers
config.showFooter = true;
config.footerText = "Summary information";
config.footerStyle.italic = true;
```

## 🔧 Compilation

```bash
# Basic compilation
g++ -std=c++11 -o demo example_usage.cpp

# With optimizations
g++ -std=c++11 -O3 -o demo example_usage.cpp

# With advanced features
g++ -std=c++11 -o advanced advanced_demo.cpp
```

### Requirements
- C++11 or later
- UTF-8 terminal for Unicode support
- ANSI color support for styling

## 💡 Usage Tips

### Terminal Setup
```bash
# Ensure UTF-8 encoding
export LANG=en_US.UTF-8

# Test Unicode support
echo "Box chars: ┌─┐│└┘ Emoji: 🚀✓✗"
```

### Best Practices

1. **Always use buffered rendering** - The system does this automatically
2. **Calculate widths automatically** - Set `config.autoWidth = true`
3. **Use appropriate alignments** - Numbers right, text left, booleans center
4. **Choose readable colors** - Test on both light and dark terminals
5. **Keep utilities modular** - Use Query/Transform/Export separately
6. **Handle exceptions** - CSV loading and file operations can fail

## 📈 Performance Characteristics

- **Buffered Output**: O(n) single-pass rendering
- **Column Width Calculation**: O(n×m) where n=rows, m=columns
- **Sorting**: O(n log n) standard sort
- **Filtering**: O(n) single-pass predicate evaluation
- **Grouping**: O(n) with map overhead

## 🎯 Use Cases

- **Data Analysis**: Quick visualization of CSV data
- **CLI Tools**: Professional terminal output
- **Reports**: Formatted data reports with styling
- **Dashboards**: Real-time data display
- **Logging**: Structured log output
- **Testing**: Display test results in tables
- **Administration**: System status displays

## 🔮 Future Enhancements

Potential additions:
- SQL-like query DSL
- JOIN operations between tables
- Index support for faster queries
- Persistent storage (SQLite integration)
- Column formatters (date, currency, percentage)
- Cell-level styling
- Multi-line cell content
- Nested tables
- Conditional formatting
- Chart/graph generation

## 📝 License

This is a demonstration system. Adapt and use as needed for your projects.

## 🤝 Contributing

This is a complete, working system ready for:
- Extension with new features
- Integration into larger projects
- Customization for specific needs
- Educational purposes


# Complete C++ Data Processing System

A professional-grade C++ library suite for data processing, visualization, and manipulation with full Unicode support.

## 📦 Components

### 1. **colors.hpp** - Advanced Color System
- RGB color class with manipulation (lighten, darken, saturate)
- Named color palette (Material Design, basic colors)
- Gradient generation (Rainbow, Sunset, Ocean, Fire)
- Color schemes (analogous, complementary, triadic, monochromatic)
- ANSI formatting (16-color, 256-color, true color)
- Text styling (bold, italic, underline, strikethrough)

### 2. **csv.hpp** - Professional CSV Parser
- RFC 4180 compliant parsing
- Type inference (STRING, INTEGER, DOUBLE, BOOLEAN, DATE)
- Robust handling of quotes, escapes, delimiters
- Filtering and transformation (where, select, sort, limit, group)
- Aggregation functions (sum, average, min, max, stddev, count)
- Data validation with custom rules
- Export to CSV, HTML, Markdown
- Memory-efficient streaming

### 3. **Date.hpp** - Complete Date System
- Date arithmetic (add/subtract days, months, years)
- Multiple format support (ISO, DD/MM/YYYY, MM/DD/YYYY)
- Weekday calculations and names
- Date ranges with iteration
- Business day calculations
- Age calculation
- Quarter operations
- Date validation
- Comparison and arithmetic operators

### 4. **database_system.hpp** - Table Rendering Engine
- Unicode-aware box drawing (4 styles)
- Buffered rendering for performance
- Column types and alignment
- ANSI color styling
- Customizable headers and footers
- CSV integration
- Query and filter support

## 🚀 Quick Start Examples

### Colors

```cpp
#include "colors.hpp"

// Basic colors
Colors::Formatter fmt;
std::cout << fmt.fg("Red text", Colors::Palette::Red()) << "\n";

// Color manipulation
Colors::RGB blue = Colors::Palette::Blue();
Colors::RGB lighter = blue.lighten(0.3);
Colors::RGB darker = blue.darken(0.3);

// Gradients
auto gradient = Colors::Gradient::Rainbow().generate(10);
for (const auto& color : gradient) {
    std::cout << fmt.fg("█", color);
}

// Color mixing
Colors::RGB purple = Colors::RGB::mix(
    Colors::Palette::Red(), 
    Colors::Palette::Blue(), 
    0.5
);
```

### CSV Processing

```cpp
#include "csv.hpp"

// Load CSV
CSV::Document doc;
doc.load("data.csv");

// Filter rows
auto filtered = CSV::Filter::where(doc, "Status", "Active");

// Aggregate
double total = CSV::Aggregate::sum(doc, "Sales");
double avg = CSV::Aggregate::average(doc, "Sales");

// Group by
auto groups = CSV::Filter::groupBy(doc, "Department");
for (const auto& group : groups) {
    std::cout << group.first << ": " 
              << group.second.rowCount() << " rows\n";
}

// Sort
auto sorted = CSV::Filter::sortBy(doc, "Date", false);

// Complex filtering
auto results = CSV::Filter::filter(doc, [](const CSV::Row& row) {
    return row.getDouble(2) > 1000 && row.getBool(5);
});

// Export
doc.save("output.csv");
CSV::Export::toHtml(doc, "report.html");
```

### Date Operations

```cpp
#include "Date.hpp"

// Create dates
Date today = Date::today();
Date birthday(1990, 5, 15);

// Arithmetic
Date nextWeek = today.addDays(7);
Date nextMonth = today.addMonths(1);
int daysBetween = today - birthday;

// Formatting
std::cout << today.toString("Weekday, Month DD, YYYY") << "\n";
std::cout << today.toString("YYYY-MM-DD") << "\n";
std::cout << today.toString("DD/MM/YYYY") << "\n";

// Weekday operations
std::cout << "Today is: " << today.weekdayName() << "\n";
Date nextMonday = DateUtils::nextWeekday(today, 1);

// Date ranges
DateRange week(today, today.addDays(6));
for (const Date& date : week) {
    std::cout << date.toString() << "\n";
}

// Business days
int workDays = DateUtils::businessDaysBetween(start, end);
Date inTenWorkDays = DateUtils::addBusinessDays(today, 10);

// Age calculation
int age = DateUtils::age(birthday);
```

### Database Tables

```cpp
#include "database_system.hpp"

// Create table
Database db;
db.addColumn("Name", ColumnType::STRING, Alignment::LEFT);
db.addColumn("Age", ColumnType::INTEGER, Alignment::RIGHT);
db.addColumn("Score", ColumnType::DOUBLE, Alignment::RIGHT);

// Add data
db.addRow({{"Name", "Alice"}, {"Age", "25"}, {"Score", "95.5"}});
db.addRow({{"Name", "Bob"}, {"Age", "30"}, {"Score", "87.3"}});

// Style it
RenderConfig config = RenderConfig::elegant();
config.boxChars = Unicode::BoxChars::doubleLine();
config.showFooter = true;
config.footerText = "Summary: 2 records";

std::cout << db.render(config);
```

## 🔗 Integration Example

```cpp
#include "colors.hpp"
#include "csv.hpp"
#include "Date.hpp"
#include "database_system.hpp"

// Load CSV data
CSV::Document csvDoc;
csvDoc.load("sales.csv");

// Filter recent data
Date cutoff = Date::today().subtractDays(30);
auto recent = CSV::Filter::filter(csvDoc, [cutoff](const CSV::Row& row) {
    Date rowDate = Date::parse(row[0]);
    return rowDate >= cutoff;
});

// Calculate statistics
double total = CSV::Aggregate::sum(recent, "Amount");
double avg = CSV::Aggregate::average(recent, "Amount");

// Create styled table
Database db;
db.addColumn("Metric", ColumnType::STRING, Alignment::LEFT);
db.addColumn("Value", ColumnType::STRING, Alignment::RIGHT);
db.addRow({{"Metric", "Total Sales"}, {"Value", "$" + std::to_string((int)total)}});
db.addRow({{"Metric", "Average"}, {"Value", "$" + std::to_string((int)avg)}});
db.addRow({{"Metric", "Records"}, {"Value", std::to_string(recent.rowCount())}});

// Apply colors
RenderConfig config;
config.headerStyle.foreground = Style::Color::BrightCyan();
config.headerStyle.bold = true;
config.showFooter = true;
config.footerText = "Last 30 days";

std::cout << db.render(config);
```

## 🎨 Color System Features

### Color Manipulation

```cpp
Colors::RGB base(100, 150, 200);

// Brightness
auto lighter = base.lighten(0.3);  // 30% lighter
auto darker = base.darken(0.3);    // 30% darker

// Saturation
auto saturated = base.saturate(0.5);    // More vivid
auto desaturated = base.desaturate(0.5); // More gray

// Complement
auto opposite = base.complement();

// Mixing
auto purple = Colors::RGB::mix(
    Colors::Palette::Red(),
    Colors::Palette::Blue()
);
```

### Gradients

```cpp
// Predefined gradients
auto rainbow = Colors::Gradient::Rainbow();
auto sunset = Colors::Gradient::Sunset();
auto ocean = Colors::Gradient::Ocean();
auto fire = Colors::Gradient::Fire();

// Custom gradient
Colors::Gradient custom;
custom.addStop(Colors::Palette::Red())
      .addStop(Colors::Palette::Yellow())
      .addStop(Colors::Palette::Green());

// Generate colors
auto colors = custom.generate(10);  // 10 color steps
```

### Color Schemes

```cpp
Colors::RGB base = Colors::Palette::Blue();

// Generate color schemes
auto analogous = Colors::ColorScheme::analogous(base, 5);
auto triadic = Colors::ColorScheme::triadic(base);
auto monochromatic = Colors::ColorScheme::monochromatic(base, 5);
auto complementary = Colors::ColorScheme::complementary(base);
```

## 📊 CSV Advanced Features

### Type Inference

```cpp
CSV::DataType type = CSV::TypeInference::inferType("123");      // INTEGER
type = CSV::TypeInference::inferType("123.45");                 // DOUBLE
type = CSV::TypeInference::inferType("true");                   // BOOLEAN
type = CSV::TypeInference::inferType("2024-12-24");            // DATE
```

### Custom Parsing Options

```cpp
CSV::Parser::Options options;
options.delimiter = '\t';        // Tab-separated
options.quote = '"';
options.hasHeader = true;
options.trimWhitespace = true;
options.skipEmptyLines = true;

CSV::Document doc;
doc.loadFromStream(stream, options);

// Presets
auto tsvOptions = CSV::Parser::Options::TSV();
auto excelOptions = CSV::Parser::Options::Excel();
```

### Data Validation

```cpp
// Check required fields
auto result = CSV::Validator::checkRequired(doc, {"Name", "Email"});
if (!result.valid) {
    for (const auto& error : result.errors) {
        std::cout << error << "\n";
    }
}

// Check types
auto typeResult = CSV::Validator::checkType(doc, "Age", CSV::DataType::INTEGER);

// Custom validation
auto customResult = CSV::Validator::validate(doc, "Email", 
    [](const std::string& val) {
        return val.find('@') != std::string::npos;
    });
```

### Export Formats

```cpp
// CSV
doc.save("output.csv");

// HTML
std::string html = CSV::Export::toHtml(doc, "My Report");
std::ofstream("report.html") << html;

// Markdown
std::cout << CSV::Export::toMarkdown(doc);
```

## 📅 Date Advanced Features

### Date Ranges

```cpp
Date start(2024, 1, 1);
Date end(2024, 12, 31);
DateRange year2024(start, end);

// Iterate
for (const Date& date : year2024) {
    if (DateUtils::isWeekend(date)) {
        std::cout << date.toString() << " is weekend\n";
    }
}

// Convert to vector
std::vector<Date> allDates = year2024.toVector();

// Check containment
bool contains = year2024.contains(Date::today());

// Check overlap
DateRange q1(Date(2024, 1, 1), Date(2024, 3, 31));
bool overlaps = year2024.overlaps(q1);
```

### Utilities

```cpp
// First/last of month
Date firstDay = DateUtils::firstOfMonth(2024, 12);
Date lastDay = DateUtils::lastOfMonth(2024, 12);

// First/last of year
Date jan1 = DateUtils::firstOfYear(2024);
Date dec31 = DateUtils::lastOfYear(2024);

// All dates in month
auto december = DateUtils::datesInMonth(2024, 12);

// Weekday checks
bool isWeekend = DateUtils::isWeekend(date);
bool isWeekday = DateUtils::isWeekday(date);

// Next/previous weekday
Date nextFriday = DateUtils::nextWeekday(today, 5);
Date lastMonday = DateUtils::previousWeekday(today, 1);

// Quarter operations
int q = DateUtils::quarter(date);           // 1-4
Date qStart = DateUtils::quarterStart(2024, 1);
Date qEnd = DateUtils::quarterEnd(2024, 1);
```

## 🎯 Performance Considerations

### CSV Processing
- **Streaming**: For large files, process row-by-row instead of loading all
- **Filtering**: Apply filters early to reduce data set size
- **Indexing**: For repeated lookups, create column maps

### Date Operations
- **Caching**: Cache frequently used dates (today, first of month)
- **Ranges**: Use DateRange iteration instead of manual loops
- **Validation**: Validate dates once, not repeatedly

### Table Rendering
- **Buffering**: Rendering uses ostringstream for efficiency
- **Width Calculation**: Auto-width calculates once per render
- **Colors**: Use ANSI_256 mode for best terminal compatibility

## 🔧 Compilation

```bash
# Basic compilation
g++ -std=c++11 -o app main.cpp

# With optimizations
g++ -std=c++11 -O3 -o app main.cpp

# With warnings
g++ -std=c++11 -Wall -Wextra -o app main.cpp

# Example compilation
g++ -std=c++11 -o integrated integrated_example.cpp
./integrated
```

## 📋 Requirements

- C++11 or later
- UTF-8 terminal for Unicode support
- ANSI color support for styling
- Standard library (no external dependencies)

## 🌟 Best Practices

### 1. Error Handling
```cpp
try {
    CSV::Document doc;
    if (!doc.load("data.csv")) {
        std::cerr << "Error: " << doc.error() << "\n";
    }
} catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
}
```

### 2. Type Safety
```cpp
// Use type-safe getters
int age = row.getInt(2, 0);           // Default value
double salary = row.getDouble(3, 0.0);
bool active = row.getBool(5, false);
```

### 3. Resource Management
```cpp
// Scope-based file handling
{
    std::ofstream file("output.csv");
    doc.saveToStream(file);
}  // File closed automatically
```

### 4. Code Organization
```cpp
// Separate concerns
void loadData(CSV::Document& doc);
void processData(const CSV::Document& doc);
void displayResults(const Database& db);
```

## 📚 Additional Examples

See the included example files:
- `example_usage.cpp` - Basic database examples
- `advanced_demo.cpp` - Advanced database features
- `integrated_example.cpp` - Complete system integration

## 🤝 Contributing

This is a complete, production-ready system. Feel free to:
- Extend with new features
- Optimize for specific use cases
- Integrate into larger projects
- Use as educational material

---

**Built for engineers, by engineers. 🚀**