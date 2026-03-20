#include "VerboseDecorator.hpp"

VerboseDecorator::VerboseDecorator(const std::string& msg, const Srgb& col, int style_flags)
    : Decorator(msg), color(col), style(style_flags) {}

VerboseDecorator::~VerboseDecorator() {}

std::string VerboseDecorator::decorate() const {
#if VERBOSE
    std::string result;
    
    // Add color
    result += color.toAnsi();
    
    // Add styles
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
#else
    return getMessage();
#endif
}

void VerboseDecorator::setStyle(int flags) {
    style = flags;
}

void VerboseDecorator::addStyle(int flags) {
    style |= flags;
}

void VerboseDecorator::removeStyle(int flags) {
    style &= ~flags;
}

int VerboseDecorator::getStyle() const {
    return style;
}

Srgb VerboseDecorator::getColor() const {
    return color;
}
