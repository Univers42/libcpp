/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   writer.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/22 10:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/term/writer.hpp"

namespace libcpp {

/* ======================================================================
 *  OCF
 * ====================================================================== */

TermWriter::TermWriter()
    : _ts(new TermStyle()),
      _os(&std::cout),
      _owns_ts(true),
      _callout_count(0),
      _in_callout(false),
      _callout_line_count(0),
      _in_table(false),
      _pending_table(0),
      _table_has_header(false) {
  _register_default_callouts();
}

TermWriter::TermWriter(TermStyle& ts)
    : _ts(&ts),
      _os(&std::cout),
      _owns_ts(false),
      _callout_count(0),
      _in_callout(false),
      _callout_line_count(0),
      _in_table(false),
      _pending_table(0),
      _table_has_header(false) {
  _register_default_callouts();
}

TermWriter::TermWriter(TermStyle& ts, std::ostream& os)
    : _ts(&ts),
      _os(&os),
      _owns_ts(false),
      _callout_count(0),
      _in_callout(false),
      _callout_line_count(0),
      _in_table(false),
      _pending_table(0),
      _table_has_header(false) {
  _register_default_callouts();
}

TermWriter::TermWriter(const TermWriter& o)
    : _ts(0),
      _os(o._os),
      _owns_ts(o._owns_ts),
      _callout_count(o._callout_count),
      _in_callout(false),
      _callout_line_count(0),
      _in_table(false),
      _pending_table(0),
      _table_has_header(false),
      _table_style(o._table_style) {
  if (_owns_ts && o._ts)
    _ts = new TermStyle(*o._ts);
  else
    _ts = o._ts;
  for (int i = 0; i < _callout_count; ++i) _callouts[i] = o._callouts[i];
}

TermWriter& TermWriter::operator=(const TermWriter& o) {
  if (this != &o) {
    if (_owns_ts) delete _ts;
    delete _pending_table;
    _os = o._os;
    _owns_ts = o._owns_ts;
    if (_owns_ts && o._ts)
      _ts = new TermStyle(*o._ts);
    else
      _ts = o._ts;
    _callout_count = o._callout_count;
    for (int i = 0; i < _callout_count; ++i) _callouts[i] = o._callouts[i];
    _in_callout = false;
    _callout_line_count = 0;
    _in_table = false;
    _pending_table = 0;
    _table_has_header = false;
    _table_style = o._table_style;
    _buf.str("");
    _buf.clear();
  }
  return *this;
}

TermWriter::~TermWriter() {
  if (_owns_ts) delete _ts;
  delete _pending_table;
}

/* ======================================================================
 *  Callout registry
 * ====================================================================== */

void TermWriter::define_callout(const std::string& name,
                                const ElemStyle& style) {
  for (int i = 0; i < _callout_count; ++i) {
    if (_callouts[i].name == name) {
      _callouts[i].es = style;
      return;
    }
  }
  if (_callout_count < MAX_CALLOUTS) {
    _callouts[_callout_count].name = name;
    _callouts[_callout_count].es = style;
    _callouts[_callout_count].used = true;
    ++_callout_count;
  }
}

ElemStyle* TermWriter::get_callout(const std::string& name) {
  for (int i = 0; i < _callout_count; ++i)
    if (_callouts[i].name == name) return &_callouts[i].es;
  return 0;
}

const ElemStyle* TermWriter::get_callout(const std::string& name) const {
  for (int i = 0; i < _callout_count; ++i)
    if (_callouts[i].name == name) return &_callouts[i].es;
  return 0;
}

/* ======================================================================
 *  Internal helpers
 * ====================================================================== */

void TermWriter::_emit(const std::string& s) {
  *_os << s << "\n";
  _buf << s << "\n";
}

void TermWriter::_register_default_callouts() {
  /* tip — green tint */
  {
    ElemStyle e;
    e.fg = Srgb(160, 230, 160);
    e.bg = Srgb(12, 30, 18);
    e.border = Srgb(60, 200, 100);
    e.glyph = Glyph::CHECK;
    e.has_bg = true;
    e.font = FONT_NONE;
    e.width = 60;
    e.pad_l = 1;
    e.pad_r = 1;
    e.margin_l = 1;
    define_callout("tip", e);
  }
  /* note — blue tint */
  {
    ElemStyle e;
    e.fg = Srgb(170, 200, 240);
    e.bg = Srgb(12, 18, 35);
    e.border = Srgb(70, 130, 230);
    e.glyph = Glyph::INFO;
    e.has_bg = true;
    e.font = FONT_NONE;
    e.width = 60;
    e.pad_l = 1;
    e.pad_r = 1;
    e.margin_l = 1;
    define_callout("note", e);
  }
  /* warning — amber tint */
  {
    ElemStyle e;
    e.fg = Srgb(240, 210, 140);
    e.bg = Srgb(35, 28, 10);
    e.border = Srgb(230, 170, 40);
    e.glyph = Glyph::WARN;
    e.has_bg = true;
    e.font = FONT_NONE;
    e.width = 60;
    e.pad_l = 1;
    e.pad_r = 1;
    e.margin_l = 1;
    define_callout("warning", e);
  }
  /* danger — red tint */
  {
    ElemStyle e;
    e.fg = Srgb(240, 170, 160);
    e.bg = Srgb(38, 12, 12);
    e.border = Srgb(220, 60, 60);
    e.glyph = Glyph::SKULL;
    e.has_bg = true;
    e.font = FONT_NONE;
    e.width = 60;
    e.pad_l = 1;
    e.pad_r = 1;
    e.margin_l = 1;
    define_callout("danger", e);
  }
  /* important — purple tint */
  {
    ElemStyle e;
    e.fg = Srgb(200, 180, 240);
    e.bg = Srgb(25, 15, 38);
    e.border = Srgb(160, 100, 230);
    e.glyph = Glyph::DIAMOND;
    e.has_bg = true;
    e.font = FONT_NONE;
    e.width = 60;
    e.pad_l = 1;
    e.pad_r = 1;
    e.margin_l = 1;
    define_callout("important", e);
  }
  /* success — bright green tint */
  {
    ElemStyle e;
    e.fg = Srgb(140, 240, 170);
    e.bg = Srgb(10, 32, 15);
    e.border = Srgb(40, 210, 100);
    e.glyph = Glyph::CHECK;
    e.has_bg = true;
    e.font = FONT_NONE;
    e.width = 60;
    e.pad_l = 1;
    e.pad_r = 1;
    e.margin_l = 1;
    define_callout("success", e);
  }
  /* error — bright red tint */
  {
    ElemStyle e;
    e.fg = Srgb(255, 160, 150);
    e.bg = Srgb(40, 10, 10);
    e.border = Srgb(255, 70, 60);
    e.glyph = Glyph::CROSS;
    e.has_bg = true;
    e.font = FONT_NONE;
    e.width = 60;
    e.pad_l = 1;
    e.pad_r = 1;
    e.margin_l = 1;
    define_callout("error", e);
  }
  /* info — cyan tint */
  {
    ElemStyle e;
    e.fg = Srgb(160, 220, 240);
    e.bg = Srgb(10, 25, 35);
    e.border = Srgb(50, 180, 220);
    e.glyph = Glyph::INFO;
    e.has_bg = true;
    e.font = FONT_NONE;
    e.width = 60;
    e.pad_l = 1;
    e.pad_r = 1;
    e.margin_l = 1;
    define_callout("info", e);
  }
}

std::string TermWriter::_render_callout(const ElemStyle& es,
                                        const std::string& label,
                                        const std::string* lines,
                                        int count) const {
  /*
  ** Obsidian-style callout with UNIFORM background width:
  **
  **   ┃  ⚠  Title                          ┃
  **   ┃     Body line 1                     ┃
  **   ┃     Body line 2                     ┃
  **
  ** PHASE 1: Buffer all content lines and measure the widest one.
  ** PHASE 2: Render all lines padded to uniform width for clean bg.
  */

  std::string rst = TermUtils::reset();
  std::string sp = TermUtils::spaces(es.margin_l);
  int gw = TermUtils::vis_len(es.glyph);

  /* ── Phase 1: measure max content width ─────────────────────────── */
  int header_vis = gw + TermUtils::vis_len(label);
  int max_content = header_vis;
  for (int i = 0; i < count; ++i) {
    if (lines[i].empty()) continue;
    int lw = gw + TermUtils::vis_len(lines[i]);
    if (lw > max_content) max_content = lw;
  }

  /* bar_vis = " ┃ " = 3 visible chars */
  int bar_vis = 3;
  int min_w = es.width > 0 ? es.width : 60;
  int content_w = max_content;
  if (content_w + bar_vis + 1 < min_w) content_w = min_w - bar_vis - 1;
  int total_w = bar_vis + content_w + 1; /* +1 for trailing pad */

  /* ── Phase 2: render with uniform padding ───────────────────────── */
  std::string r;
  r += TermUtils::newlines(es.space_before);

  /* top blank line with bg */
  if (es.has_bg) {
    r += sp;
    r += TermUtils::apply_bg(es.bg);
    r += TermUtils::apply_fg(es.border);
    r += std::string(" \xe2\x94\x83") + TermUtils::spaces(total_w - 2) + rst +
         "\n";
  }

  /* header line: bar + glyph + label */
  {
    r += sp;
    if (es.has_bg) r += TermUtils::apply_bg(es.bg);
    r += TermUtils::apply_fg(es.border);
    r += std::string(" \xe2\x94\x83 ");
    r += TermUtils::apply_font(FONT_BOLD);
    r += es.glyph + label;
    int pad = content_w - header_vis;
    if (pad > 0) r += TermUtils::spaces(pad);
    r += " " + rst + "\n";
  }

  /* body lines */
  for (int i = 0; i < count; ++i) {
    if (lines[i].empty()) continue;
    r += sp;
    if (es.has_bg) r += TermUtils::apply_bg(es.bg);
    r += TermUtils::apply_fg(es.border);
    r += std::string(" \xe2\x94\x83 ");
    r += TermUtils::apply_fg(es.fg);
    r += TermUtils::apply_font(es.font);
    r += TermUtils::spaces(gw);
    r += lines[i];
    int used = gw + TermUtils::vis_len(lines[i]);
    int pad = content_w - used;
    if (pad > 0) r += TermUtils::spaces(pad);
    r += " " + rst + "\n";
  }

  /* bottom blank line with bg */
  if (es.has_bg) {
    r += sp;
    r += TermUtils::apply_bg(es.bg);
    r += TermUtils::apply_fg(es.border);
    r += std::string(" \xe2\x94\x83") + TermUtils::spaces(total_w - 2) + rst +
         "\n";
  }

  r += TermUtils::newlines(es.space_after);
  return r;
}

/* ======================================================================
 *  Imperative API
 * ====================================================================== */

TermWriter& TermWriter::h1(const std::string& t) {
  _emit(_ts->h1(t));
  return *this;
}
TermWriter& TermWriter::h2(const std::string& t) {
  _emit(_ts->h2(t));
  return *this;
}
TermWriter& TermWriter::h3(const std::string& t) {
  _emit(_ts->h3(t));
  return *this;
}
TermWriter& TermWriter::text(const std::string& m) {
  _emit(_ts->text(m));
  return *this;
}
TermWriter& TermWriter::bold(const std::string& m) {
  _emit(_ts->bold(m));
  return *this;
}
TermWriter& TermWriter::italic(const std::string& m) {
  _emit(_ts->italic(m));
  return *this;
}
TermWriter& TermWriter::dim(const std::string& m) {
  _emit(_ts->dim(m));
  return *this;
}
TermWriter& TermWriter::underline(const std::string& m) {
  _emit(_ts->underline(m));
  return *this;
}
TermWriter& TermWriter::strike(const std::string& m) {
  _emit(_ts->strike(m));
  return *this;
}
TermWriter& TermWriter::code(const std::string& m) {
  _emit(_ts->code(m));
  return *this;
}
TermWriter& TermWriter::quote(const std::string& m) {
  _emit(_ts->quote(m));
  return *this;
}
TermWriter& TermWriter::info(const std::string& m) {
  _emit(_ts->info(m));
  return *this;
}
TermWriter& TermWriter::warn(const std::string& m) {
  _emit(_ts->warn(m));
  return *this;
}
TermWriter& TermWriter::error(const std::string& m) {
  _emit(_ts->error(m));
  return *this;
}
TermWriter& TermWriter::success(const std::string& m) {
  _emit(_ts->success(m));
  return *this;
}
TermWriter& TermWriter::danger(const std::string& m) {
  _emit(_ts->danger(m));
  return *this;
}
TermWriter& TermWriter::trace(const std::string& m) {
  _emit(_ts->trace(m));
  return *this;
}
TermWriter& TermWriter::bullet(const std::string& m) {
  _emit(_ts->bullet(m));
  return *this;
}
TermWriter& TermWriter::ordered(int n, const std::string& m) {
  _emit(_ts->ordered(n, m));
  return *this;
}
TermWriter& TermWriter::sep() {
  _emit(_ts->separator());
  return *this;
}
TermWriter& TermWriter::hr() {
  _emit(_ts->hr());
  return *this;
}
TermWriter& TermWriter::nl() {
  *_os << "\n";
  _buf << "\n";
  return *this;
}

TermWriter& TermWriter::section(const std::string& t, const std::string& b) {
  _emit(_ts->section(t, b));
  return *this;
}

/* -- named callout ---------------------------------------------------- */
TermWriter& TermWriter::callout(const std::string& type, const std::string& l1,
                                const std::string& l2, const std::string& l3,
                                const std::string& l4, const std::string& l5,
                                const std::string& l6, const std::string& l7,
                                const std::string& l8, const std::string& l9,
                                const std::string& l10) {
  const std::string arr[10] = {l1, l2, l3, l4, l5, l6, l7, l8, l9, l10};
  int count = 0;
  for (int i = 0; i < 10; ++i)
    if (!arr[i].empty()) count = i + 1;

  const ElemStyle* custom = get_callout(type);
  if (custom)
    _emit(_render_callout(*custom, type, arr, count));
  else
    _emit(_render_callout(_ts->callout_style, type, arr, count));
  return *this;
}

/* -- table(imperative) ----------------------------------------------- */
TermWriter& TermWriter::table(const Table& t) {
  /* if the table has no explicit style, apply the writer's themed style */
  Table themed(t);
  if (!themed.has_custom_style()) themed.set_style(_table_style);
  std::string rendered = themed.render();
  if (!rendered.empty() && rendered[rendered.size() - 1] == '\n')
    rendered.erase(rendered.size() - 1);
  _emit(rendered);
  return *this;
}

TableStyle& TermWriter::table_style() { return _table_style; }
const TableStyle& TermWriter::table_style() const { return _table_style; }

/* ======================================================================
 *  Output
 * ====================================================================== */

std::string TermWriter::str() const { return _buf.str(); }

void TermWriter::flush() {
  if (_in_callout) _flush_callout();
  if (_in_table) _flush_table();
  _os->flush();
  _buf.str("");
  _buf.clear();
}

void TermWriter::clear() {
  _buf.str("");
  _buf.clear();
}

TermStyle& TermWriter::style() { return *_ts; }
const TermStyle& TermWriter::style() const { return *_ts; }

/* ======================================================================
 *  Simple direct-write API(non-buffered, backward compat)
 * ====================================================================== */

void TermWriter::write(const std::string& markdown) {
  std::istringstream iss(markdown);
  std::string line;
  while (std::getline(iss, line)) _parse_line(line);
  if (_in_callout) _flush_callout();
  if (_in_table) _flush_table();
}

void TermWriter::writeln(const std::string& markdown) {
  write(markdown);
  *_os << "\n";
}

void TermWriter::write_raw(const std::string& txt) { *_os << txt; }

/* ======================================================================
 *  Markdown parser
 * ====================================================================== */

TermWriter& TermWriter::parse(const std::string& markdown) {
  std::istringstream iss(markdown);
  std::string line;
  while (std::getline(iss, line)) _parse_line(line);
  if (_in_callout) _flush_callout();
  if (_in_table) _flush_table();
  return *this;
}

TermWriter& TermWriter::parse_file(const std::string& path) {
  std::ifstream ifs(path.c_str());
  if (!ifs.is_open()) {
    error("parse_file: cannot open \"" + path + "\"");
    return *this;
  }
  std::string line;
  while (std::getline(ifs, line)) _parse_line(line);
  if (_in_callout) _flush_callout();
  if (_in_table) _flush_table();
  return *this;
}

/* ======================================================================
 *  operator<< -- stream-style line feeder
 * ====================================================================== */

TermWriter& TermWriter::operator<<(const std::string& line) {
  if (line.find('\n') != std::string::npos) {
    std::istringstream iss(line);
    std::string sub;
    while (std::getline(iss, sub)) _parse_line(sub);
  } else
    _parse_line(line);
  return *this;
}

TermWriter& TermWriter::operator<<(const char* line) {
  return *this << std::string(line);
}

/* ======================================================================
 *  Static helpers
 * ====================================================================== */

bool TermWriter::_starts_with(const std::string& s, const std::string& p) {
  if (p.size() > s.size()) return false;
  return s.compare(0, p.size(), p) == 0;
}

std::string TermWriter::_strip_prefix(const std::string& s, std::size_t n) {
  std::size_t i = n;
  while (i < s.size() && s[i] == ' ') ++i;
  return s.substr(i);
}

std::string TermWriter::_ltrim(const std::string& s) {
  std::size_t i = 0;
  while (i < s.size() && s[i] == ' ') ++i;
  return s.substr(i);
}

bool TermWriter::_is_repeated(const std::string& s, char c) {
  if (s.empty()) return false;
  for (std::size_t i = 0; i < s.size(); ++i)
    if (s[i] != c) return false;
  return true;
}

bool TermWriter::_is_ordered(const std::string& s, int& num,
                             std::string& rest) {
  std::size_t i = 0;
  while (i < s.size() && s[i] >= '0' && s[i] <= '9') ++i;
  if (i == 0 || i >= s.size()) return false;
  if (s[i] != '.') return false;
  std::istringstream iss(s.substr(0, i));
  iss >> num;
  rest = _strip_prefix(s, i + 1);
  return true;
}

/* ======================================================================
 *  Table helpers
 * ====================================================================== */

bool TermWriter::_is_table_line(const std::string& line) {
  if (line.empty() || line[0] != '|') return false;
  if (line[line.size() - 1] != '|') return false;
  return true;
}

bool TermWriter::_is_sep_line(const std::string& line) {
  if (!_is_table_line(line)) return false;
  for (std::string::size_type i = 1; i < line.size() - 1; ++i) {
    char c = line[i];
    if (c != '-' && c != '|' && c != ':' && c != ' ') return false;
  }
  return true;
}

int TermWriter::_split_pipe(const std::string& line, std::string* out,
                            int max_cols) {
  int count = 0;
  std::string::size_type start = 0;
  if (!line.empty() && line[0] == '|') start = 1;
  while (start < line.size() && count < max_cols) {
    std::string::size_type pipe = line.find('|', start);
    if (pipe == std::string::npos) break;
    std::string cell = line.substr(start, pipe - start);
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

/* ======================================================================
 *  _parse_line -- the core markdown line dispatcher
 * ====================================================================== */

void TermWriter::_parse_line(const std::string& raw) {
  if (!_ts) return;
  std::string line = _ltrim(raw);

  /* inside a callout block? */
  if (_in_callout) {
    if (_starts_with(line, "> ")) {
      if (_callout_line_count < 10)
        _callout_lines[_callout_line_count++] = _strip_prefix(line, 2);
      return;
    }
    _flush_callout();
    if (line == ">") return;
  }

  /* inside a table? */
  if (_in_table) {
    if (_is_table_line(line)) {
      if (_is_sep_line(line)) {
        _table_has_header = true;
        /* parse column alignment from :---: / :--- / ---: */
        if (_pending_table) {
          std::string cells[8];
          int n = _split_pipe(line, cells, 8);
          for (int i = 0; i < n; ++i) {
            std::string c = cells[i];
            /* strip spaces from the alignment cell */
            std::string::size_type a = c.find_first_not_of(' ');
            std::string::size_type b = c.find_last_not_of(' ');
            if (a == std::string::npos) continue;
            std::string stripped = c.substr(a, b - a + 1);
            bool left_colon = (!stripped.empty() && stripped[0] == ':');
            bool right_colon =
                (!stripped.empty() && stripped[stripped.size() - 1] == ':');
            if (left_colon && right_colon)
              _pending_table->set_col_align(i, ALIGN_CENTER);
            else if (right_colon)
              _pending_table->set_col_align(i, ALIGN_RIGHT);
            /* else left or default: ALIGN_LEFT(default) */
          }
        }
      } else {
        std::string cells[8];
        int n = _split_pipe(line, cells, _pending_table ? 8 : 8);
        if (_pending_table) {
          if (!_table_has_header) {
            std::string h[8];
            for (int i = 0; i < n; ++i) h[i] = cells[i];
            _pending_table->header(h, n);
          } else {
            std::string r[8];
            for (int i = 0; i < n; ++i) r[i] = cells[i];
            _pending_table->row(r, n);
          }
        }
      }
      return;
    }
    _flush_table();
  }

  /* table start: | ... | */
  if (_is_table_line(line) && !_is_sep_line(line)) {
    std::string cells[8];
    int n = _split_pipe(line, cells, 8);
    if (n > 0) {
      delete _pending_table;
      _pending_table = new Table(n);
      _pending_table->set_style(_table_style);
      _in_table = true;
      _table_has_header = false;
      std::string h[8];
      for (int i = 0; i < n; ++i) h[i] = cells[i];
      _pending_table->header(h, n);
      return;
    }
  }

  /* blank line */
  if (line.empty()) {
    nl();
    return;
  }

  /* separator: --- */
  if (line.size() >= 3 && _is_repeated(line, '-')) {
    sep();
    return;
  }

  /* heavy rule: === */
  if (line.size() >= 3 && _is_repeated(line, '=')) {
    hr();
    return;
  }

  /* headings: # ## ### */
  if (_starts_with(line, "### ")) {
    h3(_strip_prefix(line, 4));
    return;
  }
  if (_starts_with(line, "## ")) {
    h2(_strip_prefix(line, 3));
    return;
  }
  if (_starts_with(line, "# ")) {
    h1(_strip_prefix(line, 2));
    return;
  }

  /* callout open: >![type] label */
  if (_starts_with(line, ">![")) {
    std::size_t close = line.find(']', 3);
    if (close != std::string::npos) {
      _in_callout = true;
      _callout_type = line.substr(3, close - 3);
      _callout_label = _strip_prefix(line, close + 1);
      if (_callout_label.empty()) _callout_label = _callout_type;
      _callout_line_count = 0;
      for (int i = 0; i < 10; ++i) _callout_lines[i] = "";
      return;
    }
  }

  /* blockquote: > text */
  if (_starts_with(line, "> ")) {
    quote(_strip_prefix(line, 2));
    return;
  }

  /* log levels: !i !w !x !v !d !t !! */
  if (line.size() >= 3 && line[0] == '!') {
    char c = line[1];
    if (c == 'i' && line[2] == ' ') {
      info(_strip_prefix(line, 3));
      return;
    }
    if (c == 'w' && line[2] == ' ') {
      warn(_strip_prefix(line, 3));
      return;
    }
    if (c == '!' && line[2] == ' ') {
      warn(_strip_prefix(line, 3));
      return;
    }
    if (c == 'x' && line[2] == ' ') {
      error(_strip_prefix(line, 3));
      return;
    }
    if (c == 'v' && line[2] == ' ') {
      success(_strip_prefix(line, 3));
      return;
    }
    if (c == 'd' && line[2] == ' ') {
      danger(_strip_prefix(line, 3));
      return;
    }
    if (c == 't' && line[2] == ' ') {
      trace(_strip_prefix(line, 3));
      return;
    }
  }

  /* log levels: [info] [warn] [error] [success] [danger] [trace] */
  if (_starts_with(line, "[info] ")) {
    info(_strip_prefix(line, 7));
    return;
  }
  if (_starts_with(line, "[warn] ")) {
    warn(_strip_prefix(line, 7));
    return;
  }
  if (_starts_with(line, "[error] ")) {
    error(_strip_prefix(line, 8));
    return;
  }
  if (_starts_with(line, "[success] ")) {
    success(_strip_prefix(line, 10));
    return;
  }
  if (_starts_with(line, "[danger] ")) {
    danger(_strip_prefix(line, 9));
    return;
  }
  if (_starts_with(line, "[trace] ")) {
    trace(_strip_prefix(line, 8));
    return;
  }

  /* callout: [!name] body */
  if (_starts_with(line, "[!") && line.find(']') != std::string::npos) {
    std::size_t end = line.find(']');
    std::string name = line.substr(2, end - 2);
    std::string body = _strip_prefix(line, end + 1);
    const ElemStyle* custom = get_callout(name);
    if (custom) {
      _emit(TermUtils::apply_fg(custom->fg) +
            TermUtils::apply_font(custom->font) + custom->glyph + body +
            TermUtils::reset());
    } else {
      _emit(TermUtils::apply_fg(_ts->callout_style.fg) +
            _ts->callout_style.glyph + body + TermUtils::reset());
    }
    return;
  }

  /* unordered list: - item */
  if (_starts_with(line, "- ")) {
    bullet(_strip_prefix(line, 2));
    return;
  }

  /* ordered list: N. item */
  {
    int num = 0;
    std::string rest;
    if (_is_ordered(line, num, rest)) {
      ordered(num, rest);
      return;
    }
  }

  /* section: $$ title / body */
  if (_starts_with(line, "$$ ")) {
    std::string content = _strip_prefix(line, 3);
    std::size_t slash = content.find('/');
    if (slash != std::string::npos) {
      std::string stitle = content.substr(0, slash);
      std::string sbody = _strip_prefix(content, slash + 1);
      while (!stitle.empty() && stitle[stitle.size() - 1] == ' ')
        stitle.erase(stitle.size() - 1);
      section(stitle, sbody);
    } else
      section(content, "");
    return;
  }

  /* `code` */
  if (line[0] == '`' && line.size() > 2 && line[line.size() - 1] == '`' &&
      line[1] != '`') {
    code(line.substr(1, line.size() - 2));
    return;
  }

  /* __underline__ */
  if (_starts_with(line, "__") && line.size() > 4 &&
      line[line.size() - 1] == '_' && line[line.size() - 2] == '_') {
    underline(line.substr(2, line.size() - 4));
    return;
  }

  /* **bold** */
  if (_starts_with(line, "**") && line.size() > 4 &&
      line[line.size() - 1] == '*' && line[line.size() - 2] == '*') {
    bold(line.substr(2, line.size() - 4));
    return;
  }

  /* *italic* */
  if (line[0] == '*' && line.size() > 2 && line[line.size() - 1] == '*' &&
      line[1] != '*') {
    italic(line.substr(1, line.size() - 2));
    return;
  }

  /* ~~strike~~ */
  if (_starts_with(line, "~~") && line.size() > 4 &&
      line[line.size() - 1] == '~' && line[line.size() - 2] == '~') {
    strike(line.substr(2, line.size() - 4));
    return;
  }

  /* ~dim~ */
  if (line[0] == '~' && line.size() > 2 && line[line.size() - 1] == '~' &&
      line[1] != '~') {
    dim(line.substr(1, line.size() - 2));
    return;
  }

  /* default: plain text */
  text(line);
}

/* -- flush callout ---------------------------------------------------- */
void TermWriter::_flush_callout() {
  if (!_in_callout) return;
  const ElemStyle* custom = get_callout(_callout_type);
  const ElemStyle& es = custom ? *custom : _ts->callout_style;
  _emit(
      _render_callout(es, _callout_label, _callout_lines, _callout_line_count));
  _in_callout = false;
  _callout_type = "";
  _callout_label = "";
  _callout_line_count = 0;
  for (int i = 0; i < 10; ++i) _callout_lines[i] = "";
}

/* -- flush table ------------------------------------------------------ */
void TermWriter::_flush_table() {
  if (!_in_table || !_pending_table) return;
  table(*_pending_table);
  delete _pending_table;
  _pending_table = 0;
  _in_table = false;
  _table_has_header = false;
}

} /* namespace libcpp */
