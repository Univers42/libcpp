/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   style.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/22 10:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/term/style.hpp"

namespace libcpp
{

/* ======================================================================
 *  TermUtils
 * ====================================================================== */

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
if (c == 'm') in_esc = false;
continue;
}
if (c == 0x1B && i + 1 < s.size() && s[i + 1] == '[')
{
in_esc = true;
++i;
continue;
}
if (c < 0x80) ++w;
else if ((c & 0xC0) != 0x80) ++w;
}
return w;
}

std::string TermUtils::fit_to(const std::string& s, int w)
{
int vl = vis_len(s);
if (vl >= w) return s;
return s + std::string(w - vl, ' ');
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
for (int i = 0; i < n; ++i) out += ch;
return out;
}

std::string TermUtils::tile_sep(const std::string& glyph, int w)
{
int gw = vis_width(glyph);
if (gw <= 0) gw = 1;
std::string out;
int cur = 0;
while (cur < w) { out += glyph; cur += gw; }
return out;
}

std::string TermUtils::reset() { return "\033[0m"; }

std::string TermUtils::apply_fg(const Srgb& c) { return c.to_ansi_fg(); }
std::string TermUtils::apply_bg(const Srgb& c) { return c.to_ansi_bg(); }

std::string TermUtils::apply_font(int flags)
{
std::string out;
if (flags & FONT_BOLD)      out += "\033[1m";
if (flags & FONT_DIM)       out += "\033[2m";
if (flags & FONT_ITALIC)    out += "\033[3m";
if (flags & FONT_UNDERLINE) out += "\033[4m";
if (flags & FONT_BLINK)     out += "\033[5m";
if (flags & FONT_REVERSE)   out += "\033[7m";
if (flags & FONT_STRIKE)    out += "\033[9m";
return out;
}

std::string TermUtils::strip_ansi(const std::string& s)
{
std::string out;
bool in_esc = false;
for (std::size_t i = 0; i < s.size(); ++i)
{
if (in_esc) { if (s[i] == 'm') in_esc = false; continue; }
if ((unsigned char)s[i] == 0x1B && i + 1 < s.size() && s[i + 1] == '[')
{
in_esc = true; ++i; continue;
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
if (col > 0 && col + 1 + wlen > width) { result += '\n'; col = 0; }
if (col > 0) { result += ' '; ++col; }
result += word; col += wlen; word.clear();
}
if (c == '\n') { result += '\n'; col = 0; }
}
else word += c;
}
return result;
}

/* ======================================================================
 *  ElemStyle
 * ====================================================================== */

ElemStyle::ElemStyle()
: fg(255, 255, 255), bg(0, 0, 0), border(128, 128, 128), font(0),
  align(ALIGN_LEFT), width(60), pad_l(1), pad_r(1), pad_v(0),
  margin_l(0), space_before(0), space_after(0), has_bg(false),
  glyph(""), body_glyph(""), sep_str(Glyph::HDASH) {}

ElemStyle::ElemStyle(
const Srgb& fg_, const Srgb& bg_, const Srgb& brd_, int font_,
Align a, int w, int pl, int pr, int pv, int ml,
int sb, int sa, bool use_bg,
const std::string& gl, const std::string& sep)
: fg(fg_), bg(bg_), border(brd_), font(font_), align(a),
  width(w), pad_l(pl), pad_r(pr), pad_v(pv), margin_l(ml),
  space_before(sb), space_after(sa), has_bg(use_bg),
  glyph(gl), body_glyph(""), sep_str(sep) {}

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

/* ======================================================================
 *  TermStyle -- constructor (rich defaults matching libcpp)
 * ====================================================================== */

