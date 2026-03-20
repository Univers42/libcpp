#ifndef COLOR_DECORATOR_HPP
# define COLOR_DECORATOR_HPP

#include <iostream>
#include <string>
#include "Decorator.hpp"
#include "Srgb.hpp"

/**
 * @class ColorDecorator
 * @brief Decorator that applies SRGB colors and text styles to messages
 */
class ColorDecorator : public Decorator {
    private:
        Srgb fgColor;   // Foreground color
        Srgb bgColor;   // Background color
        int style;      // Text style flags

    public:
        // Style flags
        static const int BOLD      = 1 << 0;
        static const int DIM       = 1 << 1;
        static const int ITALIC    = 1 << 2;
        static const int UNDERLINE = 1 << 3;
        static const int BLINK     = 1 << 4;
        static const int REVERSE   = 1 << 5;

        ColorDecorator(const std::string& msg, const Srgb& fg);
        ColorDecorator(const std::string& msg, const Srgb& fg, const Srgb& bg);
        ColorDecorator(const std::string& msg, const Srgb& fg, int style_flags);
        ColorDecorator(const std::string& msg, const Srgb& fg, const Srgb& bg, int style_flags);
        virtual ~ColorDecorator();

        std::string decorate() const;
        
        void setForeground(const Srgb& fg);
        void setBackground(const Srgb& bg);
        void setStyle(int flags);
        void addStyle(int flags);
};

#endif