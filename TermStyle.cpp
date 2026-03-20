#include "TermStyle.hpp"

/* ═══════════════════════════════════════════════════════════
 *  ElemStyle
 * ═══════════════════════════════════════════════════════════ */

TermStyle::ElemStyle::ElemStyle()
    : fg(255, 255, 255), bg(0, 0, 0), border(128, 128, 128), font(0),
      align(LEFT), width(60), padL(1), padR(1), padV(0), marginL(0),
      spaceBefore(0), spaceAfter(0), hasBg(false),
      glyph(""), bodyGlyph(""), sepStr(Glyph::HDASH) {}

TermStyle::ElemStyle::ElemStyle(
        const Srgb& fg_, const Srgb& bg_, const Srgb& brd_, int font_,
        Align a, int w, int pl, int pr, int pv, int ml,
        int sb, int sa, bool useBg,
        const std::string& gl, const std::string& sep)
    : fg(fg_), bg(bg_), border(brd_), font(font_), align(a),
      width(w), padL(pl), padR(pr), padV(pv), marginL(ml),
      spaceBefore(sb), spaceAfter(sa), hasBg(useBg),
      glyph(gl), bodyGlyph(""), sepStr(sep) {}

/* ═══════════════════════════════════════════════════════════
 *  TermStyle – constructor / destructor
 * ═══════════════════════════════════════════════════════════ */

TermStyle::TermStyle()
    //              fg                     bg                    border                font                  align   w   pL pR pV mL  sB sA  hasBg  glyph              sepStr
    : _h1       (Srgb(255, 255, 255), Srgb( 50,  30,  80), Srgb(160, 100, 220), BOLD,                  CENTER, 60, 3, 3, 1, 0, 1, 0, true,  Glyph::DIAMOND,    Glyph::DOUBLE),
      _h2       (Srgb(130, 200, 255), Srgb(  0,   0,   0), Srgb( 70, 130, 200), BOLD,                  LEFT,   60, 1, 1, 0, 0, 1, 0, false, Glyph::TRIANGLE,   Glyph::HDASH_HVY),
      _h3       (Srgb(180, 220, 180), Srgb(  0,   0,   0), Srgb( 80, 140,  80), BOLD | ITALIC,         LEFT,   60, 3, 1, 0, 0, 0, 0, false, Glyph::BULLET,     Glyph::DOTTED),
      _text     (Srgb(210, 210, 210), Srgb(  0,   0,   0), Srgb(128, 128, 128), NONE,                  LEFT,   60, 1, 1, 0, 0, 0, 0, false, "",                 Glyph::HDASH),
      _infoSt   (Srgb(  0, 200, 255), Srgb(  0,   0,   0), Srgb( 60, 120, 160), DIM,                   LEFT,   60, 1, 1, 0, 0, 0, 0, false, Glyph::INFO,       Glyph::HDASH),
      _warnSt   (Srgb(255, 200,   0), Srgb(  0,   0,   0), Srgb(180, 140,   0), BOLD | ITALIC,         LEFT,   60, 1, 1, 0, 0, 0, 0, false, Glyph::WARN,       Glyph::HDASH),
      _errorSt  (Srgb(255,  60,  60), Srgb(  0,   0,   0), Srgb(200,  40,  40), BOLD | UNDERLINE,      LEFT,   60, 1, 1, 0, 0, 0, 0, false, Glyph::CROSS,      Glyph::HDASH),
      _successSt(Srgb(  0, 230, 120), Srgb(  0,   0,   0), Srgb(  0, 160,  80), BOLD,                  LEFT,   60, 1, 1, 0, 0, 0, 0, false, Glyph::CHECK,      Glyph::HDASH),
      _quoteSt  (Srgb(180, 180, 180), Srgb(  0,   0,   0), Srgb(100, 100, 100), ITALIC,                LEFT,   60, 3, 1, 0, 0, 0, 0, false, Glyph::VBAR,       Glyph::HDASH),
      _sepSt    (Srgb(100, 100, 100), Srgb(  0,   0,   0), Srgb(100, 100, 100), DIM,                   LEFT,   60, 0, 0, 0, 0, 0, 0, false, "",                 Glyph::HDASH),
      _calloutSt(Srgb(180, 200, 220), Srgb( 20,  30,  45), Srgb( 60,  80, 110), NONE,                  LEFT,   60, 3, 2, 0, 1, 0, 0, true,  Glyph::VBAR_THIN,  Glyph::HDASH),
      _bulletSt (Srgb(200, 210, 220), Srgb(  0,   0,   0), Srgb(128, 128, 128), NONE,                  LEFT,   60, 3, 1, 0, 0, 0, 0, false, Glyph::BULLET,     Glyph::HDASH),
      _olSt     (Srgb(200, 210, 220), Srgb(  0,   0,   0), Srgb(128, 128, 128), NONE,                  LEFT,   60, 3, 1, 0, 0, 0, 0, false, "",                 Glyph::HDASH)
{}