TermStyle::TermStyle()
:
/*         fg                    bg                   border               font                  align        w  pL pR pV mL sB sA  bg?   glyph             sep */
h1_style  (Srgb(255,220,240), Srgb( 30, 15, 40), Srgb(200,100,180), FONT_BOLD,                ALIGN_CENTER, 60, 3, 3, 1, 0, 1, 1, true,  Glyph::DIAMOND,   Glyph::DOUBLE),
h2_style  (Srgb(110,190,255), Srgb(  0,  0,  0), Srgb( 60,120,200), FONT_BOLD,                ALIGN_LEFT,   60, 1, 1, 0, 0, 1, 0, false, Glyph::TRIANGLE,  Glyph::HDASH_HVY),
h3_style  (Srgb(120,220,160), Srgb(  0,  0,  0), Srgb( 50,140, 80), FONT_BOLD,                ALIGN_LEFT,   60, 1, 1, 0, 0, 1, 0, false, Glyph::BULLET,    Glyph::DOTTED),
body_style(Srgb(200,200,210), Srgb(  0,  0,  0), Srgb(128,128,128), FONT_NONE,                ALIGN_LEFT,   60, 2, 1, 0, 0, 0, 0, false, "",               Glyph::HDASH),
quote_style(Srgb(160,170,180),Srgb(  0,  0,  0), Srgb( 90, 90,100), FONT_ITALIC,              ALIGN_LEFT,   60, 1, 1, 0, 3, 0, 0, false, Glyph::VBAR,      Glyph::HDASH),
info_style(Srgb( 80,200,255), Srgb(  0,  0,  0), Srgb( 60,120,160), FONT_NONE,                ALIGN_LEFT,   60, 1, 1, 0, 0, 0, 0, false, Glyph::INFO,      Glyph::HDASH),
warn_style(Srgb(255,200, 50), Srgb(  0,  0,  0), Srgb(180,140,  0), FONT_BOLD,                ALIGN_LEFT,   60, 1, 1, 0, 0, 0, 0, false, Glyph::WARN,      Glyph::HDASH),
error_style(Srgb(255, 80, 80),Srgb(  0,  0,  0), Srgb(200, 40, 40), FONT_BOLD,                ALIGN_LEFT,   60, 1, 1, 0, 0, 0, 0, false, Glyph::CROSS,     Glyph::HDASH),
success_style(Srgb(80,230,130),Srgb( 0,  0,  0), Srgb(  0,160, 80), FONT_NONE,                ALIGN_LEFT,   60, 1, 1, 0, 0, 0, 0, false, Glyph::CHECK,     Glyph::HDASH),
danger_style(Srgb(255,100,100),Srgb( 0,  0,  0), Srgb(200, 40, 40), FONT_BOLD,                ALIGN_LEFT,   60, 1, 1, 0, 0, 0, 0, false, Glyph::SKULL,     Glyph::HDASH),
trace_style(Srgb(170,140,220),Srgb(  0,  0,  0), Srgb(120, 80,180), FONT_DIM,                 ALIGN_LEFT,   60, 1, 1, 0, 0, 0, 0, false, Glyph::CHAIN,     Glyph::HDASH),
sep_style (Srgb( 80, 80, 90), Srgb(  0,  0,  0), Srgb( 80, 80, 90), FONT_DIM,                 ALIGN_LEFT,   60, 0, 0, 0, 0, 0, 0, false, "",               Glyph::HDASH),
section_style(Srgb(130,200,255),Srgb(0, 0,  0),  Srgb( 70,130,200), FONT_BOLD,                ALIGN_LEFT,   60, 1, 1, 0, 0, 0, 0, false, Glyph::TRIANGLE,  Glyph::HDASH_HVY),
callout_style(Srgb(190,200,215),Srgb(20,25, 38), Srgb( 80,120,180), FONT_NONE,                ALIGN_LEFT,   60, 1, 1, 0, 1, 0, 0, true,  Glyph::INFO,      Glyph::HDASH),
bullet_style(Srgb(190,200,210),Srgb( 0,  0,  0), Srgb(128,128,128), FONT_NONE,                ALIGN_LEFT,   60, 3, 1, 0, 0, 0, 0, false, Glyph::BULLET,    Glyph::HDASH),
ol_style  (Srgb(190,200,210), Srgb(  0,  0,  0), Srgb(128,128,128), FONT_NONE,                ALIGN_LEFT,   60, 3, 1, 0, 0, 0, 0, false, "",               Glyph::HDASH),
bold_style(Srgb(255,255,255), Srgb(  0,  0,  0), Srgb(128,128,128), FONT_BOLD,                ALIGN_LEFT,   60, 2, 1, 0, 0, 0, 0, false, "",               Glyph::HDASH),
italic_style(Srgb(200,200,220),Srgb( 0,  0,  0), Srgb(128,128,128), FONT_ITALIC,              ALIGN_LEFT,   60, 2, 1, 0, 0, 0, 0, false, "",               Glyph::HDASH),
underline_style(Srgb(200,200,220),Srgb(0,0,  0), Srgb(128,128,128), FONT_UNDERLINE,           ALIGN_LEFT,   60, 2, 1, 0, 0, 0, 0, false, "",               Glyph::HDASH),
strike_style(Srgb(150,150,160),Srgb( 0,  0,  0), Srgb(128,128,128), FONT_STRIKE,              ALIGN_LEFT,   60, 2, 1, 0, 0, 0, 0, false, "",               Glyph::HDASH),
dim_style (Srgb(140,140,150), Srgb(  0,  0,  0), Srgb(128,128,128), FONT_DIM,                 ALIGN_LEFT,   60, 2, 1, 0, 0, 0, 0, false, "",               Glyph::HDASH),
code_style(Srgb(230,180,100), Srgb( 30, 30, 40), Srgb(128,128,128), FONT_NONE,                ALIGN_LEFT,   60, 2, 1, 0, 0, 0, 0, true,  "",               Glyph::HDASH)
{}

