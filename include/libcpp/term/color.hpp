/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:38:27 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_TERM_COLOR_HPP
# define LIBCPP_TERM_COLOR_HPP

# include <string>
# include <sstream>
# include <iomanip>
# include <map>
# include <vector>
# include <cmath>
# include <cstdlib>
# include <algorithm>

namespace libcpp
{

/*
** Srgb — unified 24-bit RGB color (absorbs old Srgb + Colors::RGB)
**
** Orthodox Canonical Form compliant.
*/
class Srgb
{
public:
	/* ── Orthodox Canonical Form ───────────────────────────────── */
	Srgb();
	Srgb(unsigned int rgb);
	Srgb(unsigned char r, unsigned char g, unsigned char b);
	Srgb(const Srgb& other);
	Srgb& operator=(const Srgb& other);
	~Srgb();

	/* ── static factories ──────────────────────────────────────── */
	static Srgb from_hex(const std::string& hex);
	static Srgb parse(const std::string& s);
	static Srgb named(const std::string& name);
	static Srgb mix(const Srgb& a, const Srgb& b, double ratio);

	/* ── getters / setters ─────────────────────────────────────── */
	unsigned char get_r() const;
	unsigned char get_g() const;
	unsigned char get_b() const;
	void set_r(unsigned char r);
	void set_g(unsigned char g);
	void set_b(unsigned char b);
	unsigned int raw() const;

	/* ── color math ────────────────────────────────────────────── */
	Srgb lighten(double factor) const;
	Srgb darken(double factor) const;
	Srgb saturate(double factor) const;
	Srgb desaturate(double factor) const;
	Srgb complement() const;
	Srgb grayscale() const;
	Srgb invert() const;
	bool is_dark() const;
	bool is_light() const;
	double contrast_ratio(const Srgb& other) const;
	double distance(const Srgb& other) const;
	Srgb   blend(const Srgb& other, double t) const;
	double luminance() const;
	int to_ansi_256() const;

	/* ── ANSI escape generation ────────────────────────────────── */
	std::string to_ansi_fg() const;
	std::string to_ansi_bg() const;
	std::string to_hex() const;

	/* ── bitwise operators ─────────────────────────────────────── */
	Srgb operator|(const Srgb& other) const;
	Srgb operator&(const Srgb& other) const;
	Srgb operator^(const Srgb& other) const;
	Srgb operator~() const;

	/* ── comparison ────────────────────────────────────────────── */
	bool operator==(const Srgb& other) const;
	bool operator!=(const Srgb& other) const;

	/* ── backward compat aliases (will be removed later) ───────── */
	std::string toAnsi() const;
	std::string toBgAnsi() const;
	std::string toHex() const;
	unsigned char getR() const;
	unsigned char getG() const;
	unsigned char getB() const;
	void setR(unsigned char r);
	void setG(unsigned char g);
	void setB(unsigned char b);

private:
	unsigned int _color;

	static double _clamp(double v, double lo, double hi);
	static int _clampi(int v, int lo, int hi);
	static void _init_named_colors(std::map<std::string, Srgb>& m);
};

/*
** Gradient — generates multi-stop color gradients
** Orthodox Canonical Form compliant.
*/
class Gradient
{
public:
	Gradient();
	Gradient(const Srgb& start, const Srgb& end);
	Gradient(const Gradient& other);
	Gradient& operator=(const Gradient& other);
	~Gradient();

	Gradient& add_stop(const Srgb& color);
	Srgb at(double t) const;
	std::vector<Srgb> generate(int steps) const;
	std::string apply(const std::string& text) const;

private:
	std::vector<Srgb> _stops;
};

/*
** Palette — named color constants
*/
struct Palette
{
	static Srgb black();
	static Srgb white();
	static Srgb red();
	static Srgb green();
	static Srgb blue();
	static Srgb yellow();
	static Srgb magenta();
	static Srgb cyan();
	static Srgb gray();
	static Srgb dark_gray();
	static Srgb light_gray();
	static Srgb orange();
	static Srgb purple();
	static Srgb pink();
	static Srgb brown();
	static Srgb lime();
	static Srgb navy();
	static Srgb teal();
	static Srgb maroon();
	static Srgb olive();
};

} /* namespace libcpp */

#endif /* LIBCPP_TERM_COLOR_HPP */
