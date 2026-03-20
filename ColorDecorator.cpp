#include "ColorDecorator.hpp"

ColorDecorator::ColorDecorator(const std::string& msg, const Srgb& fg)
    : Decorator(msg), fgColor(fg), bgColor(0x000000), style(0) {}

ColorDecorator::ColorDecorator(const std::string& msg, const Srgb& fg, const Srgb& bg)
    : Decorator(msg), fgColor(fg), bgColor(bg), style(0) {}

ColorDecorator::ColorDecorator(const std::string& msg, const Srgb& fg, int style_flags)
    : Decorator(msg), fgColor(fg), bgColor(0x000000), style(style_flags) {}

ColorDecorator::ColorDecorator(const std::string& msg, const Srgb& fg, const Srgb& bg, int style_flags)
    : Decorator(msg), fgColor(fg), bgColor(bg), style(style_flags) {}

ColorDecorator::~ColorDecorator() {}

std::string ColorDecorator::decorate() const {
    std::string result;
    
    // Apply foreground color
    result += fgColor.toAnsi();
    
    // Apply styles
    if (style & BOLD)      result += "\033[1m";
    if (style & DIM)       result += "\033[2m";
    if (style & ITALIC)    result += "\033[3m";
    if (style & UNDERLINE) result += "\033[4m";
    if (style & BLINK)     result += "\033[5m";
    if (style & REVERSE)   result += "\033[7m";
    
    // Add message
    result += getMessage();
    
    // Reset
    result += "\033[0m";
    
    return result;
}

void ColorDecorator::setForeground(const Srgb& fg) {
    fgColor = fg;
}

void ColorDecorator::setBackground(const Srgb& bg) {
    bgColor = bg;
}

void ColorDecorator::setStyle(int flags) {
    style = flags;
}

void ColorDecorator::addStyle(int flags) {
    style |= flags;
}
