/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   style.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:33:17 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/term/style.hpp"

namespace libcpp
{

/* ══════════════════════════════════════════════════════════════════════════
 *  TermUtils
 * ═════════════════════════════════════════════════════════════════════════ */

int TermUtils::vis_width(const std::string& s)
{
	int w = 0;
	for (std::size_t i = 0; i < s.size(); ++i)
	{
		unsigned char c = (unsigned char)s[i];
		if (c < 0x80)
			++w;
		else if ((c & 0xC0) != 0x80)
			++w;
	}
	return w;
}

int TermUtils::vis_len(const std::string& s)
{
	int w = 0;
	bool in_esc = false;
	for (std::size_t i = 0; i < s.size(); ++i)
	{
		unsigned char c = (unsigned char)s[i];
		if (in_esc)
		{
			if (c == 'm')
				in_esc = false;
			continue;
		}
		if (c == 0x1B && i + 1 < s.size() && s[i + 1] == '[')
		{
			in_esc = true;
			++i;
			continue;
		}
		if (c < 0x80)
			++w;
		else if ((c & 0xC0) != 0x80)
			++w;
	}
	return w;
}

std::string TermUtils::fit_to(const std::string& s, int w)
{
	int vl = vis_len(s);
	if (vl >= w)
		return s;
	std::string result = s;
	for (int i = vl; i < w; ++i)
		result += ' ';
	return result;
}

std::string TermUtils::spaces(int n)
{
	if (n <= 0) return "";
	return std::string(n, ' ');
}

std::string TermUtils::newlines(int n)
{
	if (n <= 0) return "";
	return std::string(n, '\n');
}

std::string TermUtils::rep_ch(const std::string& ch, int n)
{
	std::string out;
	for (int i = 0; i < n; ++i)
		out += ch;
	return out;
}

std::string TermUtils::tile_sep(const std::string& glyph, int w)
{
	int gw = vis_width(glyph);
	if (gw <= 0) gw = 1;
	std::string out;
	int cur = 0;
	while (cur < w)
	{
		out += glyph;
		cur += gw;
	}
	return out;
}

std::string TermUtils::reset()
{
	return "\033[0m";
}

std::string TermUtils::apply_fg(const Srgb& c)
{
	return c.to_ansi_fg();
}

std::string TermUtils::apply_bg(const Srgb& c)
{
	return c.to_ansi_bg();
}

std::string TermUtils::apply_font(int flags)
{
	std::string out;
	if (flags & FONT_BOLD)      out += "\033[1m";
	if (flags & FONT_DIM)       out += "\033[2m";
	if (flags & FONT_ITALIC)    out += "\033[3m";
	if (flags & FONT_UNDERLINE) out += "\033[4m";
	if (flags & FONT_BLINK)     out += "\033[5m";
	if (flags & FONT_REVERSE)   out += "\033[7m";
	return out;
}

std::string TermUtils::strip_ansi(const std::string& s)
{
	std::string out;
	bool in_esc = false;
	for (std::size_t i = 0; i < s.size(); ++i)
	{
		if (in_esc)
		{
			if (s[i] == 'm')
				in_esc = false;
			continue;
		}
		if ((unsigned char)s[i] == 0x1B && i + 1 < s.size() && s[i + 1] == '[')
		{
			in_esc = true;
			++i;
			continue;
		}
		out += s[i];
	}
	return out;
}

int TermUtils::visible_width(const std::string& s)
{
	return static_cast<int>(strip_ansi(s).size());
}

std::string TermUtils::word_wrap(const std::string& text, int width)
{
	std::string result;
	int col = 0;
	std::string word;
	for (std::size_t i = 0; i <= text.size(); ++i)
	{
		char c = (i < text.size()) ? text[i] : ' ';
		if (c == ' ' || c == '\n' || i == text.size())
		{
			if (!word.empty())
			{
				int wlen = static_cast<int>(word.size());
				if (col > 0 && col + 1 + wlen > width)
				{
					result += '\n';
					col = 0;
				}
				if (col > 0) { result += ' '; ++col; }
				result += word;
				col += wlen;
				word.clear();
			}
			if (c == '\n') { result += '\n'; col = 0; }
		}
		else
		{
			word += c;
		}
	}
	return result;
}

/* ══════════════════════════════════════════════════════════════════════════
 *  ElemStyle
 * ═════════════════════════════════════════════════════════════════════════ */

ElemStyle::ElemStyle()
	: fg(255, 255, 255), bg(0, 0, 0), border(128, 128, 128), font(0),
	  align(ALIGN_LEFT), width(60), pad_l(1), pad_r(1), pad_v(0),
	  margin_l(0), space_before(0), space_after(0), has_bg(false),
	  glyph(""), body_glyph(""), sep_str(Glyph::HDASH) {}

ElemStyle::ElemStyle(const ElemStyle& o)
	: fg(o.fg), bg(o.bg), border(o.border), font(o.font),
	  align(o.align), width(o.width), pad_l(o.pad_l), pad_r(o.pad_r),
	  pad_v(o.pad_v), margin_l(o.margin_l), space_before(o.space_before),
	  space_after(o.space_after), has_bg(o.has_bg), glyph(o.glyph),
	  body_glyph(o.body_glyph), sep_str(o.sep_str) {}

ElemStyle& ElemStyle::operator=(const ElemStyle& o)
{
	if (this != &o)
	{
		fg = o.fg; bg = o.bg; border = o.border; font = o.font;
		align = o.align; width = o.width; pad_l = o.pad_l; pad_r = o.pad_r;
		pad_v = o.pad_v; margin_l = o.margin_l; space_before = o.space_before;
		space_after = o.space_after; has_bg = o.has_bg; glyph = o.glyph;
		body_glyph = o.body_glyph; sep_str = o.sep_str;
	}
	return *this;
}

ElemStyle::~ElemStyle() {}

/* ══════════════════════════════════════════════════════════════════════════
 *  TermStyle
 * ═════════════════════════════════════════════════════════════════════════ */

TermStyle::TermStyle()
{
	h1_style.font = FONT_BOLD;
	h1_style.fg = Srgb(0x00BFFF);
	h1_style.space_before = 1;
	h1_style.space_after = 1;

	h2_style.font = FONT_BOLD;
	h2_style.fg = Srgb(0xFFD700);
	h2_style.space_before = 1;

	h3_style.font = FONT_BOLD | FONT_ITALIC;
	h3_style.fg = Srgb(0x87CEEB);

	body_style.fg = Srgb(0xCCCCCC);

	quote_style.fg = Srgb(0x888888);
	quote_style.glyph = "\xe2\x94\x83 ";
	quote_style.font = FONT_ITALIC;

	info_style.fg = Srgb(0x00FF00);
	info_style.glyph = "\xe2\x84\xb9 ";

	warn_style.fg = Srgb(0xFFFF00);
	warn_style.glyph = "\xe2\x9a\xa0 ";

	error_style.fg = Srgb(0xFF0000);
	error_style.glyph = "\xe2\x9c\x97 ";

	sep_style.fg = Srgb(0x555555);
}

TermStyle::TermStyle(const TermStyle& o)
	: h1_style(o.h1_style), h2_style(o.h2_style), h3_style(o.h3_style),
	  body_style(o.body_style), quote_style(o.quote_style),
	  info_style(o.info_style), warn_style(o.warn_style),
	  error_style(o.error_style), sep_style(o.sep_style),
	  section_style(o.section_style) {}

TermStyle& TermStyle::operator=(const TermStyle& o)
{
	if (this != &o)
	{
		h1_style = o.h1_style; h2_style = o.h2_style; h3_style = o.h3_style;
		body_style = o.body_style; quote_style = o.quote_style;
		info_style = o.info_style; warn_style = o.warn_style;
		error_style = o.error_style; sep_style = o.sep_style;
		section_style = o.section_style;
	}
	return *this;
}

TermStyle::~TermStyle() {}

/* ── rendering helpers ─────────────────────────────────────────────────── */

std::string TermStyle::_render_elem(const ElemStyle& es, const std::string& text) const
{
	std::ostringstream out;
	out << TermUtils::newlines(es.space_before);
	out << TermUtils::spaces(es.margin_l);
	if (es.has_bg)
		out << TermUtils::apply_bg(es.bg);
	out << TermUtils::apply_fg(es.fg);
	out << TermUtils::apply_font(es.font);
	if (!es.glyph.empty())
		out << es.glyph;
	out << TermUtils::spaces(es.pad_l);
	out << text;
	out << TermUtils::spaces(es.pad_r);
	out << TermUtils::reset();
	out << TermUtils::newlines(es.space_after);
	return out.str();
}

std::string TermStyle::_apply_inline(int font, const std::string& text) const
{
	return TermUtils::apply_font(font) + text + TermUtils::reset();
}

std::string TermStyle::h1(const std::string& text) const
{
	std::ostringstream out;
	out << _render_elem(h1_style, text) << "\n";
	out << TermUtils::apply_fg(h1_style.fg);
	out << TermUtils::tile_sep(h1_style.sep_str, h1_style.width);
	out << TermUtils::reset() << "\n";
	return out.str();
}

std::string TermStyle::h2(const std::string& text) const
{
	return _render_elem(h2_style, text) + "\n";
}

std::string TermStyle::h3(const std::string& text) const
{
	return _render_elem(h3_style, text) + "\n";
}

std::string TermStyle::bold(const std::string& t) const
{ return _apply_inline(FONT_BOLD, t); }

std::string TermStyle::italic(const std::string& t) const
{ return _apply_inline(FONT_ITALIC, t); }

std::string TermStyle::dim(const std::string& t) const
{ return _apply_inline(FONT_DIM, t); }

std::string TermStyle::underline(const std::string& t) const
{ return _apply_inline(FONT_UNDERLINE, t); }

std::string TermStyle::quote(const std::string& text) const
{
	return _render_elem(quote_style, text) + "\n";
}

std::string TermStyle::separator() const
{
	std::string out;
	out += TermUtils::apply_fg(sep_style.fg);
	out += TermUtils::tile_sep(sep_style.sep_str, sep_style.width);
	out += TermUtils::reset();
	out += "\n";
	return out;
}

std::string TermStyle::hr() const { return separator(); }

std::string TermStyle::info(const std::string& text) const
{ return _render_elem(info_style, text) + "\n"; }

std::string TermStyle::warn(const std::string& text) const
{ return _render_elem(warn_style, text) + "\n"; }

std::string TermStyle::error(const std::string& text) const
{ return _render_elem(error_style, text) + "\n"; }

std::string TermStyle::success(const std::string& text) const
{
	ElemStyle es = info_style;
	es.glyph = "\xe2\x9c\x93 ";
	return _render_elem(es, text) + "\n";
}

std::string TermStyle::section(const std::string& title, const std::string& body) const
{
	std::ostringstream out;
	out << _render_elem(section_style, title) << "\n";
	out << TermUtils::apply_fg(body_style.fg);
	out << body;
	out << TermUtils::reset() << "\n";
	return out.str();
}

/* ══════════════════════════════════════════════════════════════════════════
 *  TermConf
 * ═════════════════════════════════════════════════════════════════════════ */

TermConf::TermConf()
	: global_width(60), h1_space_before(1), h1_space_after(1),
	  h2_space_before(1), h2_space_after(0), quote_margin(2) {}

TermConf::TermConf(const TermConf& o)
	: global_width(o.global_width), h1_space_before(o.h1_space_before),
	  h1_space_after(o.h1_space_after), h2_space_before(o.h2_space_before),
	  h2_space_after(o.h2_space_after), quote_margin(o.quote_margin) {}

TermConf& TermConf::operator=(const TermConf& o)
{
	if (this != &o)
	{
		global_width = o.global_width;
		h1_space_before = o.h1_space_before; h1_space_after = o.h1_space_after;
		h2_space_before = o.h2_space_before; h2_space_after = o.h2_space_after;
		quote_margin = o.quote_margin;
	}
	return *this;
}

TermConf::~TermConf() {}

void TermConf::apply(TermStyle& ts) const
{
	ts.h1_style.width = global_width;
	ts.h1_style.space_before = h1_space_before;
	ts.h1_style.space_after = h1_space_after;
	ts.h2_style.width = global_width;
	ts.h2_style.space_before = h2_space_before;
	ts.h2_style.space_after = h2_space_after;
	ts.sep_style.width = global_width;
	ts.quote_style.margin_l = quote_margin;
}

void TermConf::apply_default(TermStyle& ts)
{
	TermConf c;
	c.apply(ts);
}

void TermConf::apply_minimal(TermStyle& ts)
{
	TermConf c;
	c.global_width = 40;
	c.h1_space_before = 0;
	c.h1_space_after = 0;
	c.h2_space_before = 0;
	c.quote_margin = 0;
	c.apply(ts);
}

void TermConf::apply_compact(TermStyle& ts)
{
	TermConf c;
	c.global_width = 50;
	c.h1_space_before = 0;
	c.h1_space_after = 0;
	c.apply(ts);
}

} /* namespace libcpp */
