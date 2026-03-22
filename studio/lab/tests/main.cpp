#include "Database.hpp"
#include "ft_string.hpp"
#include <iostream>

// Example 1: Basic table creation and rendering
void example_basic() {
    std::cout << "\n=== EXAMPLE 1: Basic Table ===\n\n";
    
    Database db;
    
    // Add columns with different types and alignments
    db.addColumn("ID", ColumnType::INTEGER, Alignment::RIGHT);
    db.addColumn("Name", ColumnType::STRING, Alignment::LEFT);
    db.addColumn("Score", ColumnType::DOUBLE, Alignment::RIGHT);
    db.addColumn("Active", ColumnType::BOOLEAN, Alignment::CENTER);
    
    // Add data
    {
        std::map<std::string,std::string> r;
        r["ID"] = "1"; r["Name"] = "Alice"; r["Score"] = "95.5"; r["Active"] = "true";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["ID"] = "2"; r["Name"] = "Bob"; r["Score"] = "855555557.3"; r["Active"] = "true";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["ID"] = "3"; r["Name"] = "Charlie"; r["Score"] = "92.1"; r["Active"] = "false";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["ID"] = "4"; r["Name"] = "Diana"; r["Score"] = "88.7"; r["Active"] = "true";
        db.addRow(r);
    }
    
    // Render with default config
    std::cout << db.render();
}

// Example 2: Elegant style with custom colors
void example_styled() {
    std::cout << "\n=== EXAMPLE 2: Elegant Styled Table ===\n\n";
    
    Database db;
    
    db.addColumn("Product", ColumnType::STRING, Alignment::LEFT);
    db.addColumn("Price", ColumnType::DOUBLE, Alignment::RIGHT);
    db.addColumn("Stock", ColumnType::INTEGER, Alignment::RIGHT);
    db.addColumn("Status", ColumnType::STRING, Alignment::CENTER);
    
    {
        std::map<std::string,std::string> r;
        r["Product"]="Laptop"; r["Price"]="$999.99"; r["Stock"]="15"; r["Status"]="✓";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Product"]="Mouse"; r["Price"]="$29.99"; r["Stock"]="150"; r["Status"]="✓";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Product"]="Keyboard"; r["Price"]="$79.99"; r["Stock"]="45"; r["Status"]="✓";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Product"]="Monitor"; r["Price"]="$349.99"; r["Stock"]="8"; r["Status"]="⚠";
        db.addRow(r);
    }
    
    // Custom elegant configuration
    RenderConfig config = RenderConfig::elegant();
    config.showFooter = true;
    config.footerText = "Total Items: 4 | Total Value: $1,459.96";
    config.footerStyle.foreground = Style::Color::BrightYellow();
    config.footerStyle.italic = true;
    
    std::cout << db.render(config);
}

// Example 3: Heavy borders with custom styling
void example_heavy() {
    std::cout << "\n=== EXAMPLE 3: Heavy Borders ===\n\n";

    Database db;

    db.addColumn("Rank", ColumnType::INTEGER, Alignment::CENTER);
    db.addColumn("Player", ColumnType::STRING, Alignment::LEFT);
    db.addColumn("Score", ColumnType::INTEGER, Alignment::RIGHT);

    {
        std::map<std::string,std::string> r;
        r["Rank"] = "🥇"; r["Player"] = "Champion"; r["Score"] = "9500";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Rank"] = "🥈"; r["Player"] = "Runner-up"; r["Score"] = "8900";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Rank"] = "🥉"; r["Player"] = "Third Place"; r["Score"] = "8200";
        db.addRow(r);
    }

    RenderConfig config;
    config.boxChars = Unicode::BoxChars::heavy();
    config.headerStyle.foreground = Style::Color::BrightMagenta();
    config.headerStyle.bold = true;
    config.cellStyle.foreground = Style::Color::BrightWhite();
    config.padding = 2;

    std::cout << db.render(config);
}

