/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 22:06:02 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/term/color.hpp"
#include <vector>

namespace libcpp
{

/* ══════════════════════════════════════════════════════════════════════════
 *  Srgb — Orthodox Canonical Form
 * ═════════════════════════════════════════════════════════════════════════ */

Srgb::Srgb() : _color(0xFFFFFF) {}

Srgb::Srgb(unsigned int rgb) : _color(rgb & 0xFFFFFF) {}

Srgb::Srgb(unsigned char r, unsigned char g, unsigned char b)
	: _color(((unsigned int)r << 16) | ((unsigned int)g << 8) | b) {}

Srgb::Srgb(const Srgb& other) : _color(other._color) {}

Srgb& Srgb::operator=(const Srgb& other)
{
	if (this != &other)
		_color = other._color;
	return *this;
}

Srgb::~Srgb() {}

/* ── static factories ──────────────────────────────────────────────────── */

Srgb Srgb::from_hex(const std::string& hex)
{
	std::string h = hex;
	if (!h.empty() && h[0] == '#')
		h = h.substr(1);
	if (h.length() < 6)
		return Srgb(0, 0, 0);
	unsigned char r = (unsigned char)std::strtol(h.substr(0, 2).c_str(), 0, 16);
	unsigned char g = (unsigned char)std::strtol(h.substr(2, 2).c_str(), 0, 16);
	unsigned char b = (unsigned char)std::strtol(h.substr(4, 2).c_str(), 0, 16);
	return Srgb(r, g, b);
}

Srgb Srgb::parse(const std::string& s)
{
	if (s.empty())
		return Srgb(0, 0, 0);
	if (s[0] == '#' || (s.length() == 6 && std::isxdigit(s[0])))
		return from_hex(s);
	if (s.substr(0, 4) == "rgb(")
	{
		std::size_t p1 = s.find(',');
		std::size_t p2 = (p1 != std::string::npos) ? s.find(',', p1 + 1) : std::string::npos;
		std::size_t pe = s.find(')');
		if (p1 != std::string::npos && p2 != std::string::npos && pe != std::string::npos)
		{
			unsigned char r = (unsigned char)std::atoi(s.substr(4, p1 - 4).c_str());
			unsigned char g = (unsigned char)std::atoi(s.substr(p1 + 1, p2 - p1 - 1).c_str());
			unsigned char b = (unsigned char)std::atoi(s.substr(p2 + 1, pe - p2 - 1).c_str());
			return Srgb(r, g, b);
		}
	}
	return named(s);
}

Srgb Srgb::named(const std::string& name)
{
	static std::map<std::string, Srgb> colors;
	if (colors.empty())
		_init_named_colors(colors);
	std::string lower = name;
	for (std::size_t i = 0; i < lower.size(); ++i)
		lower[i] = (char)std::tolower((unsigned char)lower[i]);
	std::map<std::string, Srgb>::const_iterator it = colors.find(lower);
	if (it != colors.end())
		return it->second;
	return Srgb(0, 0, 0);
}

Srgb Srgb::mix(const Srgb& a, const Srgb& b, double ratio)
{
	ratio = _clamp(ratio, 0.0, 1.0);
	unsigned char r = (unsigned char)(a.get_r() * (1.0 - ratio) + b.get_r() * ratio);
	unsigned char g = (unsigned char)(a.get_g() * (1.0 - ratio) + b.get_g() * ratio);
	unsigned char bl = (unsigned char)(a.get_b() * (1.0 - ratio) + b.get_b() * ratio);
	return Srgb(r, g, bl);
}

/* ── getters / setters ─────────────────────────────────────────────────── */

unsigned char Srgb::get_r() const { return (_color >> 16) & 0xFF; }
unsigned char Srgb::get_g() const { return (_color >> 8) & 0xFF; }
unsigned char Srgb::get_b() const { return _color & 0xFF; }
unsigned int Srgb::raw() const { return _color; }

void Srgb::set_r(unsigned char r) { _color = (_color & 0x00FFFF) | ((unsigned int)r << 16); }
void Srgb::set_g(unsigned char g) { _color = (_color & 0xFF00FF) | ((unsigned int)g << 8); }
void Srgb::set_b(unsigned char b) { _color = (_color & 0xFFFF00) | b; }

/* ── color math ────────────────────────────────────────────────────────── */

Srgb Srgb::lighten(double factor) const
{
	factor = _clamp(factor, 0.0, 1.0);
	return Srgb(
		(unsigned char)_clampi((int)(get_r() + (255 - get_r()) * factor), 0, 255),
		(unsigned char)_clampi((int)(get_g() + (255 - get_g()) * factor), 0, 255),
		(unsigned char)_clampi((int)(get_b() + (255 - get_b()) * factor), 0, 255));
}

Srgb Srgb::darken(double factor) const
{
	factor = _clamp(factor, 0.0, 1.0);
	return Srgb(
		(unsigned char)_clampi((int)(get_r() * (1.0 - factor)), 0, 255),
		(unsigned char)_clampi((int)(get_g() * (1.0 - factor)), 0, 255),
		(unsigned char)_clampi((int)(get_b() * (1.0 - factor)), 0, 255));
}

Srgb Srgb::saturate(double factor) const
{
	double gray = luminance();
	return Srgb(
		(unsigned char)_clampi((int)(gray + (get_r() - gray) * (1.0 + factor)), 0, 255),
		(unsigned char)_clampi((int)(gray + (get_g() - gray) * (1.0 + factor)), 0, 255),
		(unsigned char)_clampi((int)(gray + (get_b() - gray) * (1.0 + factor)), 0, 255));
}

Srgb Srgb::desaturate(double factor) const
{
	return saturate(-factor);
}

Srgb Srgb::complement() const
{
	return Srgb((unsigned char)(255 - get_r()),
				(unsigned char)(255 - get_g()),
				(unsigned char)(255 - get_b()));
}

Srgb Srgb::grayscale() const
{
	unsigned char g = static_cast<unsigned char>(luminance());
	return Srgb(g, g, g);
}

Srgb Srgb::invert() const { return complement(); }

bool Srgb::is_dark() const { return luminance() < 128.0; }
bool Srgb::is_light() const { return !is_dark(); }

double Srgb::contrast_ratio(const Srgb& other) const
{
	double l1 = luminance() / 255.0;
	double l2 = other.luminance() / 255.0;
	if (l1 < l2) { double t = l1; l1 = l2; l2 = t; }
	return (l1 + 0.05) / (l2 + 0.05);
}

double Srgb::distance(const Srgb& other) const
{
	double dr = static_cast<double>(get_r()) - static_cast<double>(other.get_r());
	double dg = static_cast<double>(get_g()) - static_cast<double>(other.get_g());
	double db = static_cast<double>(get_b()) - static_cast<double>(other.get_b());
	return std::sqrt(dr * dr + dg * dg + db * db);
}

Srgb Srgb::blend(const Srgb& other, double t) const
{
	if (t <= 0.0) return *this;
	if (t >= 1.0) return other;
	unsigned char r = static_cast<unsigned char>(get_r() + (other.get_r() - get_r()) * t);
	unsigned char g = static_cast<unsigned char>(get_g() + (other.get_g() - get_g()) * t);
	unsigned char b = static_cast<unsigned char>(get_b() + (other.get_b() - get_b()) * t);
	return Srgb(r, g, b);
}

double Srgb::luminance() const
{
	return 0.299 * get_r() + 0.587 * get_g() + 0.114 * get_b();
}

int Srgb::to_ansi_256() const
{
	unsigned char r = get_r(), g = get_g(), b = get_b();
	if (r == g && g == b)
	{
		if (r < 8) return 16;
		if (r > 248) return 231;
		return 232 + (r - 8) / 10;
	}
	int ri = (r * 5) / 255;
	int gi = (g * 5) / 255;
	int bi = (b * 5) / 255;
	return 16 + 36 * ri + 6 * gi + bi;
}

/* ── ANSI escape generation ────────────────────────────────────────────── */

std::string Srgb::to_ansi_fg() const
{
	std::stringstream ss;
	ss << "\033[38;2;" << (int)get_r() << ";" << (int)get_g() << ";" << (int)get_b() << "m";
	return ss.str();
}

std::string Srgb::to_ansi_bg() const
{
	std::stringstream ss;
	ss << "\033[48;2;" << (int)get_r() << ";" << (int)get_g() << ";" << (int)get_b() << "m";
	return ss.str();
}

std::string Srgb::to_hex() const
{
	std::stringstream ss;
	ss << "#" << std::hex << std::setfill('0') << std::setw(6) << _color;
	return ss.str();
}

/* ── bitwise operators ─────────────────────────────────────────────────── */

Srgb Srgb::operator|(const Srgb& o) const { return Srgb(_color | o._color); }
Srgb Srgb::operator&(const Srgb& o) const { return Srgb(_color & o._color); }
Srgb Srgb::operator^(const Srgb& o) const { return Srgb(_color ^ o._color); }
Srgb Srgb::operator~() const { return Srgb(~_color & 0xFFFFFF); }

/* ── comparison ────────────────────────────────────────────────────────── */

bool Srgb::operator==(const Srgb& o) const { return _color == o._color; }
bool Srgb::operator!=(const Srgb& o) const { return _color != o._color; }

/* ── backward compat aliases ───────────────────────────────────────────── */

std::string Srgb::toAnsi() const { return to_ansi_fg(); }
std::string Srgb::toBgAnsi() const { return to_ansi_bg(); }
std::string Srgb::toHex() const { return to_hex(); }
unsigned char Srgb::getR() const { return get_r(); }
unsigned char Srgb::getG() const { return get_g(); }
unsigned char Srgb::getB() const { return get_b(); }
void Srgb::setR(unsigned char r) { set_r(r); }
void Srgb::setG(unsigned char g) { set_g(g); }
void Srgb::setB(unsigned char b) { set_b(b); }

/* ── private helpers ───────────────────────────────────────────────────── */

double Srgb::_clamp(double v, double lo, double hi)
{
	if (v < lo) return lo;
	if (v > hi) return hi;
	return v;
}

int Srgb::_clampi(int v, int lo, int hi)
{
	if (v < lo) return lo;
	if (v > hi) return hi;
	return v;
}

void Srgb::_init_named_colors(std::map<std::string, Srgb>& m)
{
	m["black"]      = Srgb(0, 0, 0);
	m["white"]      = Srgb(255, 255, 255);
	m["red"]        = Srgb(255, 0, 0);
	m["green"]      = Srgb(0, 255, 0);
	m["blue"]       = Srgb(0, 0, 255);
	m["yellow"]     = Srgb(255, 255, 0);
	m["magenta"]    = Srgb(255, 0, 255);
	m["cyan"]       = Srgb(0, 255, 255);
	m["gray"]       = Srgb(128, 128, 128);
	m["darkgray"]   = Srgb(64, 64, 64);
	m["lightgray"]  = Srgb(192, 192, 192);
	m["orange"]     = Srgb(255, 165, 0);
	m["purple"]     = Srgb(128, 0, 128);
	m["pink"]       = Srgb(255, 192, 203);
	m["brown"]      = Srgb(165, 42, 42);
	m["lime"]       = Srgb(50, 205, 50);
	m["navy"]       = Srgb(0, 0, 128);
	m["teal"]       = Srgb(0, 128, 128);
	m["maroon"]     = Srgb(128, 0, 0);
	m["olive"]      = Srgb(128, 128, 0);
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Gradient
 * ═════════════════════════════════════════════════════════════════════════ */

Gradient::Gradient() {}

Gradient::Gradient(const Srgb& start, const Srgb& end)
{
	_stops.push_back(start);
	_stops.push_back(end);
}

Gradient::Gradient(const Gradient& other) : _stops(other._stops) {}

Gradient& Gradient::operator=(const Gradient& other)
{
	if (this != &other)
		_stops = other._stops;
	return *this;
}

Gradient::~Gradient() {}

Gradient& Gradient::add_stop(const Srgb& color)
{
	_stops.push_back(color);
	return *this;
}

Srgb Gradient::at(double t) const
{
	if (_stops.empty())
		return Srgb(0, 0, 0);
	if (_stops.size() == 1 || t <= 0.0)
		return _stops[0];
	if (t >= 1.0)
		return _stops[_stops.size() - 1];
	double scaled = t * (_stops.size() - 1);
	std::size_t idx = (std::size_t)scaled;
	double frac = scaled - idx;
	if (idx + 1 >= _stops.size())
		return _stops[_stops.size() - 1];
	return Srgb::mix(_stops[idx], _stops[idx + 1], frac);
}

std::vector<Srgb> Gradient::generate(int steps) const
{
	std::vector<Srgb> result;
	if (steps <= 0)
		return result;
	if (steps == 1)
	{
		result.push_back(at(0.5));
		return result;
	}
	for (int i = 0; i < steps; ++i)
		result.push_back(at((double)i / (steps - 1)));
	return result;
}

std::string Gradient::apply(const std::string& text) const
{
	if (text.empty() || _stops.empty())
		return text;
	/* collect UTF-8 codepoints */
	std::vector<std::string> cps;
	for (std::size_t i = 0; i < text.size(); )
	{
		unsigned char c = (unsigned char)text[i];
		int nb = 1;
		if      (c >= 0xF0) nb = 4;
		else if (c >= 0xE0) nb = 3;
		else if (c >= 0xC0) nb = 2;
		if (i + nb > text.size()) nb = 1;
		cps.push_back(text.substr(i, nb));
		i += nb;
	}
	std::string out;
	std::size_t len = cps.size();
	for (std::size_t i = 0; i < len; ++i)
	{
		double t = (len > 1) ? (double)i / (len - 1) : 0.5;
		Srgb c = at(t);
		out += c.to_ansi_fg();
		out += cps[i];
	}
	out += "\033[0m";
	return out;
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Palette
 * ═════════════════════════════════════════════════════════════════════════ */

Srgb Palette::black()      { return Srgb(0, 0, 0); }
Srgb Palette::white()      { return Srgb(255, 255, 255); }
Srgb Palette::red()        { return Srgb(255, 0, 0); }
Srgb Palette::green()      { return Srgb(0, 255, 0); }
Srgb Palette::blue()       { return Srgb(0, 0, 255); }
Srgb Palette::yellow()     { return Srgb(255, 255, 0); }
Srgb Palette::magenta()    { return Srgb(255, 0, 255); }
Srgb Palette::cyan()       { return Srgb(0, 255, 255); }
Srgb Palette::gray()       { return Srgb(128, 128, 128); }
Srgb Palette::dark_gray()  { return Srgb(64, 64, 64); }
Srgb Palette::light_gray() { return Srgb(192, 192, 192); }
Srgb Palette::orange()     { return Srgb(255, 165, 0); }
Srgb Palette::purple()     { return Srgb(128, 0, 128); }
Srgb Palette::pink()       { return Srgb(255, 192, 203); }
Srgb Palette::brown()      { return Srgb(165, 42, 42); }
Srgb Palette::lime()       { return Srgb(50, 205, 50); }
Srgb Palette::navy()       { return Srgb(0, 0, 128); }
Srgb Palette::teal()       { return Srgb(0, 128, 128); }
Srgb Palette::maroon()     { return Srgb(128, 0, 0); }
Srgb Palette::olive()      { return Srgb(128, 128, 0); }

} /* namespace libcpp */
