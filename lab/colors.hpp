/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   colors.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 02:40:26 by marvin            #+#    #+#             */
/*   Updated: 2025/12/24 02:40:26 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLORS_HPP
#define COLORS_HPP

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip> // added for setw/setfill
#include <cstdlib> // added for strtol/atoi

namespace Colors {

// ============================================================================
// RGB COLOR CLASS
// ============================================================================

class RGB {
public:
    unsigned char r, g, b;
    
    RGB() : r(0), g(0), b(0) {}
    RGB(unsigned char red, unsigned char green, unsigned char blue) 
        : r(red), g(green), b(blue) {}
    
    // Hex string constructor (C++98-safe)
    RGB(const std::string& hex) {
        r = g = b = 0;
        if (hex.empty()) return;
        std::string h = hex;
        if (h[0] == '#') h = h.substr(1);
        if (h.length() >= 6) {
            r = (unsigned char)strtol(h.substr(0,2).c_str(), NULL, 16);
            g = (unsigned char)strtol(h.substr(2,2).c_str(), NULL, 16);
            b = (unsigned char)strtol(h.substr(4,2).c_str(), NULL, 16);
        }
    }

    // Parse rgb(r,g,b) format (C++98-safe) - added
    static RGB parseRgbString(const std::string& str) {
        RGB out;
        size_t start = str.find('(');
        size_t end = str.find(')');
        if (start == std::string::npos || end == std::string::npos) return out;
        std::string body = str.substr(start + 1, end - start - 1);
        size_t p1 = body.find(',');
        size_t p2 = (p1 == std::string::npos) ? std::string::npos : body.find(',', p1 + 1);
        if (p1 == std::string::npos || p2 == std::string::npos) return out;
        out.r = (unsigned char)atoi(body.substr(0, p1).c_str());
        out.g = (unsigned char)atoi(body.substr(p1 + 1, p2 - p1 - 1).c_str());
        out.b = (unsigned char)atoi(body.substr(p2 + 1).c_str());
        return out;
    }
    
    std::string toHex() const {
        std::ostringstream oss;
        oss << "#" 
            << std::hex << std::setfill('0') << std::setw(2) << (int)r
            << std::setw(2) << (int)g
            << std::setw(2) << (int)b;
        return oss.str();
    }
    
    // Convert to ANSI 256-color palette index (approximation)
    int to256() const {
        if (r == g && g == b) {
            // Grayscale
            if (r < 8) return 16;
            if (r > 248) return 231;
            return 232 + (r - 8) / 10;
        }
        // Color cube
        int rIndex = (r * 5) / 255;
        int gIndex = (g * 5) / 255;
        int bIndex = (b * 5) / 255;
        return 16 + 36 * rIndex + 6 * gIndex + bIndex;
    }
    
    // Convert to basic ANSI color (0-15)
    int toAnsi() const {
        int base = 0;
        if (r > 127) base |= 1;
        if (g > 127) base |= 2;
        if (b > 127) base |= 4;
        bool bright = (r > 191 || g > 191 || b > 191);
        return (bright ? 90 : 30) + base;
    }
    
    // Luminance calculation (perceived brightness)
    double luminance() const {
        return 0.299 * r + 0.587 * g + 0.114 * b;
    }
    
    // Color manipulation
    RGB lighten(double factor) const {
        factor = std::max(0.0, std::min(1.0, factor));
        return RGB(
            std::min(255, (int)(r + (255 - r) * factor)),
            std::min(255, (int)(g + (255 - g) * factor)),
            std::min(255, (int)(b + (255 - b) * factor))
        );
    }
    
    RGB darken(double factor) const {
        factor = std::max(0.0, std::min(1.0, factor));
        return RGB(
            std::max(0, (int)(r * (1.0 - factor))),
            std::max(0, (int)(g * (1.0 - factor))),
            std::max(0, (int)(b * (1.0 - factor)))
        );
    }
    
    RGB saturate(double factor) const {
        double gray = 0.299 * r + 0.587 * g + 0.114 * b;
        return RGB(
            std::max(0, std::min(255, (int)(gray + (r - gray) * (1 + factor)))),
            std::max(0, std::min(255, (int)(gray + (g - gray) * (1 + factor)))),
            std::max(0, std::min(255, (int)(gray + (b - gray) * (1 + factor))))
        );
    }
    
    RGB desaturate(double factor) const {
        return saturate(-factor);
    }
    
    // Mix two colors
    static RGB mix(const RGB& c1, const RGB& c2, double ratio = 0.5) {
        ratio = std::max(0.0, std::min(1.0, ratio));
        return RGB(
            (unsigned char)(c1.r * (1 - ratio) + c2.r * ratio),
            (unsigned char)(c1.g * (1 - ratio) + c2.g * ratio),
            (unsigned char)(c1.b * (1 - ratio) + c2.b * ratio)
        );
    }
    
    // Complementary color
    RGB complement() const {
        return RGB(255 - r, 255 - g, 255 - b);
    }
    
    // Operators
    bool operator==(const RGB& other) const {
        return r == other.r && g == other.g && b == other.b;
    }
    
    bool operator!=(const RGB& other) const {
        return !(*this == other);
    }
};

// ============================================================================
// COLOR PALETTE - NAMED COLORS
// ============================================================================

class Palette {
public:
    // Basic colors
    static RGB Black()       { return RGB(0, 0, 0); }
    static RGB White()       { return RGB(255, 255, 255); }
    static RGB Red()         { return RGB(255, 0, 0); }
    static RGB Green()       { return RGB(0, 255, 0); }
    static RGB Blue()        { return RGB(0, 0, 255); }
    static RGB Yellow()      { return RGB(255, 255, 0); }
    static RGB Magenta()     { return RGB(255, 0, 255); }
    static RGB Cyan()        { return RGB(0, 255, 255); }
    
    // Grayscale
    static RGB Gray()        { return RGB(128, 128, 128); }
    static RGB DarkGray()    { return RGB(64, 64, 64); }
    static RGB LightGray()   { return RGB(192, 192, 192); }
    
    // Extended colors
    static RGB Orange()      { return RGB(255, 165, 0); }
    static RGB Purple()      { return RGB(128, 0, 128); }
    static RGB Pink()        { return RGB(255, 192, 203); }
    static RGB Brown()       { return RGB(165, 42, 42); }
    static RGB Lime()        { return RGB(50, 205, 50); }
    static RGB Navy()        { return RGB(0, 0, 128); }
    static RGB Teal()        { return RGB(0, 128, 128); }
    static RGB Maroon()      { return RGB(128, 0, 0); }
    static RGB Olive()       { return RGB(128, 128, 0); }
    static RGB Aqua()        { return RGB(0, 255, 255); }
    
    // Material Design colors
    static RGB MaterialRed()       { return RGB(244, 67, 54); }
    static RGB MaterialPink()      { return RGB(233, 30, 99); }
    static RGB MaterialPurple()    { return RGB(156, 39, 176); }
    static RGB MaterialDeepPurple(){ return RGB(103, 58, 183); }
    static RGB MaterialIndigo()    { return RGB(63, 81, 181); }
    static RGB MaterialBlue()      { return RGB(33, 150, 243); }
    static RGB MaterialLightBlue() { return RGB(3, 169, 244); }
    static RGB MaterialCyan()      { return RGB(0, 188, 212); }
    static RGB MaterialTeal()      { return RGB(0, 150, 136); }
    static RGB MaterialGreen()     { return RGB(76, 175, 80); }
    static RGB MaterialLightGreen(){ return RGB(139, 195, 74); }
    static RGB MaterialLime()      { return RGB(205, 220, 57); }
    static RGB MaterialYellow()    { return RGB(255, 235, 59); }
    static RGB MaterialAmber()     { return RGB(255, 193, 7); }
    static RGB MaterialOrange()    { return RGB(255, 152, 0); }
    static RGB MaterialDeepOrange(){ return RGB(255, 87, 34); }
    
    // Get color by name
    static RGB get(const std::string& name) {
        static std::map<std::string, RGB> colors;
        if (colors.empty()) {
            colors["black"] = Black(); colors["white"] = White();
            colors["red"] = Red(); colors["green"] = Green(); colors["blue"] = Blue();
            colors["yellow"] = Yellow(); colors["magenta"] = Magenta(); colors["cyan"] = Cyan();
            colors["gray"] = Gray(); colors["orange"] = Orange(); colors["purple"] = Purple();
            colors["pink"] = Pink(); colors["brown"] = Brown(); colors["lime"] = Lime();
            colors["navy"] = Navy(); colors["teal"] = Teal(); colors["maroon"] = Maroon();
            colors["olive"] = Olive(); colors["aqua"] = Aqua();
        }
        std::map<std::string, RGB>::const_iterator it = colors.find(name);
        return (it != colors.end()) ? it->second : RGB();
    }
};

// ============================================================================
// GRADIENT GENERATOR
// ============================================================================

class Gradient {
public:
    std::vector<RGB> colors;
    
    Gradient() {}
    Gradient(const RGB& start, const RGB& end) {
        colors.push_back(start);
        colors.push_back(end);
    }
    
    Gradient& addStop(const RGB& color) {
        colors.push_back(color);
        return *this;
    }
    
    RGB at(double position) const {
        if (colors.empty()) return RGB();
        if (colors.size() == 1) return colors[0];
        
        position = std::max(0.0, std::min(1.0, position));
        double scaledPos = position * (colors.size() - 1);
        size_t index = (size_t)scaledPos;
        
        if (index >= colors.size() - 1) return colors.back();
        
        double ratio = scaledPos - index;
        return RGB::mix(colors[index], colors[index + 1], ratio);
    }
    
    std::vector<RGB> generate(size_t steps) const {
        std::vector<RGB> result;
        for (size_t i = 0; i < steps; ++i) {
            double pos = (double)i / (steps - 1);
            result.push_back(at(pos));
        }
        return result;
    }
    
    // Predefined gradients
    static Gradient Rainbow() {
        Gradient g;
        g.addStop(RGB(255, 0, 0))     // Red
         .addStop(RGB(255, 127, 0))   // Orange
         .addStop(RGB(255, 255, 0))   // Yellow
         .addStop(RGB(0, 255, 0))     // Green
         .addStop(RGB(0, 0, 255))     // Blue
         .addStop(RGB(75, 0, 130))    // Indigo
         .addStop(RGB(148, 0, 211));  // Violet
        return g;
    }
    
    static Gradient Sunset() {
        return Gradient()
            .addStop(RGB(255, 94, 77))
            .addStop(RGB(255, 154, 68))
            .addStop(RGB(250, 208, 137));
    }
    
    static Gradient Ocean() {
        return Gradient()
            .addStop(RGB(0, 48, 73))
            .addStop(RGB(0, 119, 182))
            .addStop(RGB(52, 232, 255));
    }
    
    static Gradient Fire() {
        return Gradient()
            .addStop(RGB(255, 0, 0))
            .addStop(RGB(255, 127, 0))
            .addStop(RGB(255, 255, 0));
    }
    
    static Gradient Forest() {
        return Gradient()
            .addStop(RGB(34, 139, 34))
            .addStop(RGB(144, 238, 144))
            .addStop(RGB(0, 100, 0));
    }
};

// ============================================================================
// COLOR SCHEME GENERATOR
// ============================================================================

class ColorScheme {
public:
    // Generate analogous colors (adjacent on color wheel)
    static std::vector<RGB> analogous(const RGB& base, int count = 3) {
        std::vector<RGB> colors;
        colors.push_back(base);
        // This is simplified - true HSL conversion would be better
        for (int i = 1; i < count; ++i) {
            double shift = 0.1 * i;
            RGB shifted(
                (unsigned char)std::min(255, (int)(base.r * (1 + shift))),
                (unsigned char)std::min(255, (int)(base.g * (1 - shift * 0.5))),
                (unsigned char)std::min(255, (int)(base.b * (1 + shift * 0.3)))
            );
            colors.push_back(shifted);
        }
        return colors;
    }
    
    // Generate complementary color
    static RGB complementary(const RGB& base) {
        return base.complement();
    }
    
    // Generate triadic colors (evenly spaced on color wheel)
    static std::vector<RGB> triadic(const RGB& base) {
        std::vector<RGB> colors;
        colors.push_back(base);
        colors.push_back(RGB(base.b, base.r, base.g));
        colors.push_back(RGB(base.g, base.b, base.r));
        return colors;
    }
    
    // Generate monochromatic scheme (shades of one color)
    static std::vector<RGB> monochromatic(const RGB& base, int count = 5) {
        std::vector<RGB> colors;
        for (int i = 0; i < count; ++i) {
            double factor = (double)i / (count - 1);
            colors.push_back(base.darken(factor * 0.5));
        }
        return colors;
    }
};

// ============================================================================
// ANSI COLOR FORMATTER
// ============================================================================

class Formatter {
public:
    enum Mode {
        ANSI_16,      // Basic 16 colors
        ANSI_256,     // 256 color palette
        TRUE_COLOR    // 24-bit RGB
    };
    
    Mode mode;
    
    Formatter(Mode m = ANSI_256) : mode(m) {}
    
    // Format text with foreground color
    std::string fg(const std::string& text, const RGB& color) const {
        return colorize(text, color, false);
    }
    
    // Format text with background color
    std::string bg(const std::string& text, const RGB& color) const {
        return colorize(text, color, true);
    }
    
    // Format with both foreground and background
    std::string colorize(const std::string& text, const RGB& fg, const RGB& bg) const {
        std::ostringstream oss;
        oss << getEscapeCode(fg, false) << getEscapeCode(bg, true) 
            << text << "\033[0m";
        return oss.str();
    }
    
    // Bold text
    static std::string bold(const std::string& text) {
        return "\033[1m" + text + "\033[0m";
    }
    
    // Italic text
    static std::string italic(const std::string& text) {
        return "\033[3m" + text + "\033[0m";
    }
    
    // Underline text
    static std::string underline(const std::string& text) {
        return "\033[4m" + text + "\033[0m";
    }
    
    // Strike-through
    static std::string strikethrough(const std::string& text) {
        return "\033[9m" + text + "\033[0m";
    }
    
    // Reset all formatting
    static std::string reset() {
        return "\033[0m";
    }
    
private:
    std::string colorize(const std::string& text, const RGB& color, bool background) const {
        std::ostringstream oss;
        oss << getEscapeCode(color, background) << text << "\033[0m";
        return oss.str();
    }
    
    std::string getEscapeCode(const RGB& color, bool background) const {
        std::ostringstream oss;
        oss << "\033[";
        
        switch (mode) {
            case ANSI_16:
                oss << (background ? color.toAnsi() + 10 : color.toAnsi());
                break;
            case ANSI_256:
                oss << (background ? "48;5;" : "38;5;") << color.to256();
                break;
            case TRUE_COLOR:
                oss << (background ? "48;2;" : "38;2;") 
                    << (int)color.r << ";" << (int)color.g << ";" << (int)color.b;
                break;
        }
        
        oss << "m";
        return oss.str();
    }
};

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

// Parse color from string (hex, rgb, or name)
inline RGB parse(const std::string& str) {
    if (str.empty()) return RGB();
    if (str[0] == '#') {
        return RGB(str);
    }
    if (str.size() >= 4 && str.substr(0,4) == "rgb(") {
        return RGB::parseRgbString(str);
    }
    return Palette::get(str);
}

// Color distance (perceptual difference)
inline double distance(const RGB& c1, const RGB& c2) {
    // Using weighted Euclidean distance
    double dr = c1.r - c2.r;
    double dg = c1.g - c2.g;
    double db = c1.b - c2.b;
    return std::sqrt(dr * dr * 0.299 + dg * dg * 0.587 + db * db * 0.114);
}

// Find closest named color
inline std::string closestName(const RGB& color) {
    double minDist = 1e9;
    std::string closest = "unknown";
    std::vector< std::pair<std::string, RGB> > namedColors;
    namedColors.push_back(std::make_pair(std::string("black"), Palette::Black()));
    namedColors.push_back(std::make_pair(std::string("white"), Palette::White()));
    namedColors.push_back(std::make_pair(std::string("red"), Palette::Red()));
    namedColors.push_back(std::make_pair(std::string("green"), Palette::Green()));
    namedColors.push_back(std::make_pair(std::string("blue"), Palette::Blue()));
    namedColors.push_back(std::make_pair(std::string("yellow"), Palette::Yellow()));
    namedColors.push_back(std::make_pair(std::string("magenta"), Palette::Magenta()));
    namedColors.push_back(std::make_pair(std::string("cyan"), Palette::Cyan()));
    namedColors.push_back(std::make_pair(std::string("gray"), Palette::Gray()));
    namedColors.push_back(std::make_pair(std::string("orange"), Palette::Orange()));
    namedColors.push_back(std::make_pair(std::string("purple"), Palette::Purple()));
    namedColors.push_back(std::make_pair(std::string("pink"), Palette::Pink()));

    for (size_t i = 0; i < namedColors.size(); ++i) {
        double dist = distance(color, namedColors[i].second);
        if (dist < minDist) {
            minDist = dist;
            closest = namedColors[i].first;
        }
    }
    return closest;
}

} // namespace Colors

#endif // COLORS_HPP