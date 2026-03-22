/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   style.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/22 10:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_TERM_STYLE_HPP
#define LIBCPP_TERM_STYLE_HPP

#include <sstream>
#include <string>
#include "libcpp/term/color.hpp"

namespace libcpp {

/* ======================================================================
 *  TermUtils -- static terminal / UTF-8 helpers
 * ====================================================================== */
struct TermUtils {
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

/* ======================================================================
 *  Font flags
 * ====================================================================== */
enum FontFlag {
  FONT_NONE = 0,
  FONT_BOLD = 1 << 0,
  FONT_DIM = 1 << 1,
  FONT_ITALIC = 1 << 2,
  FONT_UNDERLINE = 1 << 3,
  FONT_BLINK = 1 << 4,
  FONT_REVERSE = 1 << 5,
  FONT_STRIKE = 1 << 6
};

/* ======================================================================
 *  Alignment
 * ====================================================================== */
enum Align { ALIGN_LEFT = 0, ALIGN_CENTER = 1, ALIGN_RIGHT = 2 };

/* ======================================================================
 *  Unicode glyph constants
 * ====================================================================== */
namespace Glyph {
/* bullets / markers */
static const char BULLET[] = "\xe2\x97\x8f ";
static const char DIAMOND[] = "\xe2\x97\x86 ";
static const char TRIANGLE[] = "\xe2\x96\xb8 ";
static const char ARROW[] = "\xe2\x9e\x9c ";
static const char STAR[] = "\xe2\x98\x85 ";
static const char CIRCLE[] = "\xe2\x97\x8b ";
static const char DOT[] = "\xc2\xb7 ";

/* log-level icons */
static const char INFO[] = "\xe2\x84\xb9  ";
static const char WARN[] = "\xe2\x9a\xa0  ";
static const char CHECK[] = "\xe2\x9c\x94 ";
static const char CROSS[] = "\xe2\x9c\x97 ";
static const char SKULL[] = "\xe2\x98\xa0  ";
static const char CHAIN[] = "\xe2\x97\x87 ";

/* box drawing */
static const char VBAR[] = "\xe2\x94\x83 ";
static const char VBAR_THIN[] = "\xe2\x94\x82 ";
static const char HDASH[] = "\xe2\x94\x80";
static const char HDASH_HVY[] = "\xe2\x94\x81";
static const char DOUBLE[] = "\xe2\x95\x90";
static const char DOTTED[] = "\xe2\x95\x8c";
static const char VDASH[] = "\xe2\x94\x82";
}  // namespace Glyph

/* ======================================================================
 *  ElemStyle -- per-element visual configuration
 * ====================================================================== */
struct ElemStyle {
  Srgb fg;
  Srgb bg;
  Srgb border;
  int font;
  Align align;
  int width;
  int pad_l;
  int pad_r;
  int pad_v;
  int margin_l;
  int space_before;
  int space_after;
  bool has_bg;
  std::string glyph;
  std::string body_glyph;
  std::string sep_str;

  ElemStyle();
  ElemStyle(const Srgb& fg_, const Srgb& bg_, const Srgb& brd_, int font_,
            Align a, int w, int pl, int pr, int pv, int ml, int sb, int sa,
            bool bg, const std::string& gl, const std::string& sep);
  ElemStyle(const ElemStyle& other);
  ElemStyle& operator=(const ElemStyle& other);
  ~ElemStyle();
};

/* ======================================================================
 *  TermStyle -- markdown-like terminal renderer(OCF)
 * ====================================================================== */
class TermStyle {
 public:
  TermStyle();
  TermStyle(const TermStyle& other);
  TermStyle& operator=(const TermStyle& other);
  ~TermStyle();

  /* element styles(public for direct access) */
  ElemStyle h1_style;
  ElemStyle h2_style;
  ElemStyle h3_style;
  ElemStyle body_style;
  ElemStyle quote_style;
  ElemStyle info_style;
  ElemStyle warn_style;
  ElemStyle error_style;
  ElemStyle success_style;
  ElemStyle danger_style;
  ElemStyle trace_style;
  ElemStyle sep_style;
  ElemStyle section_style;
  ElemStyle callout_style;
  ElemStyle bullet_style;
  ElemStyle ol_style;
  ElemStyle bold_style;
  ElemStyle italic_style;
  ElemStyle underline_style;
  ElemStyle strike_style;
  ElemStyle dim_style;
  ElemStyle code_style;