TermStyle::TermStyle(const TermStyle& o)
: h1_style(o.h1_style), h2_style(o.h2_style), h3_style(o.h3_style),
  body_style(o.body_style), quote_style(o.quote_style),
  info_style(o.info_style), warn_style(o.warn_style),
  error_style(o.error_style), success_style(o.success_style),
  danger_style(o.danger_style), trace_style(o.trace_style),
  sep_style(o.sep_style), section_style(o.section_style),
  callout_style(o.callout_style), bullet_style(o.bullet_style),
  ol_style(o.ol_style), bold_style(o.bold_style),
  italic_style(o.italic_style), underline_style(o.underline_style),
  strike_style(o.strike_style), dim_style(o.dim_style),
  code_style(o.code_style) {}

TermStyle& TermStyle::operator=(const TermStyle& o)
{
if (this != &o)
{
h1_style = o.h1_style; h2_style = o.h2_style; h3_style = o.h3_style;
body_style = o.body_style; quote_style = o.quote_style;
info_style = o.info_style; warn_style = o.warn_style;
error_style = o.error_style; success_style = o.success_style;
danger_style = o.danger_style; trace_style = o.trace_style;
sep_style = o.sep_style; section_style = o.section_style;
callout_style = o.callout_style; bullet_style = o.bullet_style;
ol_style = o.ol_style; bold_style = o.bold_style;
italic_style = o.italic_style; underline_style = o.underline_style;
strike_style = o.strike_style; dim_style = o.dim_style;
code_style = o.code_style;
}
return *this;
}

TermStyle::~TermStyle() {}

/* ======================================================================
 *  Rendering helpers
 * ====================================================================== */

std::string TermStyle::_align_text(const std::string& txt, int available, Align a) const
{
int tl = TermUtils::vis_len(txt);
if (tl >= available) return txt;
int gap = available - tl;
if (a == ALIGN_CENTER)
{
int l = gap / 2;
int r = gap - l;
return TermUtils::spaces(l) + txt + TermUtils::spaces(r);
}
if (a == ALIGN_RIGHT) return TermUtils::spaces(gap) + txt;
return txt + TermUtils::spaces(gap);
}

std::string TermStyle::_render_line(const ElemStyle& es, const std::string& content) const
{
std::string r;
int w = es.width > 0 ? es.width : 60;
int inner_w = w - es.pad_l - es.pad_r;
if (inner_w < 1) inner_w = 1;

r += TermUtils::newlines(es.space_before);
r += TermUtils::spaces(es.margin_l);
if (es.has_bg) r += TermUtils::apply_bg(es.bg);
r += TermUtils::apply_fg(es.fg);
r += TermUtils::apply_font(es.font);
r += TermUtils::spaces(es.pad_l);
r += es.glyph;
r += _align_text(content, inner_w - TermUtils::vis_len(es.glyph), es.align);
r += TermUtils::spaces(es.pad_r);
r += TermUtils::reset();
r += TermUtils::newlines(es.space_after);
return r;
}

std::string TermStyle::_render_bg_line(const ElemStyle& es, const std::string& content) const
{
std::string r;
int w = es.width > 0 ? es.width : 60;
int inner_w = w - es.pad_l - es.pad_r;
if (inner_w < 1) inner_w = 1;

r += TermUtils::spaces(es.margin_l);
if (es.has_bg) r += TermUtils::apply_bg(es.bg);
r += TermUtils::apply_fg(es.fg);
r += TermUtils::apply_font(es.font);
r += TermUtils::spaces(es.pad_l);
r += _align_text(content, inner_w, es.align);
r += TermUtils::spaces(es.pad_r);
r += TermUtils::reset();
return r;
}

std::string TermStyle::_render_bg_blank(const ElemStyle& es) const
{
std::string r;
int w = es.width > 0 ? es.width : 60;
r += TermUtils::spaces(es.margin_l);
if (es.has_bg) r += TermUtils::apply_bg(es.bg);
r += TermUtils::spaces(w);
r += TermUtils::reset();
return r;
}

std::string TermStyle::_render_sep(const ElemStyle& es) const
{
int w = es.width > 0 ? es.width : 60;
std::string r;
r += TermUtils::spaces(es.margin_l);
r += TermUtils::apply_fg(es.border);
r += TermUtils::apply_font(es.font);
r += TermUtils::tile_sep(es.sep_str, w);
r += TermUtils::reset();
return r;
}

