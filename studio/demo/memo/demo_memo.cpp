/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   demo_memo.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 18:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/22 18:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** demo_memo.cpp — Text editor with undo/redo history
**
** Scenario: A minimal text editor buffer that supports undo/redo via
** the Memento pattern. Each edit pushes state to history, and the user
** can walk backwards/forwards through snapshots.
**
** Theme: Lavender Dream (soft purple-violet on dark indigo)
*/

#include <sstream>
#include <string>

#include "libcpp/core/memento.hpp"
#include "libcpp/str/format.hpp"
#include "libcpp/term/stylesheet.hpp"
#include "libcpp/term/writer.hpp"

using libcpp::Srgb;
using libcpp::StyleSheet;
using libcpp::TermStyle;
using libcpp::TermWriter;

/* ── Lavender Dream theme ────────────────────────────────────────────── */

LIBCPP_THEME_BEGIN(lavender_dream)
  RULE("h1", fg("#BB86FC").bg("#12001A").border("#BB86FC")
                 .bold().center().pad(3, 3, 1).spacing(1, 1)
                 .glyph("\xe2\x97\x86 ").sep("\xe2\x95\x90"))
  RULE("h2", fg("#9C64D6").border("#1E0033")
                 .bold().pad(1, 1).spacing(1, 0)
                 .glyph("\xe2\x96\xb8 ").sep("\xe2\x94\x81"))
  RULE("h3", fg("#CE93D8").border("#1E0033")
                 .bold().pad(1, 1).spacing(1, 0)
                 .glyph("\xe2\x97\x8f ").sep("\xe2\x95\x8c"))
  RULE("body",    fg("#D1B3E0").pad_left(2))
  RULE("quote",   fg("#9070A0").italic().glyph("\xe2\x94\x82 ").margin_left(3))
  RULE("info",    fg("#BB86FC").glyph("\xe2\x84\xb9  "))
  RULE("warn",    fg("#FFD54F").bold().glyph("\xe2\x9a\xa0  "))
  RULE("error",   fg("#CF6679").bold().glyph("\xe2\x9c\x97 "))
  RULE("success", fg("#81C784").glyph("\xe2\x9c\x94 "))
  RULE("trace",   fg("#7E57A0").glyph("   "))
  RULE("sep",     border("#1E0033").sep("\xe2\x94\x80"))
  RULE("bullet",  fg("#D1B3E0").glyph("\xe2\x96\xaa ").pad_left(4))
  RULE("ol",      fg("#D1B3E0").pad_left(4))
  CALLOUT_RULE("tip",  fg("#E1BEE7").bg("#1A0020").border("#9C64D6")
                           .glyph("\xe2\x9c\x94 "))
  CALLOUT_RULE("note", fg("#CE93D8").bg("#150020").border("#BB86FC")
                           .glyph("\xe2\x84\xb9  "))
  TABLE_RULE(border_preset(libcpp::BORDER_ROUNDED).border_color("#9C64D6")
                 .header_fg("#BB86FC").header_bg("#12001A").header_font(libcpp::FONT_BOLD)
                 .header_has_bg(true)
                 .cell_fg("#D1B3E0").cell_bg("#12001A").cell_alt_bg("#1A0025")
                 .zebra(true).cell_has_bg(true)
                 .title_fg("#BB86FC").title_bg("#12001A").title_has_bg(true)
                 .footer_fg("#9070A0"))
  WIDTH(72)
LIBCPP_THEME_END

/* ── TextBuffer — a Memento-enabled text editor ──────────────────────── */

class TextBuffer : public libcpp::core::Memento {
  friend class libcpp::core::Memento;

 public:
  TextBuffer() : _cursor(0) {}

  void set_text(const std::string& t) { _text = t; _cursor = t.size(); }
  void set_cursor(int pos) { _cursor = pos; }
  const std::string& text() const { return _text; }
  int cursor() const { return _cursor; }

  std::string display() const {
    std::string out = _text;
    if (_cursor >= 0 && _cursor <= static_cast<int>(_text.size()))
      out.insert(static_cast<std::size_t>(_cursor), "|");
    return out;
  }

 private:
  std::string _text;
  int         _cursor;

  void _saveToSnapshot(Snapshot& snap) const {
    snap << _text;
    snap << _cursor;
  }

  void _loadFromSnapshot(Snapshot& snap) {
    snap >> _text;
    snap >> _cursor;
  }
};

