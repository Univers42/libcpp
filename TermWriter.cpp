#include "TermWriter.hpp"

/* ═══════════════════════════════════════════════════════════
 *  Construction / Destruction
 * ═══════════════════════════════════════════════════════════ */

TermWriter::TermWriter(TermStyle& ts)
    : _ts(ts), _os(std::cout), _calloutCount(0),
      _inCallout(false), _calloutLineCount(0),
      _inTable(false), _pendingTable(0), _tableHasHeader(false) {}

TermWriter::TermWriter(TermStyle& ts, std::ostream& os)
    : _ts(ts), _os(os), _calloutCount(0),
      _inCallout(false), _calloutLineCount(0),
      _inTable(false), _pendingTable(0), _tableHasHeader(false) {}

TermWriter::~TermWriter() {
    delete _pendingTable;
}

/* ═══════════════════════════════════════════════════════════
 *  Callout registry
 * ═══════════════════════════════════════════════════════════ */

void TermWriter::defineCallout(const std::string& name,
                                const TermStyle::ElemStyle& style) {
    /* overwrite if already exists */
    for (int i = 0; i < _calloutCount; ++i) {
        if (_callouts[i].name == name) {
            _callouts[i].style = style;
            return;
        }
    }
    if (_calloutCount < MAX_CALLOUTS) {
        _callouts[_calloutCount].name  = name;
        _callouts[_calloutCount].style = style;
        _callouts[_calloutCount].used  = true;
        ++_calloutCount;
    }
}

TermStyle::ElemStyle* TermWriter::getCallout(const std::string& name) {
    for (int i = 0; i < _calloutCount; ++i) {
        if (_callouts[i].name == name)
            return &_callouts[i].style;
    }
    return 0;
}

const TermStyle::ElemStyle*
TermWriter::getCallout(const std::string& name) const {
    for (int i = 0; i < _calloutCount; ++i) {
        if (_callouts[i].name == name)
            return &_callouts[i].style;
    }
    return 0;
}

/* ═══════════════════════════════════════════════════════════
 *  Internal helpers
 * ═══════════════════════════════════════════════════════════ */

void TermWriter::_emit(const std::string& s) {
    _buf << s << "\n";
}