std::string TermStyle::_render_heading(const ElemStyle& es, const std::string& title) const
{
std::string r;
std::string rst = TermUtils::reset();
int w = es.width > 0 ? es.width : 60;

r += TermUtils::newlines(es.space_before);

if (es.has_bg)
{
	/* ── Banner-style heading (H1) ─────────────────────────────
	**
	**  ╔══════════════════════════════════════════╗
	**  ║                                          ║
	**  ║        ◆  Title Text Here                ║
	**  ║                                          ║
	**  ╚══════════════════════════════════════════╝
	*/
	std::string border_fg = TermUtils::apply_fg(es.border);
	std::string text_fg   = TermUtils::apply_fg(es.fg);
	std::string bg_on     = TermUtils::apply_bg(es.bg);
	std::string font_on   = TermUtils::apply_font(es.font);

	/* corner + horizontal bar chars */
	std::string tl = "\xe2\x95\x94";  /* ╔ */
	std::string tr = "\xe2\x95\x97";  /* ╗ */
	std::string bl = "\xe2\x95\x9a";  /* ╚ */
	std::string br = "\xe2\x95\x9d";  /* ╝ */
	std::string hz = "\xe2\x95\x90";  /* ═ */
	std::string vl = "\xe2\x95\x91";  /* ║ */

	std::string hz_line = TermUtils::rep_ch(hz, w - 2);

	/* top border:  ╔═══════╗ */
	r += border_fg + bg_on + tl + hz_line + tr + rst + "\n";

	/* vertical padding */
	for (int i = 0; i < es.pad_v; ++i)
		r += border_fg + bg_on + vl + TermUtils::spaces(w - 2) + vl + rst + "\n";

	/* content line:  ║  ◆ Title  ║ */
	{
		int inner_w = w - 2 - es.pad_l - es.pad_r;
		if (inner_w < 1) inner_w = 1;
		std::string content = es.glyph + title;
		std::string aligned = _align_text(content, inner_w, es.align);

		r += border_fg + bg_on + vl;
		r += TermUtils::spaces(es.pad_l);
		r += text_fg + font_on + aligned;
		r += TermUtils::spaces(es.pad_r);
		r += border_fg + vl + rst + "\n";
	}

	/* vertical padding */
	for (int i = 0; i < es.pad_v; ++i)
		r += border_fg + bg_on + vl + TermUtils::spaces(w - 2) + vl + rst + "\n";

	/* bottom border:  ╚═══════╝ */
	r += border_fg + bg_on + bl + hz_line + br + rst;
}
else
{
	/* ── Simple heading (H2/H3-fallback) ────────────────────── */
	ElemStyle inner = es;
	inner.space_before = 0;
	inner.space_after  = 0;
	r += _render_sep(es) + "\n";
	for (int i = 0; i < es.pad_v; ++i)
		r += _render_bg_blank(es) + "\n";
	r += _render_line(inner, title) + "\n";
	for (int i = 0; i < es.pad_v; ++i)
		r += _render_bg_blank(es) + "\n";
	r += _render_sep(es);
}
r += TermUtils::newlines(es.space_after);
return r;
}

std::string TermStyle::_apply_inline(int font, const std::string& t) const
{
return TermUtils::apply_fg(body_style.fg) + TermUtils::apply_font(font)
 + t + TermUtils::reset();
}

/* ======================================================================
 *  Headings
 * ====================================================================== */

std::string TermStyle::h1(const std::string& t) const { return _render_heading(h1_style, t); }

std::string TermStyle::h2(const std::string& t) const
{
/*
** H2 — left accent bar + bold text + separator underline
**
**  ┃ ▸ Heading Title
**  ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━
*/
std::string rst = TermUtils::reset();
std::string border_fg = TermUtils::apply_fg(h2_style.border);
std::string text_fg   = TermUtils::apply_fg(h2_style.fg);
std::string font_on   = TermUtils::apply_font(h2_style.font);
int w = h2_style.width > 0 ? h2_style.width : 60;

std::string bar     = "\xe2\x94\x83";   /* ┃ */
std::string corner  = "\xe2\x94\x97";   /* ┗ */

std::string r;
r += TermUtils::newlines(h2_style.space_before);

/* text line: ┃ ▸ Title */
r += border_fg + bar + rst;
r += TermUtils::spaces(h2_style.pad_l);
r += text_fg + font_on + h2_style.glyph + t + rst + "\n";

/* underline: ┗━━━━━━━━━ */
r += border_fg + corner;
r += TermUtils::tile_sep(h2_style.sep_str, w - 1);
r += rst;

r += TermUtils::newlines(h2_style.space_after);
return r;
}

std::string TermStyle::h3(const std::string& t) const
{
/*
** H3 — colored glyph + text + subtle dotted underline
**
**  ● Third Level Title
**  ╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌
*/
std::string rst = TermUtils::reset();
std::string text_fg   = TermUtils::apply_fg(h3_style.fg);
std::string border_fg = TermUtils::apply_fg(h3_style.border);
std::string font_on   = TermUtils::apply_font(h3_style.font);
int w = h3_style.width > 0 ? h3_style.width : 60;

std::string r;
r += TermUtils::newlines(h3_style.space_before);

/* text line: ● Title */
r += TermUtils::spaces(h3_style.pad_l);
r += text_fg + font_on + h3_style.glyph + t + rst + "\n";

/* dotted underline */
if (!h3_style.sep_str.empty())
{
	r += TermUtils::spaces(h3_style.pad_l);
	r += border_fg + TermUtils::apply_font(FONT_DIM);
	r += TermUtils::tile_sep(h3_style.sep_str, w - h3_style.pad_l);
	r += rst;
}
r += TermUtils::newlines(h3_style.space_after);
return r;
}

/* ======================================================================
 *  Inline styles
 * ====================================================================== */

std::string TermStyle::text(const std::string& msg) const     { return _render_line(body_style, msg); }

