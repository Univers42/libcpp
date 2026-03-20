#ifndef SRGB_HPP
# define SRGB_HPP

#include <string>
#include <sstream>
#include <iomanip>

/**
 * @class Srgb
 * @brief ANSI 256-color manager with bitwise operators and text style support
 */
class Srgb {
    private:
        unsigned int color;  // 32-bit: 0xRRGGBB

    public:
        // Constructors
        Srgb();
        Srgb(unsigned int rgb);
        Srgb(unsigned char r, unsigned char g, unsigned char b);
        ~Srgb();

        // Getters
        unsigned char getR() const;
        unsigned char getG() const;
        unsigned char getB() const;

        // Setters
        void setR(unsigned char r);
        void setG(unsigned char g);
        void setB(unsigned char b);

        // Bitwise operators
        Srgb operator|(const Srgb& other) const;
        Srgb operator&(const Srgb& other) const;
        Srgb operator^(const Srgb& other) const;
        Srgb operator~() const;

        // Conversion methods
        std::string toAnsi() const;
        std::string toBgAnsi() const;
        std::string toHex() const;
};

#endif