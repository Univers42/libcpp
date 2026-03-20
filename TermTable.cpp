#include "TermTable.hpp"
#include <sstream>

/* ═══════════════════════════════════════════════════════════
 *  Box-drawing glyphs
 *
 *  Heavy set (━ ┃ ┏ ┓ ┗ ┛ ┣ ┫ ┳ ┻ ╋)
 *  Light set (─ │ ╭ ╮ ╰ ╯ ├ ┤ ┬ ┴ ┼)
 *  We pick at render time based on style.heavy.
 * ═══════════════════════════════════════════════════════════ */

/* heavy */
static const char H_H[]  = "\xe2\x94\x81";  // ━
static const char H_V[]  = "\xe2\x94\x83";  // ┃
static const char H_TL[] = "\xe2\x94\x8f";  // ┏
static const char H_TR[] = "\xe2\x94\x93";  // ┓
static const char H_BL[] = "\xe2\x94\x97";  // ┗
static const char H_BR[] = "\xe2\x94\x9b";  // ┛
static const char H_LT[] = "\xe2\x94\xa3";  // ┣
static const char H_RT[] = "\xe2\x94\xab";  // ┫
static const char H_TD[] = "\xe2\x94\xb3";  // ┳
static const char H_TU[] = "\xe2\x94\xbb";  // ┻
static const char H_X[]  = "\xe2\x95\x8b";  // ╋

/* light */
static const char L_H[]  = "\xe2\x94\x80";  // ─
static const char L_V[]  = "\xe2\x94\x82";  // │
static const char L_LT[] = "\xe2\x94\x9c";  // ├
static const char L_RT[] = "\xe2\x94\xa4";  // ┤
static const char L_TD[] = "\xe2\x94\xac";  // ┬
static const char L_TU[] = "\xe2\x94\xb4";  // ┴
static const char L_X[]  = "\xe2\x94\xbc";  // ┼

/* round corners (always light weight) */
static const char R_TL[] = "\xe2\x95\xad";  // ╭
static const char R_TR[] = "\xe2\x95\xae";  // ╮
static const char R_BL[] = "\xe2\x95\xb0";  // ╰
static const char R_BR[] = "\xe2\x95\xaf";  // ╯

/* ═══════════════════════════════════════════════════════════
 *  Style defaults
 * ═══════════════════════════════════════════════════════════ */

TermTable::Style::Style()
    : frameFg(120, 100, 160),
      headerFg(200, 180, 255),
      cellFg(210, 210, 205),
      titleFg(255, 200, 100),
      cellPadL(1),
      cellPadR(1),
      maxColWidth(40),
      heavy(true),
      roundCorners(false) {}

/* ═══════════════════════════════════════════════════════════
 *  Construction / Destruction
 * ═══════════════════════════════════════════════════════════ */

TermTable::TermTable(int cols)
    : _cols(cols > MAX_COLS ? MAX_COLS : cols),
      _rowCount(0), _hasTitle(false), _hasHeader(false),
      _curCol(0) {}

TermTable::TermTable(int cols, const Style& s)
    : _cols(cols > MAX_COLS ? MAX_COLS : cols),
      _rowCount(0), _hasTitle(false), _hasHeader(false),
      _style(s), _curCol(0) {}

TermTable::~TermTable() {}

/* ═══════════════════════════════════════════════════════════
 *  Public API
 * ═══════════════════════════════════════════════════════════ */

int TermTable::cols() const { return _cols; }
int TermTable::rows() const { return _rowCount; }
TermTable::Style& TermTable::style() { return _style; }
const TermTable::Style& TermTable::style() const { return _style; }

void TermTable::title(const std::string& t) {
    _title = t;
    _hasTitle = true;
}

void TermTable::header(const std::string& c0,
                       const std::string& c1,
                       const std::string& c2,
                       const std::string& c3,
                       const std::string& c4,
                       const std::string& c5,
                       const std::string& c6,
                       const std::string& c7) {
    const std::string* args[] = {&c0,&c1,&c2,&c3,&c4,&c5,&c6,&c7};
    for (int i = 0; i < _cols && i < 8; ++i)
        _headers[i] = *args[i];
    _hasHeader = true;
}