std::string TermStyle::bold(const std::string& msg) const      { return _render_line(bold_style, msg); }
std::string TermStyle::italic(const std::string& msg) const    { return _render_line(italic_style, msg); }
std::string TermStyle::dim(const std::string& msg) const       { return _render_line(dim_style, msg); }
std::string TermStyle::underline(const std::string& msg) const { return _render_line(underline_style, msg); }
std::string TermStyle::strike(const std::string& msg) const    { return _render_line(strike_style, msg); }
std::string TermStyle::code(const std::string& msg) const      { return _render_line(code_style, msg); }

/* ======================================================================
 *  Blocks
 * ====================================================================== */

std::string TermStyle::quote(const std::string& msg) const { return _render_line(quote_style, msg); }
std::string TermStyle::separator() const { return _render_sep(sep_style); }

std::string TermStyle::hr() const
{
ElemStyle heavy = sep_style;
heavy.sep_str = Glyph::HDASH_HVY;
return _render_sep(heavy);
}

std::string TermStyle::section(const std::string& title, const std::string& body) const
{
std::string r;
ElemStyle inner = h2_style;
inner.space_before = 0;
inner.space_after  = 0;
r += _render_sep(sep_style) + "\n";
r += _render_line(inner, title) + "\n";
r += _render_sep(sep_style) + "\n";
r += _render_line(body_style, body) + "\n";
r += _render_sep(sep_style);
return r;
}

/* ======================================================================
 *  Log-level shortcuts
 * ====================================================================== */

std::string TermStyle::info(const std::string& msg) const    { return _render_line(info_style, msg); }
std::string TermStyle::warn(const std::string& msg) const    { return _render_line(warn_style, msg); }
std::string TermStyle::error(const std::string& msg) const   { return _render_line(error_style, msg); }
std::string TermStyle::success(const std::string& msg) const { return _render_line(success_style, msg); }
std::string TermStyle::danger(const std::string& msg) const  { return _render_line(danger_style, msg); }
std::string TermStyle::trace(const std::string& msg) const   { return _render_line(trace_style, msg); }

/* ======================================================================
 *  List items
 * ====================================================================== */

std::string TermStyle::bullet(const std::string& msg) const  { return _render_line(bullet_style, msg); }

std::string TermStyle::ordered(int num, const std::string& msg) const
{
ElemStyle tmp = ol_style;
std::ostringstream oss;
if (!ol_style.body_glyph.empty())
	oss << ol_style.body_glyph;
oss << num;
if (!ol_style.glyph.empty())
	oss << ol_style.glyph;
else
	oss << ". ";
tmp.glyph = oss.str();
return _render_line(tmp, msg);
}

/* ======================================================================
 *  Callout -- multi-line block
 * ====================================================================== */

std::string TermStyle::callout(const std::string& label,
const std::string& l1, const std::string& l2, const std::string& l3,
const std::string& l4, const std::string& l5, const std::string& l6,
const std::string& l7, const std::string& l8, const std::string& l9,
const std::string& l10) const
{
const std::string* lines[10] = {&l1,&l2,&l3,&l4,&l5,&l6,&l7,&l8,&l9,&l10};
int line_count = 0;
for (int i = 0; i < 10; ++i)
if (!lines[i]->empty()) line_count = i + 1;

int gw = TermUtils::vis_len(callout_style.glyph);

/* Phase 1: measure max content width across all lines */
int header_vis = gw + TermUtils::vis_len(label);
int max_content = header_vis;
for (int i = 0; i < line_count; ++i)
{
	if (lines[i]->empty()) continue;
	int lw = gw + TermUtils::vis_len(*lines[i]);
	if (lw > max_content) max_content = lw;
}

int bar_vis = 3; /* " ┃ " */
int min_w = callout_style.width > 0 ? callout_style.width : 60;
int content_w = max_content;
if (content_w + bar_vis + 1 < min_w)
	content_w = min_w - bar_vis - 1;
int total_w = bar_vis + content_w + 1;

std::string rst = TermUtils::reset();
std::string sp  = TermUtils::spaces(callout_style.margin_l);

std::string r;
r += TermUtils::newlines(callout_style.space_before);

/* top blank */
if (callout_style.has_bg)
{
r += sp + TermUtils::apply_bg(callout_style.bg)
   + TermUtils::apply_fg(callout_style.border)
   + std::string(" \xe2\x94\x83") + TermUtils::spaces(total_w - 2) + rst + "\n";
}

/* header */
r += sp;
if (callout_style.has_bg) r += TermUtils::apply_bg(callout_style.bg);
r += TermUtils::apply_fg(callout_style.border);
r += std::string(" \xe2\x94\x83 ");
r += TermUtils::apply_font(FONT_BOLD);
r += callout_style.glyph + label;
{
int pad = content_w - header_vis;
if (pad > 0) r += TermUtils::spaces(pad);
}
r += " " + rst + "\n";

/* body */
for (int i = 0; i < line_count; ++i)
{
if (lines[i]->empty()) continue;
r += sp;
if (callout_style.has_bg) r += TermUtils::apply_bg(callout_style.bg);
r += TermUtils::apply_fg(callout_style.border);
r += std::string(" \xe2\x94\x83 ");
r += TermUtils::apply_fg(callout_style.fg);
r += TermUtils::apply_font(callout_style.font);
r += TermUtils::spaces(gw) + *lines[i];
int used = gw + TermUtils::vis_len(*lines[i]);
int pad = content_w - used;
if (pad > 0) r += TermUtils::spaces(pad);
r += " " + rst + "\n";
}

/* bottom blank */
if (callout_style.has_bg)
{
r += sp + TermUtils::apply_bg(callout_style.bg)
   + TermUtils::apply_fg(callout_style.border)
   + std::string(" \xe2\x94\x83") + TermUtils::spaces(total_w - 2) + rst + "\n";
}

r += TermUtils::newlines(callout_style.space_after);
return r;
}