int main() {
  TermStyle ts;
  lavender_dream().apply(ts);
  TermWriter w(ts);

  w << "# Text Editor with Undo/Redo";
  w << "";
  w << "Demonstrating the `Memento` pattern: each edit is snapshotted";
  w << "to a `DataBuffer`, enabling unlimited undo/redo navigation.";

  // ── Creating the buffer ───────────────────────────────────────────
  w << "## Editing Session";
  w << "";

  TextBuffer buf;

  /* Edit 1: type initial text */
  buf.set_text("Hello");
  buf.pushHistory();
  w << ("1. Type \"Hello\"          -> `" + buf.display() + "`");

  /* Edit 2: append world */
  buf.set_text("Hello, World");
  buf.pushHistory();
  w << ("2. Append \", World\"     -> `" + buf.display() + "`");

  /* Edit 3: add exclamation */
  buf.set_text("Hello, World!");
  buf.pushHistory();
  w << ("3. Append \"!\"           -> `" + buf.display() + "`");

  /* Edit 4: replace World with C++ */
  buf.set_text("Hello, C++!");
  buf.set_cursor(9);
  buf.pushHistory();
  w << ("4. Replace with \"C++\"   -> `" + buf.display() + "`");

  /* Edit 5: add more text */
  buf.set_text("Hello, C++! Welcome to libcpp.");
  buf.set_cursor(29);
  buf.pushHistory();
  w << ("5. Extend sentence       -> `" + buf.display() + "`");
  w << "";

  w << ("!i History size: "
      + libcpp::str::to_string(static_cast<int>(buf.historySize()))
      + " snapshots");
  w << "";

  // ── Undo sequence ────────────────────────────────────────────────
  w << "## Undo Sequence";
  w << "";

  w << "| Action | Buffer State | History Pos |";
  w << "| :--- | :--- | :---: |";
  w << ("| Current | `" + buf.display() + "` | 5 |");

  int pos = 5;
  for (int i = 0; i < 3; ++i) {
    bool ok = buf.undo();
    --pos;
    std::string status = ok ? "OK" : "FAIL";
    w << ("| Undo (" + status + ") | `" + buf.display()
        + "` | " + libcpp::str::to_string(pos) + " |");
  }
  w << "";

  w << ("!i After 3 undos, buffer shows: `" + buf.display() + "`");
  w << "";

  // ── Redo sequence ────────────────────────────────────────────────
  w << "## Redo Sequence";
  w << "";

  w << "| Action | Buffer State |";
  w << "| :--- | :--- |";

  for (int i = 0; i < 2; ++i) {
    bool ok = buf.redo();
    std::string status = ok ? "OK" : "FAIL";
    w << ("| Redo (" + status + ") | `" + buf.display() + "` |");
  }
  w << "";

  w << ("!v After 2 redos: `" + buf.display() + "`");
  w << "";

  // ── Snapshot serialization ────────────────────────────────────────
  w << "## Snapshot Serialization";
  w << "";

  libcpp::core::Memento::Snapshot snap = buf.save();
  w << ("!i Snapshot size: "
      + libcpp::str::to_string(static_cast<int>(snap.size())) + " bytes");
  w << "The snapshot is a `DataBuffer` containing:";
  w << "- 4 bytes: string length prefix";
  w << ("- " + libcpp::str::to_string(static_cast<int>(buf.text().size()))
      + " bytes: text content");
  w << "- 4 bytes: cursor position (int)";
  w << "";

  /* Restore from snapshot into a new buffer */
  TextBuffer restored;
  restored.load(snap);
  w << ("!v Restored from snapshot: `" + restored.display() + "`");
  w << "";

  // ── Branching history ─────────────────────────────────────────────
  w << "## Branching — Edit after Undo";
  w << "> When you undo and then make a new edit, the redo history";
  w << "> is discarded — just like in a real editor.";
  w << "";

  (void)buf.undo();
  w << ("1. Undo -> `" + buf.display() + "`");
  buf.set_text("Hello, C++! Goodbye.");
  buf.pushHistory();
  w << ("2. New edit -> `" + buf.display() + "`");

  bool can_redo = buf.redo();
  w << ("3. Redo available? " + std::string(can_redo ? "yes" : "no")
      + " (branch was discarded)");
  w << "";

  // ── Summary callout ───────────────────────────────────────────────
  w << ">![tip] Memento Module Summary";
  w << "> `Memento` — abstract base with undo/redo/pushHistory";
  w << "> `DataBuffer` — polymorphic byte container for snapshots";
  w << "> Subclass implements `_saveToSnapshot` / `_loadFromSnapshot`";
  w << "> History is a vector of DataBuffer states — zero-copy save/load";
  w << "";

  w << "---";
  w << "!v Editor session complete — 5 edits, 3 undos, 2 redos.";
  return 0;
}