void TermTable::row(const std::string& c0,
                    const std::string& c1,
                    const std::string& c2,
                    const std::string& c3,
                    const std::string& c4,
                    const std::string& c5,
                    const std::string& c6,
                    const std::string& c7) {
    if (_rowCount >= MAX_ROWS) return;
    const std::string* args[] = {&c0,&c1,&c2,&c3,&c4,&c5,&c6,&c7};
    for (int i = 0; i < _cols && i < 8; ++i)
        _cells[_rowCount][i] = *args[i];
    ++_rowCount;
}

void TermTable::cell(const std::string& val) {
    if (_curCol == 0 && _rowCount >= MAX_ROWS) return;
    if (_curCol == 0) {
        /* init new row */
        for (int i = 0; i < _cols; ++i) _cells[_rowCount][i] = "";
    }
    _cells[_rowCount][_curCol] = val;
    ++_curCol;
    if (_curCol >= _cols) {
        _curCol = 0;
        ++_rowCount;
    }
}

/* ═══════════════════════════════════════════════════════════
 *  Column width calculation
 * ═══════════════════════════════════════════════════════════ */

void TermTable::calcWidths(int* widths) const {
    for (int c = 0; c < _cols; ++c) {
        widths[c] = 1; /* min width */
        if (_hasHeader) {
            int hw = TermUtils::visWidth(_headers[c]);
            if (hw > widths[c]) widths[c] = hw;
        }
        for (int r = 0; r < _rowCount; ++r) {
            int cw = TermUtils::visWidth(_cells[r][c]);
            if (cw > widths[c]) widths[c] = cw;
        }
        if (_style.maxColWidth > 0 && widths[c] > _style.maxColWidth)
            widths[c] = _style.maxColWidth;
    }
}

int TermTable::totalSpan(const int* widths) const {
    int span = 0;
    for (int c = 0; c < _cols; ++c)
        span += widths[c] + _style.cellPadL + _style.cellPadR;
    span += _cols - 1; /* inner column separators */
    return span;
}

/* ═══════════════════════════════════════════════════════════
 *  Line drawing
 *
 *  kind:
 *    0 = top plain      (TL ━━━ TR)  no column junctions
 *    1 = top-with-cols  (TL ━┳━ TR)  columns start at top
 *    2 = mid cross      (LT ━╋━ RT)  cols above and below
 *    3 = bottom-cols    (BL ━┻━ BR)  columns close at bottom
 *    4 = full-span mid  (LT ━━━ RT)  no column junctions
 *    8 = col-open       (LT ━┳━ RT)  opening columns mid-table
 *    9 = col-close      (LT ━┻━ RT)  closing columns mid-table
 * ═══════════════════════════════════════════════════════════ */

std::string TermTable::hline(const int* widths, int kind) const {
    std::string rst = TermUtils::reset();
    std::string fg  = TermUtils::applyFg(_style.frameFg);

    const char* h   = _style.heavy ? H_H  : L_H;
    const char* lt  = _style.heavy ? H_LT : L_LT;
    const char* rt  = _style.heavy ? H_RT : L_RT;
    const char* tl  = _style.heavy ? H_TL : R_TL;
    const char* tr  = _style.heavy ? H_TR : R_TR;
    const char* bl  = _style.heavy ? H_BL : R_BL;
    const char* br  = _style.heavy ? H_BR : R_BR;
    const char* td  = _style.heavy ? H_TD : L_TD;
    const char* tu  = _style.heavy ? H_TU : L_TU;
    const char* x   = _style.heavy ? H_X  : L_X;

    if (_style.roundCorners) {
        tl = R_TL; tr = R_TR; bl = R_BL; br = R_BR;
    }

    std::string r = fg;
    int span = totalSpan(widths);

    if (kind == 0) {
        r += tl;
        r += TermUtils::repCh(h, span);
        r += tr;
    } else if (kind == 1) {
        r += tl;
        for (int c = 0; c < _cols; ++c) {
            r += TermUtils::repCh(h, widths[c] + _style.cellPadL + _style.cellPadR);
            if (c < _cols - 1)
                r += td;
        }
        r += tr;
    } else if (kind == 3) {
        r += bl;
        for (int c = 0; c < _cols; ++c) {
            r += TermUtils::repCh(h, widths[c] + _style.cellPadL + _style.cellPadR);
            if (c < _cols - 1)
                r += tu;
        }
        r += br;
    } else if (kind == 4) {
        r += lt;
        r += TermUtils::repCh(h, span);
        r += rt;
    } else {
        const char* jn = x;
        if (kind == 8)  jn = td;
        if (kind == 9)  jn = tu;

        r += lt;
        for (int c = 0; c < _cols; ++c) {
            r += TermUtils::repCh(h, widths[c] + _style.cellPadL + _style.cellPadR);
            if (c < _cols - 1)
                r += jn;
        }
        r += rt;
    }
    r += rst;
    r += "\n";
    return r;
}