/* ── render a callout block with a given style ─────────── */
std::string TermWriter::_renderCallout(
        const TermStyle::ElemStyle& es,
        const std::string& label,
        const std::string* lines,
        int count) const {
    /*
     * Two-pass callout renderer:
     *   Pass 1 — measure the visual width of every content line
     *            (header label + each body line, including glyph).
     *            Compute maxW = max of all, then effective box width
     *            = max(es.width, maxW + padL + padR).
     *   Pass 2 — render every line, padding each to the effective
     *            box width so the background fills uniformly.
     */

    int glyphLen  = TermUtils::visLen(es.glyph);
    std::string bGlyph = es.bodyGlyph.empty() ? es.glyph : es.bodyGlyph;
    int bGlyphLen = TermUtils::visLen(bGlyph);

    /* ── Pass 1: measure ──────────────────────────────── */
    int maxContent = 0;                       /* widest content (glyph + text) */

    /* header: glyph + label */
    int hdrW = glyphLen + TermUtils::visLen(label);
    if (hdrW > maxContent) maxContent = hdrW;

    /* body lines: bodyGlyph + text */
    for (int i = 0; i < count; ++i) {
        if (lines[i].empty()) continue;
        int lw = bGlyphLen + TermUtils::visLen(lines[i]);
        if (lw > maxContent) maxContent = lw;
    }

    /* effective box width = max(configured, needed) */
    int minW = es.width > 0 ? es.width : 60;
    int innerW = maxContent;
    if (innerW < minW - es.padL - es.padR)
        innerW = minW - es.padL - es.padR;
    int w = innerW + es.padL + es.padR;

    /* ── Pass 2: render ───────────────────────────────── */
    std::string r;
    std::string sp  = TermUtils::spaces(es.marginL);
    std::string plS = TermUtils::spaces(es.padL);
    std::string prS = TermUtils::spaces(es.padR);
    std::string rst = TermUtils::reset();

    r += TermUtils::newlines(es.spaceBefore);

    /* top bg blank — opening bar */
    if (es.hasBg) {
        r += sp + TermUtils::applyBg(es.bg)
           + TermUtils::spaces(w) + rst + "\n";
    }

    /* header line (bold, border color) */
    {
        r += sp;
        if (es.hasBg) r += TermUtils::applyBg(es.bg);
        r += TermUtils::applyFg(es.border);
        r += TermUtils::applyFont(TermUtils::BOLD);
        r += plS + es.glyph;

        std::string txt = label;
        int avail = innerW - glyphLen;
        int tLen  = TermUtils::visLen(txt);
        if (tLen < avail) txt += std::string(avail - tLen, ' ');
        r += txt + prS + rst + "\n";
    }

    /* body lines (fg color, body glyph) */
    for (int i = 0; i < count; ++i) {
        if (lines[i].empty()) continue;
        r += sp;
        if (es.hasBg) r += TermUtils::applyBg(es.bg);
        r += TermUtils::applyFg(es.fg);
        r += TermUtils::applyFont(es.font);
        r += plS + bGlyph;

        std::string txt = lines[i];
        int avail = innerW - bGlyphLen;
        int tLen  = TermUtils::visLen(txt);
        if (tLen < avail) txt += std::string(avail - tLen, ' ');
        r += txt + prS + rst + "\n";
    }

    /* bottom bg blank — closing bar */
    if (es.hasBg) {
        r += sp + TermUtils::applyBg(es.bg)
           + TermUtils::spaces(w) + rst + "\n";
    }

    r += TermUtils::newlines(es.spaceAfter);
    return r;
}

/* ═══════════════════════════════════════════════════════════
 *  Imperative API
 * ═══════════════════════════════════════════════════════════ */

TermWriter& TermWriter::h1(const std::string& t) {
    _emit(_ts.h1(t)); return *this;
}
TermWriter& TermWriter::h2(const std::string& t) {
    _emit(_ts.h2(t)); return *this;
}
TermWriter& TermWriter::h3(const std::string& t) {
    _emit(_ts.h3(t)); return *this;
}
TermWriter& TermWriter::text(const std::string& m) {
    _emit(_ts.text(m)); return *this;
}
TermWriter& TermWriter::bold(const std::string& m) {
    _emit(_ts.bold(m)); return *this;
}
TermWriter& TermWriter::italic(const std::string& m) {
    _emit(_ts.italic(m)); return *this;
}
TermWriter& TermWriter::dim(const std::string& m) {
    _emit(_ts.dim(m)); return *this;
}
TermWriter& TermWriter::underline(const std::string& m) {
    _emit(_ts.underline(m)); return *this;
}
TermWriter& TermWriter::strike(const std::string& m) {
    _emit(_ts.strike(m)); return *this;
}
TermWriter& TermWriter::quote(const std::string& m) {
    _emit(_ts.quote(m)); return *this;
}
TermWriter& TermWriter::info(const std::string& m) {
    _emit(_ts.info(m)); return *this;
}
TermWriter& TermWriter::warn(const std::string& m) {
    _emit(_ts.warn(m)); return *this;
}
TermWriter& TermWriter::error(const std::string& m) {
    _emit(_ts.error(m)); return *this;
}
TermWriter& TermWriter::success(const std::string& m) {
    _emit(_ts.success(m)); return *this;
}
TermWriter& TermWriter::danger(const std::string& m) {
    _emit(_ts.danger(m)); return *this;
}
TermWriter& TermWriter::trace(const std::string& m) {
    _emit(_ts.trace(m)); return *this;
}
TermWriter& TermWriter::bullet(const std::string& m) {
    _emit(_ts.bullet(m)); return *this;
}
TermWriter& TermWriter::ordered(int num, const std::string& m) {
    _emit(_ts.ordered(num, m)); return *this;
}
TermWriter& TermWriter::sep() {
    _emit(_ts.separator()); return *this;
}
TermWriter& TermWriter::hr() {
    _emit(_ts.hr()); return *this;
}
TermWriter& TermWriter::nl() {
    _buf << "\n"; return *this;
}
TermWriter& TermWriter::section(const std::string& t,
                                 const std::string& b) {
    _emit(_ts.section(t, b)); return *this;
}

