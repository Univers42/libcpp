#ifndef VERBOSE_DECORATOR_HPP
# define VERBOSE_DECORATOR_HPP

#include <iostream>
#include <string>
#include "Decorator.hpp"
#include "Srgb.hpp"

/**
 * @class VerboseDecorator
 * @brief Enhanced decorator with ANSI colors and text styles
 * Supports: bold, dim, italic, underline, blink, reverse
 */
class VerboseDecorator : public Decorator {
    private:
        Srgb color;
        int style;  // Bitfield for text styles

    public:
        // Style flags (can be OR'd together)
        static const int BOLD      = 1 << 0;   // 0x01
        static const int DIM       = 1 << 1;   // 0x02
        static const int ITALIC    = 1 << 2;   // 0x04
        static const int UNDERLINE = 1 << 3;   // 0x08
        static const int BLINK     = 1 << 4;   // 0x10
        static const int REVERSE   = 1 << 5;   // 0x20

        VerboseDecorator(const std::string& msg, const Srgb& col, int style_flags = 0);
        virtual ~VerboseDecorator();

        std::string decorate() const;
        
        // Accessors
        Srgb getColor() const;
        int getStyle() const;

        // Style management
        void setStyle(int flags);
        void addStyle(int flags);
        void removeStyle(int flags);
};

#endif