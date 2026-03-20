#include "Srgb.hpp"

Srgb::Srgb() : color(0xFFFFFF) {}

Srgb::Srgb(unsigned int rgb) : color(rgb & 0xFFFFFF) {}

Srgb::Srgb(unsigned char r, unsigned char g, unsigned char b) 
    : color((r << 16) | (g << 8) | b) {}

Srgb::~Srgb() {}

unsigned char Srgb::getR() const {
    return (color >> 16) & 0xFF;
}

unsigned char Srgb::getG() const {
    return (color >> 8) & 0xFF;
}

unsigned char Srgb::getB() const {
    return color & 0xFF;
}

void Srgb::setR(unsigned char r) {
    color = (color & 0x00FFFF) | (r << 16);
}

void Srgb::setG(unsigned char g) {
    color = (color & 0xFF00FF) | (g << 8);
}

void Srgb::setB(unsigned char b) {
    color = (color & 0xFFFF00) | b;
}

Srgb Srgb::operator|(const Srgb& other) const {
    return Srgb(color | other.color);
}

Srgb Srgb::operator&(const Srgb& other) const {
    return Srgb(color & other.color);
}

Srgb Srgb::operator^(const Srgb& other) const {
    return Srgb(color ^ other.color);
}

Srgb Srgb::operator~() const {
    return Srgb(~color & 0xFFFFFF);
}

std::string Srgb::toAnsi() const {
    std::stringstream ss;
    ss << "\033[38;2;" << static_cast<int>(getR()) << ";" 
       << static_cast<int>(getG()) << ";" << static_cast<int>(getB()) << "m";
    return ss.str();
}

std::string Srgb::toBgAnsi() const {
    std::stringstream ss;
    ss << "\033[48;2;" << static_cast<int>(getR()) << ";"
       << static_cast<int>(getG()) << ";" << static_cast<int>(getB()) << "m";
    return ss.str();
}

std::string Srgb::toHex() const {
    std::stringstream ss;
    ss << "#" << std::hex << std::setfill('0') << std::setw(6) << color;
    return ss.str();
}