/* ======================================================================
 *  TermConf -- constructor (the "default" preset values)
 * ====================================================================== */

TermConf::TermConf()
:
global_width(60),

h1_space_before(1), h1_space_after(1),
h2_space_before(1), h2_space_after(0),
h3_space_before(0), h3_space_after(0),

log_space_before(0), log_space_after(0),

sep_space_before(0), sep_space_after(0),
quote_space_before(0), quote_space_after(0),
section_space_before(1), section_space_after(1),
text_space_before(0), text_space_after(0),

h1_fg(220, 160, 255),     h1_border(160, 100, 220),
h2_fg(130, 200, 255),     h2_border( 70, 130, 200),
h3_fg(180, 220, 180),     h3_border( 80, 140,  80),

h1_font(FONT_BOLD),
h2_font(FONT_BOLD),
h3_font(FONT_BOLD | FONT_ITALIC),

h1_glyph(Glyph::DIAMOND),
h2_glyph(Glyph::TRIANGLE),
h3_glyph(Glyph::BULLET),

h1_sep(Glyph::DOUBLE),
h2_sep(Glyph::HDASH_HVY),
h3_sep(Glyph::DOTTED),

h1_align(ALIGN_CENTER),
h2_align(ALIGN_LEFT),
h3_align(ALIGN_LEFT),

info_fg(  0, 200, 255),
warn_fg(255, 200,   0),
error_fg(255,  60,  60),
success_fg(  0, 230, 120),
danger_fg(255,  80,  80),
trace_fg(180, 130, 255),

info_glyph(Glyph::INFO),
warn_glyph(Glyph::WARN),
error_glyph(Glyph::CROSS),
success_glyph(Glyph::CHECK),
danger_glyph(Glyph::SKULL),
trace_glyph(Glyph::CHAIN),

quote_fg(180, 180, 180),
quote_glyph(Glyph::VBAR),
quote_margin(2),

sep_border(100, 100, 100),
sep_str(Glyph::HDASH),

text_fg(210, 210, 210),

bullet_fg(200, 210, 220),
bullet_glyph(Glyph::BULLET),
ol_fg(200, 210, 220),

callout_fg(180, 200, 220),
callout_bg( 20,  30,  45),
callout_border( 60,  80, 110),
callout_glyph(Glyph::VBAR_THIN),
callout_body_glyph(Glyph::VBAR_THIN),
callout_has_bg(true),

h1_bg( 50,  30,  80),
h1_has_bg(true),
h1_pad_v(1)
{}

TermConf::TermConf(const TermConf& o)
: global_width(o.global_width),
  h1_space_before(o.h1_space_before), h1_space_after(o.h1_space_after),
  h2_space_before(o.h2_space_before), h2_space_after(o.h2_space_after),
  h3_space_before(o.h3_space_before), h3_space_after(o.h3_space_after),
  log_space_before(o.log_space_before), log_space_after(o.log_space_after),
  sep_space_before(o.sep_space_before), sep_space_after(o.sep_space_after),
  quote_space_before(o.quote_space_before), quote_space_after(o.quote_space_after),
  section_space_before(o.section_space_before), section_space_after(o.section_space_after),
  text_space_before(o.text_space_before), text_space_after(o.text_space_after),
  h1_fg(o.h1_fg), h1_border(o.h1_border), h2_fg(o.h2_fg), h2_border(o.h2_border),
  h3_fg(o.h3_fg), h3_border(o.h3_border),
  h1_font(o.h1_font), h2_font(o.h2_font), h3_font(o.h3_font),
  h1_glyph(o.h1_glyph), h2_glyph(o.h2_glyph), h3_glyph(o.h3_glyph),
  h1_sep(o.h1_sep), h2_sep(o.h2_sep), h3_sep(o.h3_sep),
  h1_align(o.h1_align), h2_align(o.h2_align), h3_align(o.h3_align),
  info_fg(o.info_fg), warn_fg(o.warn_fg), error_fg(o.error_fg),
  success_fg(o.success_fg), danger_fg(o.danger_fg), trace_fg(o.trace_fg),
  info_glyph(o.info_glyph), warn_glyph(o.warn_glyph), error_glyph(o.error_glyph),
  success_glyph(o.success_glyph), danger_glyph(o.danger_glyph), trace_glyph(o.trace_glyph),
  quote_fg(o.quote_fg), quote_glyph(o.quote_glyph), quote_margin(o.quote_margin),
  sep_border(o.sep_border), sep_str(o.sep_str),
  text_fg(o.text_fg),
  bullet_fg(o.bullet_fg), bullet_glyph(o.bullet_glyph), ol_fg(o.ol_fg),
  callout_fg(o.callout_fg), callout_bg(o.callout_bg), callout_border(o.callout_border),
  callout_glyph(o.callout_glyph), callout_body_glyph(o.callout_body_glyph),
  callout_has_bg(o.callout_has_bg),
  h1_bg(o.h1_bg), h1_has_bg(o.h1_has_bg), h1_pad_v(o.h1_pad_v)
{}