// Example 4: Unicode characters and international text
void example_unicode() {
    std::cout << "\n=== EXAMPLE 4: Unicode Support ===\n\n";

    // Headers and raw rows
    std::vector<std::string> headers;
    headers.push_back(std::string("País"));
    headers.push_back(std::string("Ciudad"));
    headers.push_back(std::string("Población"));

    std::vector< std::vector<std::string> > raw;
    {
        std::vector<std::string> r;
        r.push_back(std::string("日本")); r.push_back(std::string("東京")); r.push_back(std::string("13.96M"));
        raw.push_back(r);
    }
    {
        std::vector<std::string> r;
        r.push_back(std::string("España")); r.push_back(std::string("Madrid")); r.push_back(std::string("3.27M"));
        raw.push_back(r);
    }
    {
        std::vector<std::string> r;
        r.push_back(std::string("中国")); r.push_back(std::string("上海")); r.push_back(std::string("24.28M"));
        raw.push_back(r);
    }
    {
        std::vector<std::string> r;
        r.push_back(std::string("Россия")); r.push_back(std::string("Москва")); r.push_back(std::string("12.54M"));
        raw.push_back(r);
    }

    // build a Database from raw rows to use generic renderer (simpler integration)
    Database db;
    db.addColumn("País", ColumnType::STRING, Alignment::LEFT);
    db.addColumn("Ciudad", ColumnType::STRING, Alignment::LEFT);
    db.addColumn("Población", ColumnType::STRING, Alignment::RIGHT);
    for (size_t r = 0; r < raw.size(); ++r) {
        std::map<std::string,std::string> row;
        row["País"] = raw[r][0];
        row["Ciudad"] = raw[r][1];
        row["Población"] = raw[r][2];
        db.addRow(row);
    }
    RenderConfig config;
    config.boxChars = Unicode::BoxChars::doubleLine();
    std::cout << db.render(config);
}

