/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   style.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:33:16 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_TERM_STYLE_HPP
# define LIBCPP_TERM_STYLE_HPP

# include <string>
# include <sstream>
# include "libcpp/term/color.hpp"

namespace libcpp
{

/* ══════════════════════════════════════════════════════════════════════════
 *  TermUtils — static terminal / UTF-8 helpers
 * ═════════════════════════════════════════════════════════════════════════ */
struct TermUtils
{
	static int vis_width(const std::string& s);
	static int vis_len(const std::string& s);
	static std::string fit_to(const std::string& s, int w);
	static std::string spaces(int n);
	static std::string newlines(int n);
	static std::string rep_ch(const std::string& ch, int n);
	static std::string tile_sep(const std::string& glyph, int w);
	static std::string reset();
	static std::string apply_fg(const Srgb& c);
	static std::string apply_bg(const Srgb& c);
	static std::string apply_font(int flags);
	static std::string strip_ansi(const std::string& s);
	static int visible_width(const std::string& s);
	static std::string word_wrap(const std::string& text, int width);
};

/* ══════════════════════════════════════════════════════════════════════════
 *  Font flags
 * ═════════════════════════════════════════════════════════════════════════ */
enum FontFlag
{
	FONT_NONE      = 0,
	FONT_BOLD      = 1 << 0,
	FONT_DIM       = 1 << 1,
	FONT_ITALIC    = 1 << 2,
	FONT_UNDERLINE = 1 << 3,
	FONT_BLINK     = 1 << 4,
	FONT_REVERSE   = 1 << 5
};

/* ══════════════════════════════════════════════════════════════════════════
 *  Alignment
 * ═════════════════════════════════════════════════════════════════════════ */
enum Align
{
	ALIGN_LEFT   = 0,
	ALIGN_CENTER = 1,
	ALIGN_RIGHT  = 2
};

/* ══════════════════════════════════════════════════════════════════════════
 *  Unicode glyph constants
 * ═════════════════════════════════════════════════════════════════════════ */
namespace Glyph
{
	static const char HDASH[]   = "\xe2\x94\x80";
	static const char VDASH[]   = "\xe2\x94\x82";
	static const char BULLET[]  = "\xe2\x97\x8f";
	static const char ARROW[]   = "\xe2\x96\xb6";
	static const char CHECK[]   = "\xe2\x9c\x93";
	static const char CROSS[]   = "\xe2\x9c\x97";
	static const char DOT[]     = "\xc2\xb7";
	static const char STAR[]    = "\xe2\x98\x85";
	static const char DIAMOND[] = "\xe2\x97\x86";
	static const char CIRCLE[]  = "\xe2\x97\x8b";
}

/* ══════════════════════════════════════════════════════════════════════════
 *  ElemStyle — per-element visual configuration
 * ═════════════════════════════════════════════════════════════════════════ */
struct ElemStyle
{
	Srgb        fg;
	Srgb        bg;
	Srgb        border;
	int         font;
	Align       align;
	int         width;
	int         pad_l;
	int         pad_r;
	int         pad_v;
	int         margin_l;
	int         space_before;
	int         space_after;
	bool        has_bg;
	std::string glyph;
	std::string body_glyph;
	std::string sep_str;

	ElemStyle();
	ElemStyle(const ElemStyle& other);
	ElemStyle& operator=(const ElemStyle& other);
	~ElemStyle();
};

/* ══════════════════════════════════════════════════════════════════════════
 *  TermStyle — markdown-like terminal renderer
 *  Orthodox Canonical Form compliant.
 * ═════════════════════════════════════════════════════════════════════════ */
class TermStyle
{
public:
	/* ── OCF ───────────────────────────────────────────────────── */
	TermStyle();
	TermStyle(const TermStyle& other);
	TermStyle& operator=(const TermStyle& other);
	~TermStyle();

	/* ── element styles (public for direct access) ─────────────── */
	ElemStyle h1_style;
	ElemStyle h2_style;
	ElemStyle h3_style;
	ElemStyle body_style;
	ElemStyle quote_style;
	ElemStyle info_style;
	ElemStyle warn_style;
	ElemStyle error_style;
	ElemStyle sep_style;
	ElemStyle section_style;

	/* ── rendering ─────────────────────────────────────────────── */
	std::string h1(const std::string& text) const;
	std::string h2(const std::string& text) const;
	std::string h3(const std::string& text) const;
	std::string bold(const std::string& text) const;
	std::string italic(const std::string& text) const;
	std::string dim(const std::string& text) const;
	std::string underline(const std::string& text) const;
	std::string quote(const std::string& text) const;
	std::string separator() const;
	std::string hr() const;
	std::string info(const std::string& text) const;
	std::string warn(const std::string& text) const;
	std::string error(const std::string& text) const;
	std::string success(const std::string& text) const;
	std::string section(const std::string& title, const std::string& body) const;

private:
	std::string _render_elem(const ElemStyle& es, const std::string& text) const;
	std::string _apply_inline(int font, const std::string& text) const;
};

/* ══════════════════════════════════════════════════════════════════════════
 *  TermConf — configuration presets for TermStyle
 *  Orthodox Canonical Form compliant.
 * ═════════════════════════════════════════════════════════════════════════ */
class TermConf
{
public:
	int global_width;
	int h1_space_before;
	int h1_space_after;
	int h2_space_before;
	int h2_space_after;
	int quote_margin;

	/* ── OCF ───────────────────────────────────────────────────── */
	TermConf();
	TermConf(const TermConf& other);
	TermConf& operator=(const TermConf& other);
	~TermConf();

	void apply(TermStyle& ts) const;

	static void apply_default(TermStyle& ts);
	static void apply_minimal(TermStyle& ts);
	static void apply_compact(TermStyle& ts);
};

} /* namespace libcpp */

#endif /* LIBCPP_TERM_STYLE_HPP */