TermConf& TermConf::operator=(const TermConf& o)
{
if (this == &o) return *this;
global_width = o.global_width;
h1_space_before = o.h1_space_before; h1_space_after = o.h1_space_after;
h2_space_before = o.h2_space_before; h2_space_after = o.h2_space_after;
h3_space_before = o.h3_space_before; h3_space_after = o.h3_space_after;
log_space_before = o.log_space_before; log_space_after = o.log_space_after;
sep_space_before = o.sep_space_before; sep_space_after = o.sep_space_after;
quote_space_before = o.quote_space_before; quote_space_after = o.quote_space_after;
section_space_before = o.section_space_before; section_space_after = o.section_space_after;
text_space_before = o.text_space_before; text_space_after = o.text_space_after;
h1_fg = o.h1_fg; h1_border = o.h1_border;
h2_fg = o.h2_fg; h2_border = o.h2_border;
h3_fg = o.h3_fg; h3_border = o.h3_border;
h1_font = o.h1_font; h2_font = o.h2_font; h3_font = o.h3_font;
h1_glyph = o.h1_glyph; h2_glyph = o.h2_glyph; h3_glyph = o.h3_glyph;
h1_sep = o.h1_sep; h2_sep = o.h2_sep; h3_sep = o.h3_sep;
h1_align = o.h1_align; h2_align = o.h2_align; h3_align = o.h3_align;
info_fg = o.info_fg; warn_fg = o.warn_fg; error_fg = o.error_fg;
success_fg = o.success_fg; danger_fg = o.danger_fg; trace_fg = o.trace_fg;
info_glyph = o.info_glyph; warn_glyph = o.warn_glyph; error_glyph = o.error_glyph;
success_glyph = o.success_glyph; danger_glyph = o.danger_glyph; trace_glyph = o.trace_glyph;
quote_fg = o.quote_fg; quote_glyph = o.quote_glyph; quote_margin = o.quote_margin;
sep_border = o.sep_border; sep_str = o.sep_str;
text_fg = o.text_fg;
bullet_fg = o.bullet_fg; bullet_glyph = o.bullet_glyph; ol_fg = o.ol_fg;
callout_fg = o.callout_fg; callout_bg = o.callout_bg; callout_border = o.callout_border;
callout_glyph = o.callout_glyph; callout_body_glyph = o.callout_body_glyph;
callout_has_bg = o.callout_has_bg;
h1_bg = o.h1_bg; h1_has_bg = o.h1_has_bg; h1_pad_v = o.h1_pad_v;
return *this;
}

TermConf::~TermConf() {}

/* ======================================================================
 *  TermConf::apply()
 * ====================================================================== */