TermStyle::~TermStyle() {}

/* ═══════════════════════════════════════════════════════════
 *  Style accessors
 * ═══════════════════════════════════════════════════════════ */

TermStyle::ElemStyle& TermStyle::h1Style()      { return _h1; }
TermStyle::ElemStyle& TermStyle::h2Style()      { return _h2; }
TermStyle::ElemStyle& TermStyle::h3Style()      { return _h3; }
TermStyle::ElemStyle& TermStyle::textStyle()    { return _text; }
TermStyle::ElemStyle& TermStyle::infoStyle()    { return _infoSt; }
TermStyle::ElemStyle& TermStyle::warnStyle()    { return _warnSt; }
TermStyle::ElemStyle& TermStyle::errorStyle()   { return _errorSt; }
TermStyle::ElemStyle& TermStyle::successStyle() { return _successSt; }
TermStyle::ElemStyle& TermStyle::quoteStyle()   { return _quoteSt; }
TermStyle::ElemStyle& TermStyle::sepStyle()     { return _sepSt; }
TermStyle::ElemStyle& TermStyle::calloutStyle()  { return _calloutSt; }
TermStyle::ElemStyle& TermStyle::bulletStyle()   { return _bulletSt; }
TermStyle::ElemStyle& TermStyle::olStyle()       { return _olSt; }

/* ═══════════════════════════════════════════════════════════
 *  Backward-compatible static delegates → TermUtils
 * ═══════════════════════════════════════════════════════════ */

std::string TermStyle::reset()  { return TermUtils::reset(); }

std::string TermStyle::tileSep(const std::string& unit, int width) {
    return TermUtils::tileSep(unit, width);
}

std::string TermStyle::newlines(int n) {
    return TermUtils::newlines(n);
}

int TermStyle::visLen(const std::string& s) {
    return TermUtils::visLen(s);
}

std::string TermStyle::alignText(const std::string& txt,
                                  int available, Align a) const {
    int tLen = TermUtils::visLen(txt);
    if (tLen >= available) return txt;
    int gap = available - tLen;
    if (a == CENTER) {
        int l = gap / 2;
        int r = gap - l;
        return TermUtils::spaces(l) + txt + TermUtils::spaces(r);
    } else if (a == RIGHT) {
        return TermUtils::spaces(gap) + txt;
    }
    return txt + TermUtils::spaces(gap);
}

/* ─── render a single styled content line ──────────────── */
std::string TermStyle::renderLine(const ElemStyle& es,
                                   const std::string& content) const {
    std::string r;
    int w = es.width > 0 ? es.width : 60;
    int innerW = w - es.padL - es.padR;
    if (innerW < 1) innerW = 1;

    r += TermUtils::newlines(es.spaceBefore);
    r += TermUtils::spaces(es.marginL);
    if (es.hasBg) r += TermUtils::applyBg(es.bg);
    r += TermUtils::applyFg(es.fg);
    r += TermUtils::applyFont(es.font);
    r += TermUtils::spaces(es.padL);
    r += es.glyph;
    std::string aligned = alignText(content, innerW - TermUtils::visLen(es.glyph), es.align);
    r += aligned;
    r += TermUtils::spaces(es.padR);
    r += TermUtils::reset();
    r += TermUtils::newlines(es.spaceAfter);
    return r;
}

/* ─── render a bg-filled line (for padV / callout body) ── */
std::string TermStyle::renderBgLine(const ElemStyle& es,
                                     const std::string& content) const {
    std::string r;
    int w = es.width > 0 ? es.width : 60;
    int innerW = w - es.padL - es.padR;
    if (innerW < 1) innerW = 1;

    r += TermUtils::spaces(es.marginL);
    if (es.hasBg) r += TermUtils::applyBg(es.bg);
    r += TermUtils::applyFg(es.fg);
    r += TermUtils::applyFont(es.font);
    r += TermUtils::spaces(es.padL);
    std::string aligned = alignText(content, innerW, es.align);
    r += aligned;
    r += TermUtils::spaces(es.padR);
    r += TermUtils::reset();
    return r;
}