  /* headings */
  std::string h1(const std::string& text) const;
  std::string h2(const std::string& text) const;
  std::string h3(const std::string& text) const;

  /* inline styles */
  std::string text(const std::string& msg) const;
  std::string bold(const std::string& msg) const;
  std::string italic(const std::string& msg) const;
  std::string dim(const std::string& msg) const;
  std::string underline(const std::string& msg) const;
  std::string strike(const std::string& msg) const;
  std::string code(const std::string& msg) const;

  /* blocks */
  std::string quote(const std::string& msg) const;
  std::string separator() const;
  std::string hr() const;
  std::string section(const std::string& title, const std::string& body) const;

  /* callout(multi-line block) */
  std::string callout(const std::string& label, const std::string& l1,
                      const std::string& l2 = "", const std::string& l3 = "",
                      const std::string& l4 = "", const std::string& l5 = "",
                      const std::string& l6 = "", const std::string& l7 = "",
                      const std::string& l8 = "", const std::string& l9 = "",
                      const std::string& l10 = "") const;

  /* log-level shortcuts */
  std::string info(const std::string& msg) const;
  std::string warn(const std::string& msg) const;
  std::string error(const std::string& msg) const;
  std::string success(const std::string& msg) const;
  std::string danger(const std::string& msg) const;
  std::string trace(const std::string& msg) const;

  /* list items */
  std::string bullet(const std::string& msg) const;
  std::string ordered(int num, const std::string& msg) const;

 private:
  std::string _align_text(const std::string& txt, int available, Align a) const;
  std::string _render_line(const ElemStyle& es,
                           const std::string& content) const;
  std::string _render_bg_line(const ElemStyle& es,
                              const std::string& content) const;
  std::string _render_bg_blank(const ElemStyle& es) const;
  std::string _render_sep(const ElemStyle& es) const;
  std::string _render_heading(const ElemStyle& es,
                              const std::string& title) const;
  std::string _apply_inline(int font, const std::string& text) const;
};

/* ======================================================================
 *  TermConf -- configuration presets for TermStyle(OCF)
 * ====================================================================== */
class TermConf {
 public:
  int global_width;

  int h1_space_before;
  int h1_space_after;
  int h2_space_before;
  int h2_space_after;
  int h3_space_before;
  int h3_space_after;

  int log_space_before;
  int log_space_after;

  int sep_space_before;
  int sep_space_after;
  int quote_space_before;
  int quote_space_after;
  int section_space_before;
  int section_space_after;
  int text_space_before;
  int text_space_after;

  Srgb h1_fg;
  Srgb h1_border;
  Srgb h2_fg;
  Srgb h2_border;
  Srgb h3_fg;
  Srgb h3_border;

  int h1_font;
  int h2_font;
  int h3_font;

  std::string h1_glyph;
  std::string h2_glyph;
  std::string h3_glyph;

  std::string h1_sep;
  std::string h2_sep;
  std::string h3_sep;

  Align h1_align;
  Align h2_align;
  Align h3_align;

  Srgb info_fg;
  Srgb warn_fg;
  Srgb error_fg;
  Srgb success_fg;
  Srgb danger_fg;
  Srgb trace_fg;

  std::string info_glyph;
  std::string warn_glyph;
  std::string error_glyph;
  std::string success_glyph;
  std::string danger_glyph;
  std::string trace_glyph;

  Srgb quote_fg;
  std::string quote_glyph;
  int quote_margin;

  Srgb sep_border;
  std::string sep_str;

  Srgb text_fg;

  Srgb bullet_fg;
  std::string bullet_glyph;
  Srgb ol_fg;

  Srgb callout_fg;
  Srgb callout_bg;
  Srgb callout_border;
  std::string callout_glyph;
  std::string callout_body_glyph;
  bool callout_has_bg;

  Srgb h1_bg;
  bool h1_has_bg;
  int h1_pad_v;

  TermConf();
  TermConf(const TermConf& other);
  TermConf& operator=(const TermConf& other);
  ~TermConf();

  void apply(TermStyle& ts) const;

  static void apply_default(TermStyle& ts);
  static void apply_minimal(TermStyle& ts);
  static void apply_compact(TermStyle& ts);
  static void apply_wide(TermStyle& ts);
};

} /* namespace libcpp */

#endif /* LIBCPP_TERM_STYLE_HPP */
