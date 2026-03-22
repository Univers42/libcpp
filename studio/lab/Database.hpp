/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Database.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 02:13:46 by marvin            #+#    #+#             */
/*   Updated: 2025/12/24 02:13:46 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <stdint.h>
#include <cctype>   // <--- added for tolower
#include "ft_string.hpp" // use Unicode-aware case helpers from ft_string.cpp

// Forward declare strcase_toggle (defined in ft_string.cpp)
void strcase_toggle(std::string *s, int mod);

// ============================================================================
// UNICODE UTILITIES
// ============================================================================

namespace Unicode {
    
    // Decode a single UTF-8 character and return its code point
    inline uint32_t decodeUTF8(const std::string& str, size_t& index) {
        uint32_t codepoint = 0;
        unsigned char c = (unsigned char)str[index];
        
        if ((c & 0x80) == 0) {
            // 1-byte ASCII
            codepoint = c;
            index += 1;
        } else if ((c & 0xE0) == 0xC0) {
            // 2-byte sequence
            if (index + 1 < str.size()) {
                codepoint = ((c & 0x1F) << 6) | ((unsigned char)str[index + 1] & 0x3F);
            }
            index += 2;
        } else if ((c & 0xF0) == 0xE0) {
            // 3-byte sequence
            if (index + 2 < str.size()) {
                codepoint = ((c & 0x0F) << 12) | 
                           (((unsigned char)str[index + 1] & 0x3F) << 6) | 
                           ((unsigned char)str[index + 2] & 0x3F);
            }
            index += 3;
        } else if ((c & 0xF8) == 0xF0) {
            // 4-byte sequence (emoji, etc.)
            if (index + 3 < str.size()) {
                codepoint = ((c & 0x07) << 18) | 
                           (((unsigned char)str[index + 1] & 0x3F) << 12) | 
                           (((unsigned char)str[index + 2] & 0x3F) << 6) | 
                           ((unsigned char)str[index + 3] & 0x3F);
            }
            index += 4;
        } else {
            // Invalid sequence, skip
            index += 1;
        }
        
        return codepoint;
    }
    
    // Calculate display width of a single Unicode code point
    inline int codepointWidth(uint32_t cp) {
        // Combining characters (zero width)
        if (cp >= 0x0300 && cp <= 0x036F) return 0; // Combining Diacritical Marks
        if (cp >= 0x1AB0 && cp <= 0x1AFF) return 0; // Combining Diacritical Marks Extended
        if (cp >= 0x1DC0 && cp <= 0x1DFF) return 0; // Combining Diacritical Marks Supplement
        if (cp >= 0x20D0 && cp <= 0x20FF) return 0; // Combining Diacritical Marks for Symbols
        if (cp >= 0xFE20 && cp <= 0xFE2F) return 0; // Combining Half Marks
        
        // Control characters
        if (cp < 0x20 || (cp >= 0x7F && cp < 0xA0)) return 0;
        
        // Emoji and special wide characters (2 columns)
        if (cp >= 0x1F300 && cp <= 0x1F9FF) return 2; // Miscellaneous Symbols and Pictographs, Emoticons, etc.
        if (cp >= 0x1F000 && cp <= 0x1F2FF) return 2; // Mahjong Tiles, Domino Tiles, Playing Cards
        if (cp >= 0x1FA00 && cp <= 0x1FAFF) return 2; // Chess Symbols, Symbols and Pictographs Extended-A
        if (cp >= 0x2600 && cp <= 0x26FF) return 2;   // Miscellaneous Symbols
        if (cp >= 0x2700 && cp <= 0x27BF) return 2;   // Dingbats
        if (cp >= 0xFE00 && cp <= 0xFE0F) return 0;   // Variation Selectors (zero width)
        
        // CJK Unified Ideographs (Chinese, Japanese, Korean) - 2 columns
        if (cp >= 0x4E00 && cp <= 0x9FFF) return 2;   // CJK Unified Ideographs
        if (cp >= 0x3400 && cp <= 0x4DBF) return 2;   // CJK Unified Ideographs Extension A
        if (cp >= 0x20000 && cp <= 0x2A6DF) return 2; // CJK Unified Ideographs Extension B
        if (cp >= 0x2A700 && cp <= 0x2B73F) return 2; // CJK Unified Ideographs Extension C
        if (cp >= 0x2B740 && cp <= 0x2B81F) return 2; // CJK Unified Ideographs Extension D
        if (cp >= 0x2B820 && cp <= 0x2CEAF) return 2; // CJK Unified Ideographs Extension E
        if (cp >= 0xF900 && cp <= 0xFAFF) return 2;   // CJK Compatibility Ideographs
        if (cp >= 0x2F800 && cp <= 0x2FA1F) return 2; // CJK Compatibility Ideographs Supplement
        
        // Hangul (Korean)
        if (cp >= 0xAC00 && cp <= 0xD7AF) return 2;   // Hangul Syllables
        if (cp >= 0x1100 && cp <= 0x11FF) return 2;   // Hangul Jamo
        if (cp >= 0x3130 && cp <= 0x318F) return 2;   // Hangul Compatibility Jamo
        if (cp >= 0xA960 && cp <= 0xA97F) return 2;   // Hangul Jamo Extended-A
        if (cp >= 0xD7B0 && cp <= 0xD7FF) return 2;   // Hangul Jamo Extended-B
        
        // Hiragana and Katakana (Japanese)
        if (cp >= 0x3040 && cp <= 0x309F) return 2;   // Hiragana
        if (cp >= 0x30A0 && cp <= 0x30FF) return 2;   // Katakana
        if (cp >= 0x31F0 && cp <= 0x31FF) return 2;   // Katakana Phonetic Extensions
        
        // Arabic (right-to-left, single width but may combine)
        if (cp >= 0x0600 && cp <= 0x06FF) return 1;   // Arabic
        if (cp >= 0x0750 && cp <= 0x077F) return 1;   // Arabic Supplement
        if (cp >= 0x08A0 && cp <= 0x08FF) return 1;   // Arabic Extended-A
        if (cp >= 0xFB50 && cp <= 0xFDFF) return 1;   // Arabic Presentation Forms-A
        if (cp >= 0xFE70 && cp <= 0xFEFF) return 1;   // Arabic Presentation Forms-B
        
        // Hebrew
        if (cp >= 0x0590 && cp <= 0x05FF) return 1;   // Hebrew
        
        // Devanagari (Hindi, Sanskrit)
        if (cp >= 0x0900 && cp <= 0x097F) return 1;   // Devanagari
        if (cp >= 0xA8E0 && cp <= 0xA8FF) return 1;   // Devanagari Extended
        
        // Greek
        if (cp >= 0x0370 && cp <= 0x03FF) return 1;   // Greek and Coptic
        if (cp >= 0x1F00 && cp <= 0x1FFF) return 1;   // Greek Extended
        
        // Cyrillic (Russian, etc.)
        if (cp >= 0x0400 && cp <= 0x04FF) return 1;   // Cyrillic
        if (cp >= 0x0500 && cp <= 0x052F) return 1;   // Cyrillic Supplement
        if (cp >= 0x2DE0 && cp <= 0x2DFF) return 1;   // Cyrillic Extended-A
        if (cp >= 0xA640 && cp <= 0xA69F) return 1;   // Cyrillic Extended-B
        
        // Full-width Latin characters
        if (cp >= 0xFF01 && cp <= 0xFF5E) return 2;   // Fullwidth ASCII variants
        
        // Default: single width for everything else (Latin, ASCII, etc.)
        return 1;
    }
    
    // Calculate display width of UTF-8 string
    size_t displayWidth(const std::string& str) {
        size_t width = 0;
        size_t index = 0;
        
        while (index < str.size()) {
            uint32_t cp = decodeUTF8(str, index);
            if (cp > 0) {
                width += codepointWidth(cp);
            }
        }
        
        return width;
    }
    
    // Pad string to specific display width
    std::string pad(const std::string& str, size_t width, char align = 'l') {
        size_t currentWidth = displayWidth(str);
        if (currentWidth >= width) return str;
        
        size_t padding = width - currentWidth;
        if (align == 'c') {
            size_t left = padding / 2;
            size_t right = padding - left;
            return std::string(left, ' ') + str + std::string(right, ' ');
        } else if (align == 'r') {
            return std::string(padding, ' ') + str;
        } else {
            return str + std::string(padding, ' ');
        }
    }

    struct BoxChars {
        std::string topLeft;
        std::string topRight;
        std::string bottomLeft;
        std::string bottomRight;
        std::string horizontal;
        std::string vertical;
        std::string cross;
        std::string teeLeft;
        std::string teeRight;
        std::string teeTop;
        std::string teeBottom;

        BoxChars() {
            topLeft = "┌"; topRight = "┐"; bottomLeft = "└"; bottomRight = "┘";
            horizontal = "─"; vertical = "│"; cross = "┼";
            teeLeft = "├"; teeRight = "┤"; teeTop = "┬"; teeBottom = "┴";
        }

        static BoxChars heavy() {
            BoxChars b;
            b.topLeft = "┏"; b.topRight = "┓";
            b.bottomLeft = "┗"; b.bottomRight = "┛";
            b.horizontal = "━"; b.vertical = "┃";
            b.cross = "╋"; b.teeLeft = "┣";
            b.teeRight = "┫"; b.teeTop = "┳";
            b.teeBottom = "┻";
            return b;
        }

        static BoxChars doubleLine() {
            BoxChars b;
            b.topLeft = "╔"; b.topRight = "╗";
            b.bottomLeft = "╚"; b.bottomRight = "╝";
            b.horizontal = "═"; b.vertical = "║";
            b.cross = "╬"; b.teeLeft = "╠";
            b.teeRight = "╣"; b.teeTop = "╦";
            b.teeBottom = "╩";
            return b;
        }

        static BoxChars rounded() {
            BoxChars b;
            b.topLeft = "╭"; b.topRight = "╮";
            b.bottomLeft = "╰"; b.bottomRight = "╯";
            return b;
        }
    };
}

// ============================================================================
// COLOR AND STYLE UTILITIES
// ============================================================================

namespace Style {
    struct Color {
        int code;
        Color() : code(39) {}
        explicit Color(int c) : code(c) {}
        static Color Black() { return Color(30); }
        static Color Red() { return Color(31); }
        static Color Green() { return Color(32); }
        static Color Yellow() { return Color(33); }
        static Color Blue() { return Color(34); }
        static Color Magenta() { return Color(35); }
        static Color Cyan() { return Color(36); }
        static Color White() { return Color(37); }
        static Color BrightBlack() { return Color(90); }
        static Color BrightRed() { return Color(91); }
        static Color BrightGreen() { return Color(92); }
        static Color BrightYellow() { return Color(93); }
        static Color BrightBlue() { return Color(94); }
        static Color BrightMagenta() { return Color(95); }
        static Color BrightCyan() { return Color(96); }
        static Color BrightWhite() { return Color(97); }
        static Color Default() { return Color(39); }

        static Color RGB(int r, int g, int b) {
            return Color(30 + (r > 127 ? 0 : 0) + (g > 127 ? 2 : 0) + (b > 127 ? 4 : 0));
        }
    };

    struct CellStyle {
        Color foreground;
        Color background;
        bool bold;
        bool italic;
        bool underline;

        CellStyle() : foreground(Color::Default()), background(Color(49)), bold(false), italic(false), underline(false) {}

        std::string apply(const std::string& text) const {
            std::ostringstream oss;
            oss << "\033[";
            bool first = true;

            if (bold) { if (!first) oss << ";"; oss << "1"; first = false; }
            if (italic) { if (!first) oss << ";"; oss << "3"; first = false; }
            if (underline) { if (!first) oss << ";"; oss << "4"; first = false; }
            if (foreground.code != 39) { if (!first) oss << ";"; oss << foreground.code; first = false; }
            if (background.code != 49) { if (!first) oss << ";"; oss << background.code; first = false; }

            oss << "m" << text << "\033[0m";
            return oss.str();
        }
    };
}

// ============================================================================
// COLUMN TYPE SYSTEM
// ============================================================================

namespace ColumnType {
    enum Type { STRING = 0, INTEGER = 1, DOUBLE = 2, DATE = 3, BOOLEAN = 4 };
}

namespace Alignment {
    enum Type { LEFT = 0, CENTER = 1, RIGHT = 2 };
}

class Column {
public:
    Column(const std::string& name, ColumnType::Type type = ColumnType::STRING, Alignment::Type align = Alignment::LEFT)
        : _name(name), _type(type), _alignment(align), _width(0) {}

    const std::string& name() const { return _name; }
    ColumnType::Type type() const { return _type; }
    Alignment::Type alignment() const { return _alignment; }
    size_t width() const { return _width; }
    void setWidth(size_t w) { _width = w; }

    char getAlignChar() const {
        if (_alignment == Alignment::CENTER) return 'c';
        if (_alignment == Alignment::RIGHT) return 'r';
        return 'l';
    }

    std::string format(const std::string& value) const {
        if (_type == ColumnType::INTEGER || _type == ColumnType::DOUBLE) return value;
        if (_type == ColumnType::BOOLEAN) return (value == "1" || value == "true") ? "✓" : "✗";
        return value;
    }

private:
    std::string _name;
    ColumnType::Type _type;
    Alignment::Type _alignment;
    size_t _width;
};

// ============================================================================
// ROW - DATA RECORD
// ============================================================================

class Row {
public:
    Row() {}

    void setValue(const std::string& columnName, const std::string& value) {
        _data[columnName] = value;
    }

    std::string getValue(const std::string& columnName) const {
        std::map<std::string, std::string>::const_iterator it = _data.find(columnName);
        if (it != _data.end()) return it->second;
        return "";
    }

    const std::map<std::string, std::string>& data() const { return _data; }

private:
    std::map<std::string, std::string> _data;
};

// ============================================================================
// TABLE - CONTAINER FOR COLUMNS AND ROWS
// ============================================================================

class Table {
public:
    Table() {}

    void addColumn(const Column& col) {
        _columns.push_back(col);
    }

    void addRow(const Row& row) {
        _rows.push_back(row);
    }

    const std::vector<Column>& columns() const { return _columns; }
    std::vector<Column>& columns() { return _columns; }
    const std::vector<Row>& rows() const { return _rows; }
    // NEW: non-const rows accessor so callers can modify rows (REPL/queries)
    std::vector<Row>& rows() { return _rows; }
    
    void clear() {
        _rows.clear();
    }

    size_t columnCount() const { return _columns.size(); }
    size_t rowCount() const { return _rows.size(); }

    void calculateColumnWidths() {
        for (size_t ci = 0; ci < _columns.size(); ++ci) {
            Column& col = _columns[ci];
            size_t maxWidth = Unicode::displayWidth(col.name());

            for (size_t ri = 0; ri < _rows.size(); ++ri) {
                const Row& row = _rows[ri];
                std::string value = col.format(row.getValue(col.name()));
                size_t width = Unicode::displayWidth(value);
                if (width > maxWidth) maxWidth = width;
            }

            col.setWidth(maxWidth);
        }
    }

private:
    std::vector<Column> _columns;
    std::vector<Row> _rows;
};

// ============================================================================
// RENDER CONFIGURATION
// ============================================================================

enum TableStyle {
	TABLE_STYLE_LIGHT = 0,
	TABLE_STYLE_HEAVY,
	TABLE_STYLE_DOUBLE,
	TABLE_STYLE_ROUNDED,
	TABLE_STYLE_MINIMAL
};

struct RenderConfig {
    Unicode::BoxChars boxChars;
    Style::CellStyle headerStyle;
    Style::CellStyle cellStyle;
    Style::CellStyle footerStyle;
    Style::CellStyle borderStyle; // new: style for borders / box chars
    bool showHeader;
    bool showFooter;
    std::string footerText;
    int padding;
    bool autoWidth;

    // NEW: auto-increment ID behavior when loading CSV
    bool autoIncrementId;

    // NEW: body coloring options
    Style::Color body_background;              // background applied to all body cells (if != 49)
    Style::Color color_body;                   // foreground applied to all body cells (if != 39)
    Style::Color even_background;              // background for even rows (overrides body_background if set)
    Style::Color odd_background;               // background for odd rows
    std::map<std::string, Style::Color> color_column; // per-column foreground overrides (column name -> color)
    std::map<std::string, Style::CellStyle> value_styles; // value-based cell rules
    TableStyle tableStyle; // new: choose shape

    RenderConfig() : boxChars(), headerStyle(), cellStyle(), footerStyle(), showHeader(true),
                     showFooter(false), footerText(""), padding(1), autoWidth(true),
                     autoIncrementId(true),
                     body_background(Style::Color(49)), color_body(Style::Color::Default()),
                     even_background(Style::Color(49)), odd_background(Style::Color(49)),
                     tableStyle(TABLE_STYLE_ROUNDED) // default to rounded corners
    {
        headerStyle.bold = true;
        headerStyle.foreground = Style::Color::BrightCyan();
        borderStyle.foreground = Style::Color::BrightWhite();
        // default box chars follow the default table style
        boxChars = Unicode::BoxChars::rounded();
    }

    static RenderConfig elegant() {
        RenderConfig cfg;
        cfg.boxChars = Unicode::BoxChars::doubleLine();
        cfg.headerStyle.bold = true;
        cfg.headerStyle.foreground = Style::Color::BrightWhite();
        cfg.headerStyle.background = Style::Color(44);
        cfg.borderStyle.foreground = Style::Color::BrightWhite();
        cfg.tableStyle = TABLE_STYLE_DOUBLE; // ensure double lines for elegant style
        return cfg;
    }

    static RenderConfig minimal() {
        RenderConfig cfg;
        cfg.padding = 0;
        cfg.headerStyle.bold = true;
        cfg.borderStyle.foreground = Style::Color::Default();
        cfg.tableStyle = TABLE_STYLE_MINIMAL; // ensure minimal style
        return cfg;
    }
};

// ============================================================================
// TABLE RENDERER - BUFFERED OUTPUT
// ============================================================================

class TableRenderer {
public:
    TableRenderer(const RenderConfig& config = RenderConfig())
        : _config(config) {}
    
    std::string render(Table& table) {
        if (_config.autoWidth) {
            table.calculateColumnWidths();
        }
        
        _buffer.clear();
        _buffer.str("");

        const std::vector<Column>& cols = table.columns();

        // Top/header/data rendering (unchanged logic)
        renderTopBorder(table);
        if (_config.showHeader) {
            renderHeader(table);
            renderHeaderSeparator(table);
        }
        renderRows(table);

        // Decide footer placement: inside if footer fits, otherwise print outside
        size_t totalInnerWidth = 0;
        for (size_t i = 0; i < cols.size(); ++i) {
            totalInnerWidth += cols[i].width() + 2 * _config.padding;
            if (i + 1 < cols.size()) totalInnerWidth += 1; // join spacing between columns
        }

        if (_config.showFooter && !_config.footerText.empty()) {
            size_t footerWidth = Unicode::displayWidth(_config.footerText);
            if (footerWidth > totalInnerWidth) {
                // Footer too wide: close table first, then print footer as its own styled line
                renderBottomBorder(table);
                _buffer << _config.footerStyle.apply(_config.footerText) << "\n";
            } else {
                // Footer fits: render inside table as before
                renderFooterSeparator(table);
                renderFooter(table);
                renderBottomBorder(table);
            }
        } else {
            // No footer: just close table
            renderBottomBorder(table);
        }

        return _buffer.str();
    }
    
    void setConfig(const RenderConfig& config) {
        _config = config;
    }

private:
    RenderConfig _config;
    std::ostringstream _buffer;

    // helper to output a box character (styled)
    void putBorder(const std::string& s) {
        // apply border style around the given string
        _buffer << _config.borderStyle.apply(s);
    }
    
    void renderTopBorder(const Table& table) {
        putBorder(_config.boxChars.topLeft);

        const std::vector<Column>& cols = table.columns();
        for (size_t i = 0; i < cols.size(); ++i) {
            size_t width = cols[i].width() + 2 * _config.padding;
            putBorder(repeat(_config.boxChars.horizontal, width));

            if (i < cols.size() - 1) {
                putBorder(_config.boxChars.teeTop);
            }
        }

        putBorder(_config.boxChars.topRight);
        _buffer << "\n";
    }
    
    void renderHeader(const Table& table) {
        putBorder(_config.boxChars.vertical);

        const std::vector<Column>& cols = table.columns();
        for (size_t i = 0; i < cols.size(); ++i) {
            std::string content = Unicode::pad(cols[i].name(), cols[i].width(), 'c');

            _buffer << repeat(" ", _config.padding);
            _buffer << _config.headerStyle.apply(content);
            _buffer << repeat(" ", _config.padding);
            putBorder(_config.boxChars.vertical);
        }

        _buffer << "\n";
    }

    void renderHeaderSeparator(const Table& table) {
        putBorder(_config.boxChars.teeLeft);

        const std::vector<Column>& cols = table.columns();
        for (size_t i = 0; i < cols.size(); ++i) {
            size_t width = cols[i].width() + 2 * _config.padding;
            putBorder(repeat(_config.boxChars.horizontal, width));

            if (i < cols.size() - 1) {
                putBorder(_config.boxChars.cross);
            }
        }

        putBorder(_config.boxChars.teeRight);
        _buffer << "\n";
    }
    
    void renderRows(const Table& table) {
        const std::vector<Column>& cols = table.columns();
        const std::vector<Row>& rows = table.rows();

        for (size_t r = 0; r < rows.size(); ++r) {
            putBorder(_config.boxChars.vertical);

            for (size_t c = 0; c < cols.size(); ++c) {
                std::string raw = cols[c].format(rows[r].getValue(cols[c].name()));
                std::string content = Unicode::pad(raw, cols[c].width(), cols[c].getAlignChar());

                // Build effective cell style: start from default cellStyle
                Style::CellStyle eff = _config.cellStyle;

                // global body foreground override
                if (_config.color_body.code != Style::Color::Default().code) {
                    eff.foreground = _config.color_body;
                }

                // per-column foreground override (higher priority)
                std::map<std::string, Style::Color>::const_iterator itcol =
                    _config.color_column.find(cols[c].name());
                if (itcol != _config.color_column.end()) {
                    eff.foreground = itcol->second;
                }

                // background: base is body_background unless row stripe overrides
                if (_config.body_background.code != 49) {
                    eff.background = _config.body_background;
                }

                // row stripe overrides
                if ((r % 2) == 0) { // even row (0-based)
                    if (_config.even_background.code != 49) eff.background = _config.even_background;
                } else {
                    if (_config.odd_background.code != 49) eff.background = _config.odd_background;
                }

                // NEW: value-based styling (case-insensitive)
                std::string key = raw;
                strcase_toggle(&key, 1); // lowercase
                std::map<std::string, Style::CellStyle>::const_iterator itv = _config.value_styles.find(key);
                if (itv != _config.value_styles.end()) {
                    const Style::CellStyle &rule = itv->second;
                    // override foreground if provided (non-default)
                    if (rule.foreground.code != Style::Color::Default().code) eff.foreground = rule.foreground;
                    // override background if provided (non-default)
                    if (rule.background.code != 49) eff.background = rule.background;
                    // override text attributes if requested (true means enable)
                    if (rule.bold) eff.bold = true;
                    if (rule.italic) eff.italic = true;
                    if (rule.underline) eff.underline = true;
                }

                _buffer << repeat(" ", _config.padding);
                _buffer << eff.apply(content);
                _buffer << repeat(" ", _config.padding);
                putBorder(_config.boxChars.vertical);
            }

            _buffer << "\n";
        }
    }
    
    void renderFooterSeparator(const Table& table) {
        putBorder(_config.boxChars.teeLeft);

        const std::vector<Column>& cols = table.columns();
        for (size_t i = 0; i < cols.size(); ++i) {
            size_t width = cols[i].width() + 2 * _config.padding;
            putBorder(repeat(_config.boxChars.horizontal, width));

            if (i < cols.size() - 1) {
                putBorder(_config.boxChars.cross);
            }
        }

        putBorder(_config.boxChars.teeRight);
        _buffer << "\n";
    }
    
    void renderFooter(const Table& table) {
        putBorder(_config.boxChars.vertical);

        size_t totalWidth = 0;
        const std::vector<Column>& cols = table.columns();
        for (size_t i = 0; i < cols.size(); ++i) {
            totalWidth += cols[i].width() + 2 * _config.padding + 1;
        }
        totalWidth -= 1;

        std::string content = Unicode::pad(_config.footerText, totalWidth - 2, 'c');
        _buffer << " " << _config.footerStyle.apply(content) << " ";
        putBorder(_config.boxChars.vertical);
        _buffer << "\n";
    }
    
    void renderBottomBorder(const Table& table) {
        putBorder(_config.boxChars.bottomLeft);

        const std::vector<Column>& cols = table.columns();
        for (size_t i = 0; i < cols.size(); ++i) {
            size_t width = cols[i].width() + 2 * _config.padding;
            putBorder(repeat(_config.boxChars.horizontal, width));

            if (i < cols.size() - 1) {
                putBorder(_config.boxChars.teeBottom);
            }
        }

        putBorder(_config.boxChars.bottomRight);
        _buffer << "\n";
    }
    
    std::string repeat(const std::string& s, size_t n) {
        std::string result;
        for (size_t i = 0; i < n; ++i) {
            result += s;
        }
        return result;
    }
};

// ============================================================================
// CSV PARSER
// ============================================================================

class CsvParser {
public:
    static Table parse(const std::string& path, bool hasHeader = true, const RenderConfig& cfg = RenderConfig()) {
        Table table;
        std::ifstream file;
        file.open(path.c_str());
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + path);
        }

        std::string line;
        bool isFirstLine = true;
        std::vector<std::string> headers;

        bool insertedIdColumn = false;
        size_t nextAutoId = 1;

        while (std::getline(file, line)) {
            std::vector<std::string> fields = parseLine(line);

            if (isFirstLine && hasHeader) {
                headers = fields;
                // detect existing ID header (case-insensitive, Unicode-aware)
                bool hasId = false;
                for (size_t i = 0; i < headers.size(); ++i) {
                    std::string htmp = headers[i];
                    strcase_toggle(&htmp, 1); // to lower (mod==1)
                    if (htmp == "id") { hasId = true; break; }
                }
                // if requested, auto-insert ID as first column
                if (cfg.autoIncrementId && !hasId) {
                    headers.insert(headers.begin(), std::string("ID"));
                    insertedIdColumn = true;
                }
                for (size_t i = 0; i < headers.size(); ++i) {
                    table.addColumn(Column(headers[i]));
                }
                isFirstLine = false;
            } else {
                if (headers.empty()) {
                    // no header provided: create Column1..N; optionally prepend ID
                    for (size_t i = 0; i < fields.size(); ++i) {
                        std::ostringstream oss;
                        oss << (i + 1);
                        headers.push_back(std::string("Column") + oss.str());
                    }
                    if (cfg.autoIncrementId) {
                        headers.insert(headers.begin(), std::string("ID"));
                        insertedIdColumn = true;
                    }
                    for (size_t i = 0; i < headers.size(); ++i) {
                        table.addColumn(Column(headers[i]));
                    }
                }

                Row row;
                size_t fi = 0;
                if (insertedIdColumn) {
                    std::ostringstream idss;
                    idss << nextAutoId++;
                    row.setValue(headers[0], idss.str());
                    // fill fields starting at headers[1]
                    for (size_t i = 1; i < headers.size() && fi < fields.size(); ++i, ++fi) {
                        row.setValue(headers[i], fields[fi]);
                    }
                } else {
                    for (size_t i = 0; i < fields.size() && i < headers.size(); ++i) {
                        row.setValue(headers[i], fields[i]);
                    }
                }
                table.addRow(row);
            }
        }

        return table;
    }

private:
    static std::vector<std::string> parseLine(const std::string& line) {
        std::vector<std::string> fields;
        std::string field;
        bool inQuotes = false;
        
        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];
            
            if (c == '"') {
                if (inQuotes && i + 1 < line.length() && line[i + 1] == '"') {
                    field += '"';
                    ++i;
                } else {
                    inQuotes = !inQuotes;
                }
            } else if (c == ',' && !inQuotes) {
                fields.push_back(trim(field));
                field.clear();
            } else {
                field += c;
            }
        }
        
        fields.push_back(trim(field));
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
// DATABASE - HIGH LEVEL ORCHESTRATOR
// ============================================================================

class Database {
public:
    Database() {}

    void loadFromCsv(const std::string& path, bool hasHeader = true, const RenderConfig& cfg = RenderConfig()) {
        _table = CsvParser::parse(path, hasHeader, cfg);
    }

    void addColumn(const std::string& name, ColumnType::Type type = ColumnType::STRING,
                   Alignment::Type align = Alignment::LEFT) {
        _table.addColumn(Column(name, type, align));
    }

    void addRow(const std::map<std::string, std::string>& data) {
        Row row;
        for (std::map<std::string, std::string>::const_iterator it = data.begin(); it != data.end(); ++it) {
            row.setValue(it->first, it->second);
        }
        _table.addRow(row);
    }

    // expose table for callers (needed by tests/util modules)
    Table& table() { return _table; }
    const Table& table() const { return _table; }

    // Render the table using TableRenderer so styles are always applied
    std::string render(const RenderConfig& config = RenderConfig()) const {
        TableRenderer renderer(config);
        return renderer.render(const_cast<Table&>(_table));
    }

    std::vector<Row> where(const std::string& column, const std::string& value) const {
        std::vector<Row> results;
        const std::vector<Row>& rows = _table.rows();
        for (size_t i = 0; i < rows.size(); ++i) {
            if (rows[i].getValue(column) == value) {
                results.push_back(rows[i]);
            }
        }
        return results;
    }

    size_t count() const { return _table.rowCount(); }

private:
    Table _table;
};

#endif // DATABASE_HPP