/* ─── render an empty bg-filled line (vertical padding) ── */
std::string TermStyle::renderBgBlank(const ElemStyle& es) const {
    std::string r;
    int w = es.width > 0 ? es.width : 60;
    r += TermUtils::spaces(es.marginL);
    if (es.hasBg) r += TermUtils::applyBg(es.bg);
    r += TermUtils::spaces(w);
    r += TermUtils::reset();
    return r;
}

/* ─── render a Unicode separator bar ───────────────────── */
std::string TermStyle::renderSep(const ElemStyle& es) const {
    int w = es.width > 0 ? es.width : 60;
    std::string r;
    r += TermUtils::spaces(es.marginL);
    r += TermUtils::applyFg(es.border);
    r += TermUtils::applyFont(es.font);
    r += TermUtils::tileSep(es.sepStr, w);
    r += TermUtils::reset();
    return r;
}

/* ─── render a heading (sep + padV + title + padV + sep) ── */
std::string TermStyle::renderHeading(const ElemStyle& es,
                                      const std::string& title) const {
    std::string r;
    ElemStyle inner = es;
    inner.spaceBefore = 0;
    inner.spaceAfter  = 0;

    r += TermUtils::newlines(es.spaceBefore);
    r += renderSep(es) + "\n";
    for (int i = 0; i < es.padV; ++i)
        r += renderBgBlank(es) + "\n";
    r += renderLine(inner, title) + "\n";
    for (int i = 0; i < es.padV; ++i)
        r += renderBgBlank(es) + "\n";
    r += renderSep(es);
    r += TermUtils::newlines(es.spaceAfter);
    return r;
}

/* ═══════════════════════════════════════════════════════════
 *  Headings
 * ═══════════════════════════════════════════════════════════ */

std::string TermStyle::h1(const std::string& title) const {
    return renderHeading(_h1, title);
}

std::string TermStyle::h2(const std::string& title) const {
    std::string r;
    ElemStyle inner = _h2;
    inner.spaceBefore = 0;
    inner.spaceAfter  = 0;

    r += newlines(_h2.spaceBefore);
    r += renderLine(inner, title) + "\n";
    r += renderSep(_h2);
    r += newlines(_h2.spaceAfter);
    return r;
}

std::string TermStyle::h3(const std::string& title) const {
    return renderLine(_h3, title);
}

/* ═══════════════════════════════════════════════════════════
 *  Inline styles
 * ═══════════════════════════════════════════════════════════ */

std::string TermStyle::text(const std::string& msg) const {
    return renderLine(_text, msg);
}

std::string TermStyle::bold(const std::string& msg) const {
    return TermUtils::applyFg(_text.fg) + TermUtils::applyFont(BOLD) + msg + TermUtils::reset();
}

std::string TermStyle::italic(const std::string& msg) const {
    return TermUtils::applyFg(_text.fg) + TermUtils::applyFont(ITALIC) + msg + TermUtils::reset();
}

std::string TermStyle::underline(const std::string& msg) const {
    return TermUtils::applyFg(_text.fg) + TermUtils::applyFont(UNDERLINE) + msg + TermUtils::reset();
}

std::string TermStyle::dim(const std::string& msg) const {
    return TermUtils::applyFg(_text.fg) + TermUtils::applyFont(DIM) + msg + TermUtils::reset();
}

std::string TermStyle::strike(const std::string& msg) const {
    return TermUtils::applyFg(_text.fg) + TermUtils::applyFont(STRIKE) + msg + TermUtils::reset();
}

/* ═══════════════════════════════════════════════════════════
 *  Blocks
 * ═══════════════════════════════════════════════════════════ */

std::string TermStyle::quote(const std::string& msg) const {
    return renderLine(_quoteSt, msg);
}

std::string TermStyle::separator() const {
    return renderSep(_sepSt);
}

std::string TermStyle::hr() const {
    ElemStyle heavy = _sepSt;
    heavy.sepStr = Glyph::HDASH_HVY;
    return renderSep(heavy);
}