/* ── named callout ─────────────────────────────────────── */
TermWriter& TermWriter::callout(const std::string& type,
                                 const std::string& l1,
                                 const std::string& l2,
                                 const std::string& l3,
                                 const std::string& l4,
                                 const std::string& l5,
                                 const std::string& l6,
                                 const std::string& l7,
                                 const std::string& l8,
                                 const std::string& l9,
                                 const std::string& l10) {
    const std::string arr[10] = {l1,l2,l3,l4,l5,l6,l7,l8,l9,l10};
    int count = 0;
    for (int i = 0; i < 10; ++i) {
        if (!arr[i].empty()) count = i + 1;
    }

    /* look for a user-defined preset first */
    const TermStyle::ElemStyle* custom = getCallout(type);
    if (custom) {
        _emit(_renderCallout(*custom, type, arr, count));
    } else {
        /* fall back to the default callout style from TermStyle */
        _emit(_renderCallout(_ts.calloutStyle(), type, arr, count));
    }
    return *this;
}

/* ═══════════════════════════════════════════════════════════
 *  Table (imperative)
 * ═══════════════════════════════════════════════════════════ */

TermWriter& TermWriter::table(const TermTable& t) {
    std::string rendered = t.render();
    /* render() ends with \n, _emit adds another — trim trailing */
    if (!rendered.empty() && rendered[rendered.size() - 1] == '\n')
        rendered.erase(rendered.size() - 1);
    _emit(rendered);
    return *this;
}

TermTable::Style& TermWriter::tableStyle() { return _tableStyle; }
const TermTable::Style& TermWriter::tableStyle() const { return _tableStyle; }

/* ═══════════════════════════════════════════════════════════
 *  Output
 * ═══════════════════════════════════════════════════════════ */

std::string TermWriter::str() const {
    return _buf.str();
}

void TermWriter::flush() {
    if (_inCallout) _flushCallout();
    if (_inTable)   _flushTable();
    _os << _buf.str();
    _buf.str("");
    _buf.clear();
}

void TermWriter::clear() {
    _buf.str("");
    _buf.clear();
}

TermStyle& TermWriter::style() { return _ts; }
const TermStyle& TermWriter::style() const { return _ts; }

/* ═══════════════════════════════════════════════════════════
 *  Markdown parser — GNL-style, line by line
 *
 *  Reads the input string line-by-line and dispatches
 *  each line to the appropriate TermStyle method based
 *  on the leading prefix.
 * ═══════════════════════════════════════════════════════════ */

TermWriter& TermWriter::parse(const std::string& markdown) {
    std::istringstream iss(markdown);
    std::string line;
    while (std::getline(iss, line)) {
        _parseLine(line);
    }
    /* close any open blocks */
    if (_inCallout) _flushCallout();
    if (_inTable)   _flushTable();
    return *this;
}

TermWriter& TermWriter::parseFile(const std::string& path) {
    std::ifstream ifs(path.c_str());
    if (!ifs.is_open()) {
        error("parseFile: cannot open \"" + path + "\"");
        return *this;
    }
    std::string line;
    while (std::getline(ifs, line)) {
        _parseLine(line);
    }
    if (_inCallout) _flushCallout();
    if (_inTable)   _flushTable();
    return *this;
}

/* ═══════════════════════════════════════════════════════════
 *  operator<< — stream-style line feeder
 *
 *  Each << feeds exactly ONE line through _parseLine().
 *  Supports multi-line strings too (splits on \n).
 *  No trailing \n needed from the caller.
 * ═══════════════════════════════════════════════════════════ */