// Example 5: Loading from CSV (simulated)
void example_csv() {
    std::cout << "\n=== EXAMPLE 5: CSV Loading ===\n\n";
    
    // Create a sample CSV file first
    std::ofstream csvFile("sample_data.csv");
    csvFile << "Employee,Department,Salary,Hire_Date\n";
    csvFile << "John Smith,Engineering,95000,2020-01-15\n";
    csvFile << "Sarah Johnson,Marketing,78000,2019-06-22\n";
    csvFile << "Michael Brown,Sales,82000,2021-03-10\n";
    csvFile << "Emma Davis,HR,71055555555555555500,2018-11-05\n";
    csvFile.close();
    
    try {
        Database db;
        db.loadFromCsv("sample_data.csv", true);
        
        // Customize column alignments after loading
        std::vector<Column>& cols = db.table().columns();
        if (cols.size() >= 3) {
            cols[2] = Column("Salary", ColumnType::DOUBLE, Alignment::RIGHT);
        }
        
        RenderConfig config = RenderConfig::elegant();
        config.showFooter = true;
        config.footerText = "🏢 Company Employee Database";
        config.footerStyle.foreground = Style::Color::BrightGreen();
        
        std::cout << db.render(config);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// Example 6: Query operations
void example_queries() {
    std::cout << "\n=== EXAMPLE 6: Query Operations ===\n\n";
    
    Database db;
    
    db.addColumn("ID", ColumnType::INTEGER, Alignment::RIGHT);
    db.addColumn("Department", ColumnType::STRING, Alignment::LEFT);
    db.addColumn("Budget", ColumnType::STRING, Alignment::RIGHT);
    
    {
        std::map<std::string,std::string> r;
        r["ID"] = "1"; r["Department"] = "Engineering"; r["Budget"] = "$500K";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["ID"] = "2"; r["Department"] = "Marketing"; r["Budget"] = "$300K";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["ID"] = "3"; r["Department"] = "Engineering"; r["Budget"] = "$450K";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["ID"] = "4"; r["Department"] = "Sales"; r["Budget"] = "$400K";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["ID"] = "5"; r["Department"] = "Engineering"; r["Budget"] = "$550K";
        db.addRow(r);
    }
    
    // Query for Engineering departments
    std::vector<Row> results = db.where("Department", "Engineering");
    
    std::cout << "Found " << results.size() << " Engineering departments:\n\n";
    
    // Create a new database with filtered results
    Database filteredDb;
    filteredDb.addColumn("ID", ColumnType::INTEGER, Alignment::RIGHT);
    filteredDb.addColumn("Department", ColumnType::STRING, Alignment::LEFT);
    filteredDb.addColumn("Budget", ColumnType::STRING, Alignment::RIGHT);
    
    for (size_t i = 0; i < results.size(); ++i) {
        const Row& row = results[i];
        std::map<std::string, std::string> data;
        // use row.data() to iterate
        const std::map<std::string,std::string>& rd = row.data();
        for (std::map<std::string,std::string>::const_iterator it = rd.begin(); it != rd.end(); ++it) {
            data[it->first] = it->second;
        }
        filteredDb.addRow(data);
    }
    
    RenderConfig config;
    config.headerStyle.foreground = Style::Color::BrightGreen();
    config.headerStyle.bold = true;
    
    std::cout << filteredDb.render(config);
}

// Example 7: Minimal style
void example_minimal() {
    std::cout << "\n=== EXAMPLE 7: Minimal Style ===\n\n";
    
    Database db;
    
    db.addColumn("Task", ColumnType::STRING, Alignment::LEFT);
    db.addColumn("Done", ColumnType::BOOLEAN, Alignment::CENTER);
    
    {
        std::map<std::string,std::string> r;
        r["Task"] = "Design database schema"; r["Done"] = "true";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Task"] = "Implement rendering"; r["Done"] = "true";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Task"] = "Add CSV support"; r["Done"] = "true";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Task"] = "Write documentation"; r["Done"] = "false";
        db.addRow(r);
    }
    
    RenderConfig config = RenderConfig::minimal();
    config.boxChars = Unicode::BoxChars::doubleLine();
    
    std::cout << db.render(config);
}

// Example 8: Financial report style
void example_financial() {
    std::cout << "\n=== EXAMPLE 8: Financial Report ===\n\n";
    
    Database db;
    
    db.addColumn("Quarter", ColumnType::STRING, Alignment::CENTER);
    db.addColumn("Revenue", ColumnType::STRING, Alignment::RIGHT);
    db.addColumn("Expenses", ColumnType::STRING, Alignment::RIGHT);
    db.addColumn("Profit", ColumnType::STRING, Alignment::RIGHT);
    db.addColumn("Δ", ColumnType::STRING, Alignment::RIGHT);
    
    {
        std::map<std::string,std::string> r;
        r["Quarter"] = "Q1 2024"; r["Revenue"] = "$2.5M"; r["Expenses"] = "$1.8M"; r["Profit"] = "$700K"; r["Δ"] = "+15%";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Quarter"] = "Q2 2024"; r["Revenue"] = "$2.8M"; r["Expenses"] = "$1.9M"; r["Profit"] = "$900K"; r["Δ"] = "+28%";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Quarter"] = "Q3 2024"; r["Revenue"] = "$3.1M"; r["Expenses"] = "$2.0M"; r["Profit"] = "$1.1M"; r["Δ"] = "+22%";
        db.addRow(r);
    }
    {
        std::map<std::string,std::string> r;
        r["Quarter"] = "Q4 2024"; r["Revenue"] = "$3.4M"; r["Expenses"] = "$2.1M"; r["Profit"] = "$1.3M"; r["Δ"] = "+18%";
        db.addRow(r);
    }

    RenderConfig config;
    config.boxChars = Unicode::BoxChars::doubleLine();
    config.headerStyle.foreground = Style::Color::BrightWhite();
    {
        Style::Color bg(42); // Green background
        config.headerStyle.background = bg;
    }
    config.headerStyle.bold = true;
    config.showFooter = true;
    config.footerText = "📊 Annual Growth: +21% | Total Profit: $4.0M";
    config.footerStyle.foreground = Style::Color::BrightYellow();
    config.footerStyle.bold = true;
    config.padding = 2;
    
    std::cout << db.render(config);
}

int main() {
    std::cout << "╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║   Database & Table Rendering System - Demo Suite      ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
    
    example_basic();
    example_styled();
    example_heavy();
    example_unicode();
    example_csv();
    example_queries();
    example_minimal();
    example_financial();
    
    std::cout << "\n✨ All examples completed successfully!\n\n";
    
    return 0;
}

// Compilation instructions:
// g++ -std=c++11 -o database_demo example_usage.cpp
// ./database_demo
//
// For proper Unicode display, ensure your terminal supports UTF-8 encoding