void TermConf::apply(TermStyle& ts) const
{
/* h1 */
ts.h1_style.fg = h1_fg; ts.h1_style.bg = h1_bg; ts.h1_style.border = h1_border;
ts.h1_style.font = h1_font; ts.h1_style.align = h1_align;
ts.h1_style.width = global_width; ts.h1_style.glyph = h1_glyph;
ts.h1_style.sep_str = h1_sep;
ts.h1_style.space_before = h1_space_before; ts.h1_style.space_after = h1_space_after;
ts.h1_style.has_bg = h1_has_bg; ts.h1_style.pad_v = h1_pad_v;
/* h2 */
ts.h2_style.fg = h2_fg; ts.h2_style.border = h2_border;
ts.h2_style.font = h2_font; ts.h2_style.align = h2_align;
ts.h2_style.width = global_width; ts.h2_style.glyph = h2_glyph;
ts.h2_style.sep_str = h2_sep;
ts.h2_style.space_before = h2_space_before; ts.h2_style.space_after = h2_space_after;
/* h3 */
ts.h3_style.fg = h3_fg; ts.h3_style.border = h3_border;
ts.h3_style.font = h3_font; ts.h3_style.align = h3_align;
ts.h3_style.width = global_width; ts.h3_style.glyph = h3_glyph;
ts.h3_style.sep_str = h3_sep;
ts.h3_style.space_before = h3_space_before; ts.h3_style.space_after = h3_space_after;
/* text */
ts.body_style.fg = text_fg; ts.body_style.width = global_width;
ts.body_style.space_before = text_space_before; ts.body_style.space_after = text_space_after;
/* info */
ts.info_style.fg = info_fg; ts.info_style.glyph = info_glyph;
ts.info_style.width = global_width;
ts.info_style.space_before = log_space_before; ts.info_style.space_after = log_space_after;
/* warn */
ts.warn_style.fg = warn_fg; ts.warn_style.glyph = warn_glyph;
ts.warn_style.width = global_width;
ts.warn_style.space_before = log_space_before; ts.warn_style.space_after = log_space_after;
/* error */
ts.error_style.fg = error_fg; ts.error_style.glyph = error_glyph;
ts.error_style.width = global_width;
ts.error_style.space_before = log_space_before; ts.error_style.space_after = log_space_after;
/* success */
ts.success_style.fg = success_fg; ts.success_style.glyph = success_glyph;
ts.success_style.width = global_width;
ts.success_style.space_before = log_space_before; ts.success_style.space_after = log_space_after;
/* danger */
ts.danger_style.fg = danger_fg; ts.danger_style.glyph = danger_glyph;
ts.danger_style.width = global_width;
ts.danger_style.space_before = log_space_before; ts.danger_style.space_after = log_space_after;
/* trace */
ts.trace_style.fg = trace_fg; ts.trace_style.glyph = trace_glyph;
ts.trace_style.width = global_width;
ts.trace_style.space_before = log_space_before; ts.trace_style.space_after = log_space_after;
/* quote */
ts.quote_style.fg = quote_fg; ts.quote_style.glyph = quote_glyph;
ts.quote_style.width = global_width; ts.quote_style.margin_l = quote_margin;
ts.quote_style.space_before = quote_space_before; ts.quote_style.space_after = quote_space_after;
/* separator */
ts.sep_style.border = sep_border; ts.sep_style.sep_str = sep_str;
ts.sep_style.width = global_width;
ts.sep_style.space_before = sep_space_before; ts.sep_style.space_after = sep_space_after;
/* bullet list */
ts.bullet_style.fg = bullet_fg; ts.bullet_style.glyph = bullet_glyph;
ts.bullet_style.width = global_width;
/* ordered list */
ts.ol_style.fg = ol_fg; ts.ol_style.width = global_width;
/* callout */
ts.callout_style.fg = callout_fg; ts.callout_style.bg = callout_bg;
ts.callout_style.border = callout_border;
ts.callout_style.glyph = callout_glyph; ts.callout_style.body_glyph = callout_body_glyph;
ts.callout_style.has_bg = callout_has_bg; ts.callout_style.width = global_width;
}

/* ======================================================================
 *  Presets
 * ====================================================================== */

void TermConf::apply_default(TermStyle& ts) { TermConf c; c.apply(ts); }

void TermConf::apply_minimal(TermStyle& ts)
{
TermConf c;
c.global_width = 50;
c.h1_space_before = 1; c.h1_space_after = 0;
c.h2_space_before = 0; c.h2_space_after = 0;
c.section_space_before = 0; c.section_space_after = 0;
c.h1_font = FONT_BOLD; c.h2_font = FONT_BOLD; c.h3_font = FONT_NONE;
c.h1_glyph = Glyph::TRIANGLE; c.h2_glyph = Glyph::BULLET; c.h3_glyph = "";
c.h1_sep = Glyph::HDASH; c.h2_sep = Glyph::HDASH; c.h3_sep = "";
c.h1_fg = Srgb(200, 200, 200); c.h1_border = Srgb(120, 120, 120);
c.h2_fg = Srgb(180, 180, 180); c.h2_border = Srgb(100, 100, 100);
c.apply(ts);
}

void TermConf::apply_compact(TermStyle& ts)
{
TermConf c;
c.global_width = 60;
c.h1_space_before = 0; c.h1_space_after = 0;
c.h2_space_before = 0; c.h2_space_after = 0;
c.h3_space_before = 0; c.h3_space_after = 0;
c.log_space_before = 0; c.log_space_after = 0;
c.sep_space_before = 0; c.sep_space_after = 0;
c.section_space_before = 0; c.section_space_after = 0;
c.apply(ts);
}

void TermConf::apply_wide(TermStyle& ts)
{
TermConf c;
c.global_width = 80;
c.h1_space_before = 2; c.h1_space_after = 1;
c.h2_space_before = 1; c.h2_space_after = 1;
c.h3_space_before = 1; c.h3_space_after = 0;
c.section_space_before = 1; c.section_space_after = 1;
c.h1_fg = Srgb(255, 200, 100); c.h1_border = Srgb(200, 150, 50);
c.h1_sep = Glyph::HDASH_HVY; c.h2_sep = Glyph::DOUBLE;
c.apply(ts);
}

} /* namespace libcpp */