TermWriter& TermWriter::operator<<(const std::string& line) {
    /* if the string contains \n, split and feed each sub-line */
    if (line.find('\n') != std::string::npos) {
        std::istringstream iss(line);
        std::string sub;
        while (std::getline(iss, sub)) {
            _parseLine(sub);
        }
    } else {
        _parseLine(line);
    }
    return *this;
}

TermWriter& TermWriter::operator<<(const char* line) {
    return *this << std::string(line);
}

/* ── helper: trim leading spaces (returns trimmed + indent count) */
static std::string ltrim(const std::string& s, int& indent) {
    std::size_t i = 0;
    while (i < s.size() && s[i] == ' ') ++i;
    indent = static_cast<int>(i);
    return s.substr(i);
}

/* ── helper: starts-with ───────────────────────────────── */
static bool startsWith(const std::string& s, const std::string& prefix) {
    if (prefix.size() > s.size()) return false;
    return s.compare(0, prefix.size(), prefix) == 0;
}

/* ── helper: strip prefix and trim ─────────────────────── */
static std::string stripPrefix(const std::string& s, std::size_t n) {
    std::size_t i = n;
    while (i < s.size() && s[i] == ' ') ++i;
    return s.substr(i);
}

/* ── helper: is the string only repetitions of a char? ─── */
static bool isRepeatedChar(const std::string& s, char c) {
    if (s.empty()) return false;
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (s[i] != c) return false;
    }
    return true;
}

/* ── helper: check if string looks like "N. text" ──────── */
static bool isOrderedList(const std::string& s, int& num, std::string& rest) {
    std::size_t i = 0;
    while (i < s.size() && s[i] >= '0' && s[i] <= '9') ++i;
    if (i == 0 || i >= s.size()) return false;
    if (s[i] != '.') return false;
    std::istringstream iss(s.substr(0, i));
    iss >> num;
    rest = stripPrefix(s, i + 1);
    return true;
}