/* ═══════════════════════════════════════════════════════════
 *  Row rendering
 * ═══════════════════════════════════════════════════════════ */

std::string TermTable::dataRow(const int* widths,
                               const std::string* cells,
                               const Srgb& fg,
                               bool bold) const {
    std::string rst = TermUtils::reset();
    std::string ffg = TermUtils::applyFg(_style.frameFg);
    const char* v   = _style.heavy ? H_V : L_V;
    const char* iv  = L_V;

    std::string r;
    r += ffg + v + rst;
    for (int c = 0; c < _cols; ++c) {
        r += TermUtils::spaces(_style.cellPadL);
        r += TermUtils::applyFg(fg);
        if (bold) r += TermUtils::applyFont(TermUtils::BOLD);
        r += TermUtils::fitTo(cells[c], widths[c]);
        r += rst;
        r += TermUtils::spaces(_style.cellPadR);
        if (c < _cols - 1)
            r += ffg + iv + rst;
    }
    r += ffg + v + rst;
    r += "\n";
    return r;
}

std::string TermTable::fullSpanRow(const int* widths,
                                   const std::string& text,
                                   const Srgb& fg,
                                   bool bold) const {
    std::string rst = TermUtils::reset();
    std::string ffg = TermUtils::applyFg(_style.frameFg);
    const char* v   = _style.heavy ? H_V : L_V;

    int span = totalSpan(widths);
    int tw = TermUtils::visWidth(text);
    int pad = span - tw - 1;
    if (pad < 0) pad = 0;

    std::string r;
    r += ffg + v + rst;
    r += " ";
    r += TermUtils::applyFg(fg);
    if (bold) r += TermUtils::applyFont(TermUtils::BOLD);
    r += text;
    r += rst;
    r += std::string(pad, ' ');
    r += ffg + v + rst;
    r += "\n";
    return r;
}

/* ═══════════════════════════════════════════════════════════
 *  render() — assemble the full table
 * ═══════════════════════════════════════════════════════════ */

std::string TermTable::render() const {
    int widths[MAX_COLS];
    calcWidths(widths);
    std::string r;

    if (_hasTitle) {
        /* title is full-span — top has no column junctions */
        r += hline(widths, 0);
        r += fullSpanRow(widths, _title, _style.titleFg, true);
        if (_hasHeader || _rowCount > 0) {
            r += hline(widths, 8);  /* col-open mid ┳ */
        }
    } else if (_hasHeader || _rowCount > 0) {
        /* no title, columns start at the very top */
        r += hline(widths, 1);  /* top-with-cols ┳ */
    } else {
        r += hline(widths, 0);
    }

    if (_hasHeader) {
        r += dataRow(widths, _headers, _style.headerFg, true);
        if (_rowCount > 0)
            r += hline(widths, 2);  /* cross ╋ — cols above and below */
        /* else columns close at bottom */
    }

    for (int i = 0; i < _rowCount; ++i) {
        r += dataRow(widths, _cells[i], _style.cellFg, false);
    }

    /* bottom — always has ┻ junctions when cols are open */
    r += hline(widths, 3);
    return r;
}