std::string TermStyle::section(const std::string& title,
                                const std::string& body) const {
    std::string r;
    ElemStyle inner = _h2;
    inner.spaceBefore = 0;
    inner.spaceAfter  = 0;

    r += renderSep(_sepSt) + "\n";
    r += renderLine(inner, title) + "\n";
    r += renderSep(_sepSt) + "\n";
    r += renderLine(_text, body) + "\n";
    r += renderSep(_sepSt);
    return r;
}

/* ═══════════════════════════════════════════════════════════
 *  Log-level shortcuts
 * ═══════════════════════════════════════════════════════════ */

std::string TermStyle::info(const std::string& msg) const {
    return renderLine(_infoSt, msg);
}

std::string TermStyle::warn(const std::string& msg) const {
    return renderLine(_warnSt, msg);
}

std::string TermStyle::error(const std::string& msg) const {
    return renderLine(_errorSt, msg);
}

std::string TermStyle::success(const std::string& msg) const {
    return renderLine(_successSt, msg);
}

/* ═══════════════════════════════════════════════════════════
 *  List items
 * ═══════════════════════════════════════════════════════════ */

std::string TermStyle::bullet(const std::string& msg) const {
    return renderLine(_bulletSt, msg);
}

std::string TermStyle::ordered(int num, const std::string& msg) const {
    ElemStyle tmp = _olSt;
    std::ostringstream oss;
    oss << num << ". ";
    tmp.glyph = oss.str();
    return renderLine(tmp, msg);
}

/* ═══════════════════════════════════════════════════════════
 *  Callout — multi-line block with header and bordered body
 *
 *  ┃ LABEL
 *  ┃ line 1
 *  ┃ line 2
 *  ┃ ...
 * ═══════════════════════════════════════════════════════════ */

std::string TermStyle::callout(const std::string& label,
                                const std::string& l1,
                                const std::string& l2,
                                const std::string& l3,
                                const std::string& l4,
                                const std::string& l5,
                                const std::string& l6,
                                const std::string& l7,
                                const std::string& l8,
                                const std::string& l9,
                                const std::string& l10) const {
    const std::string* lines[10] = {&l1,&l2,&l3,&l4,&l5,&l6,&l7,&l8,&l9,&l10};
    int lineCount = 0;
    for (int i = 0; i < 10; ++i) {
        if (!lines[i]->empty()) lineCount = i + 1;
    }

    std::string r;
    int w = _calloutSt.width > 0 ? _calloutSt.width : 60;
    int innerW = w - _calloutSt.padL - _calloutSt.padR;
    if (innerW < 1) innerW = 1;

    r += TermUtils::newlines(_calloutSt.spaceBefore);

    // header line (bold label)
    std::string hdr;
    hdr += TermUtils::spaces(_calloutSt.marginL);
    if (_calloutSt.hasBg) hdr += TermUtils::applyBg(_calloutSt.bg);
    hdr += TermUtils::applyFg(_calloutSt.border);
    hdr += TermUtils::applyFont(BOLD);
    hdr += TermUtils::spaces(_calloutSt.padL);
    hdr += _calloutSt.glyph;
    std::string hText = alignText(label, innerW - TermUtils::visLen(_calloutSt.glyph), _calloutSt.align);
    hdr += hText;
    hdr += TermUtils::spaces(_calloutSt.padR);
    hdr += TermUtils::reset();
    r += hdr + "\n";

    // body lines (dimmer, with border bar — not the header glyph)
    std::string bGlyph = _calloutSt.bodyGlyph.empty()
        ? _calloutSt.glyph : _calloutSt.bodyGlyph;
    int bGlyphLen = TermUtils::visLen(bGlyph);
    for (int i = 0; i < lineCount; ++i) {
        if (lines[i]->empty()) continue;
        std::string ln;
        ln += TermUtils::spaces(_calloutSt.marginL);
        if (_calloutSt.hasBg) ln += TermUtils::applyBg(_calloutSt.bg);
        ln += TermUtils::applyFg(_calloutSt.fg);
        ln += TermUtils::applyFont(_calloutSt.font);
        ln += TermUtils::spaces(_calloutSt.padL);
        ln += bGlyph;
        std::string bText = alignText(*lines[i], innerW - bGlyphLen, _calloutSt.align);
        ln += bText;
        ln += TermUtils::spaces(_calloutSt.padR);
        ln += TermUtils::reset();
        r += ln + "\n";
    }

    // bottom bg blank line to close the box
    if (_calloutSt.hasBg)
        r += renderBgBlank(_calloutSt);

    r += TermUtils::newlines(_calloutSt.spaceAfter);
    return r;
}