void TermWriter::_parseLine(const std::string& raw) {
    int indent = 0;
    std::string line = ltrim(raw, indent);
    (void)indent;

    /* ── inside a callout block? ───────────────────────── */
    if (_inCallout) {
        /* "> text" continues the callout */
        if (startsWith(line, "> ")) {
            if (_calloutLineCount < 10) {
                _calloutLines[_calloutLineCount++] = stripPrefix(line, 2);
            }
            return;
        }
        /* ">" alone or anything else closes the callout */
        _flushCallout();
        if (line == ">") return; // consumed the close marker
        /* fall through to parse the current line normally */
    }

    /* ── inside a table? ───────────────────────────────── */
    if (_inTable) {
        if (_isTableLine(line)) {
            if (_isSepLine(line)) {
                /* separator row — marks header boundary */
                _tableHasHeader = true;
            } else {
                /* data or header row */
                std::string cells[TermTable::MAX_COLS];
                int n = _splitPipe(line, cells, _pendingTable->cols());
                if (!_tableHasHeader) {
                    /* first real row = header */
                    _pendingTable->header(
                        n > 0 ? cells[0] : "", n > 1 ? cells[1] : "",
                        n > 2 ? cells[2] : "", n > 3 ? cells[3] : "",
                        n > 4 ? cells[4] : "", n > 5 ? cells[5] : "",
                        n > 6 ? cells[6] : "", n > 7 ? cells[7] : "");
                } else {
                    /* body row */
                    _pendingTable->row(
                        n > 0 ? cells[0] : "", n > 1 ? cells[1] : "",
                        n > 2 ? cells[2] : "", n > 3 ? cells[3] : "",
                        n > 4 ? cells[4] : "", n > 5 ? cells[5] : "",
                        n > 6 ? cells[6] : "", n > 7 ? cells[7] : "");
                }
            }
            return;
        }
        /* not a table line — flush and fall through */
        _flushTable();
    }

    /* ── table start: | ... | ──────────────────────────── */
    if (_isTableLine(line) && !_isSepLine(line)) {
        /* count columns from first row */
        std::string cells[TermTable::MAX_COLS];
        int n = _splitPipe(line, cells, TermTable::MAX_COLS);
        if (n > 0) {
            delete _pendingTable;
            _pendingTable = new TermTable(n, _tableStyle);
            _inTable = true;
            _tableHasHeader = false;
            _pendingTable->header(
                n > 0 ? cells[0] : "", n > 1 ? cells[1] : "",
                n > 2 ? cells[2] : "", n > 3 ? cells[3] : "",
                n > 4 ? cells[4] : "", n > 5 ? cells[5] : "",
                n > 6 ? cells[6] : "", n > 7 ? cells[7] : "");
            return;
        }
    }

    /* ── blank line ────────────────────────────────────── */
    if (line.empty()) {
        nl();
        return;
    }

    /* ── separator: --- ────────────────────────────────── */
    if (line.size() >= 3 && isRepeatedChar(line, '-')) {
        sep();
        return;
    }

    /* ── heavy rule: === ───────────────────────────────── */
    if (line.size() >= 3 && isRepeatedChar(line, '=')) {
        hr();
        return;
    }

    /* ── headings: # ## ### ────────────────────────────── */
    if (startsWith(line, "### ")) {
        h3(stripPrefix(line, 4));
        return;
    }
    if (startsWith(line, "## ")) {
        h2(stripPrefix(line, 3));
        return;
    }
    if (startsWith(line, "# ")) {
        h1(stripPrefix(line, 2));
        return;
    }

    /* ── callout open: >![type] label ──────────────────── */
    if (startsWith(line, ">![")) {
        std::size_t close = line.find(']', 3);
        if (close != std::string::npos) {
            _inCallout = true;
            _calloutType = line.substr(3, close - 3);
            _calloutLabel = stripPrefix(line, close + 1);
            if (_calloutLabel.empty())
                _calloutLabel = _calloutType;
            _calloutLineCount = 0;
            for (int i = 0; i < 10; ++i) _calloutLines[i] = "";
            return;
        }
    }

    /* ── blockquote: > text ────────────────────────────── */
    if (startsWith(line, "> ")) {
        quote(stripPrefix(line, 2));
        return;
    }

    /* ── log levels: !i !w !x !v !d !t ──────────────────── */
    if (line.size() >= 3 && line[0] == '!') {
        char c = line[1];
        if (c == 'i' && line[2] == ' ') { info(stripPrefix(line, 3));    return; }
        if (c == 'w' && line[2] == ' ') { warn(stripPrefix(line, 3));    return; }
        if (c == '!' && line[2] == ' ') { warn(stripPrefix(line, 3));    return; }
        if (c == 'x' && line[2] == ' ') { error(stripPrefix(line, 3));   return; }
        if (c == 'v' && line[2] == ' ') { success(stripPrefix(line, 3)); return; }
        if (c == 'd' && line[2] == ' ') { danger(stripPrefix(line, 3));  return; }
        if (c == 't' && line[2] == ' ') { trace(stripPrefix(line, 3));   return; }
    }

    /* ── unordered list: - item ────────────────────────── */
    if (startsWith(line, "- ")) {
        bullet(stripPrefix(line, 2));
        return;
    }

    /* ── ordered list: N. item ─────────────────────────── */
    {
        int num = 0;
        std::string rest;
        if (isOrderedList(line, num, rest)) {
            ordered(num, rest);
            return;
        }
    }

    /* ── section: $$ title / body ──────────────────────── */
    if (startsWith(line, "$$ ")) {
        /* read next non-empty line as body from the stream */
        /* for simplicity, section stores title; body comes next parse call */
        /* Actually, we do title$body split by / */
        std::string content = stripPrefix(line, 3);
        std::size_t slash = content.find('/');
        if (slash != std::string::npos) {
            std::string title = content.substr(0, slash);
            std::string body  = stripPrefix(content, slash + 1);
            /* trim title trailing spaces */
            while (!title.empty() && title[title.size()-1] == ' ')
                title.erase(title.size()-1);
            section(title, body);
        } else {
            section(content, "");
        }
        return;
    }

    /* ── **bold** ──────────────────────────────────────── */
    if (startsWith(line, "**") && line.size() > 4
        && line[line.size()-1] == '*' && line[line.size()-2] == '*') {
        bold(line.substr(2, line.size() - 4));
        return;
    }

    /* ── *italic* ──────────────────────────────────────── */
    if (line[0] == '*' && line.size() > 2
        && line[line.size()-1] == '*'
        && line[1] != '*') {
        italic(line.substr(1, line.size() - 2));
        return;
    }

    /* ── ~~strike~~ ────────────────────────────────────── */
    if (startsWith(line, "~~") && line.size() > 4
        && line[line.size()-1] == '~' && line[line.size()-2] == '~') {
        strike(line.substr(2, line.size() - 4));
        return;
    }

    /* ── ~dim~ ─────────────────────────────────────────── */
    if (line[0] == '~' && line.size() > 2
        && line[line.size()-1] == '~'
        && line[1] != '~') {
        dim(line.substr(1, line.size() - 2));
        return;
    }

    /* ── default: plain text ───────────────────────────── */
    text(line);
}

/* ── flush accumulated callout lines ───────────────────── */
void TermWriter::_flushCallout() {
    if (!_inCallout) return;

    const TermStyle::ElemStyle* custom = getCallout(_calloutType);
    const TermStyle::ElemStyle& es = custom ? *custom : _ts.calloutStyle();

    _emit(_renderCallout(es, _calloutLabel, _calloutLines, _calloutLineCount));

    _inCallout = false;
    _calloutType = "";
    _calloutLabel = "";
    _calloutLineCount = 0;
    for (int i = 0; i < 10; ++i) _calloutLines[i] = "";
}

/* ═══════════════════════════════════════════════════════════
 *  Table parser helpers
 * ═══════════════════════════════════════════════════════════ */

/* Is this a markdown table line?  |...|  */
bool TermWriter::_isTableLine(const std::string& line) {
    if (line.empty() || line[0] != '|') return false;
    if (line[line.size() - 1] != '|')   return false;
    return true;
}

/* Is this a separator line? |---|---| or |:---:|:---|  */
bool TermWriter::_isSepLine(const std::string& line) {
    if (!_isTableLine(line)) return false;
    for (std::string::size_type i = 1; i < line.size() - 1; ++i) {
        char c = line[i];
        if (c != '-' && c != '|' && c != ':' && c != ' ') return false;
    }
    return true;
}

/* Split a | delimited line into cells, trim whitespace.
 * Returns number of cells found.
 * e.g. "| Name | Age |" → ["Name", "Age"] → returns 2 */
int TermWriter::_splitPipe(const std::string& line,
                           std::string* out, int maxCols) {
    int count = 0;
    std::string::size_type start = 0;

    /* skip leading | */
    if (!line.empty() && line[0] == '|') start = 1;

    while (start < line.size() && count < maxCols) {
        std::string::size_type pipe = line.find('|', start);
        if (pipe == std::string::npos) break;
        std::string cell = line.substr(start, pipe - start);
        /* trim */
        std::string::size_type a = cell.find_first_not_of(' ');
        std::string::size_type b = cell.find_last_not_of(' ');
        if (a != std::string::npos && b != std::string::npos)
            out[count] = cell.substr(a, b - a + 1);
        else
            out[count] = "";
        ++count;
        start = pipe + 1;
    }
    return count;
}

void TermWriter::_flushTable() {
    if (!_inTable || !_pendingTable) return;
    table(*_pendingTable);
    delete _pendingTable;
    _pendingTable = 0;
    _inTable = false;
    _tableHasHeader = false;
}
