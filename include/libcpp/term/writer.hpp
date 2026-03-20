/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   writer.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/22 10:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_TERM_WRITER_HPP
# define LIBCPP_TERM_WRITER_HPP

# include <string>
# include <sstream>
# include <fstream>
# include <iostream>
# include <map>
# include "libcpp/term/style.hpp"
# include "libcpp/term/table.hpp"

namespace libcpp
{

/*
** TermWriter -- buffered markdown-to-terminal renderer
**
**  Two modes of operation:
**
**  1) IMPERATIVE -- brick-by-brick
**     TermWriter w(ts);
**     w.h1("Title").text("paragraph").bullet("item").flush();
**
**  2) DECLARATIVE -- markdown string parsed line-by-line
**     w << "# Title"
**       << "paragraph"
**       << "- item"
**       << "---";
**     w.flush();
**
**  Markdown syntax supported by the parser:
**     #        h1 heading
**     ##       h2 heading
**     ###      h3 heading
**     >        blockquote
**     >![T]    callout with type T (open block)
**     > text   callout body line (while block open)
**     ---      separator
**     ===      heavy rule (hr)
**     - item   unordered list item (bullet)
**     N. item  ordered list item
**     !! text  warning
**     !x text  error
**     !v text  success
**     !i text  info
**     !d text  danger
**     !t text  trace
**     **t**    bold
**     *t*      italic
**     ~~t~~    strikethrough
**     ~t~      dim
**     __t__    underline
**     `t`      inline code
**     $$ t/b   section (title / body)
**     | a | b  table row (pipe-delimited)
**     |:--|:--:|--:|  table alignment (left, center, right)
**     (blank)  newline
**     (other)  plain text
**
** Orthodox Canonical Form compliant.
*/
class TermWriter
{
public:
static const int MAX_CALLOUTS = 16;

/* -- OCF --------------------------------------------------- */
TermWriter();
explicit TermWriter(TermStyle& ts);
TermWriter(TermStyle& ts, std::ostream& os);
TermWriter(const TermWriter& other);
TermWriter& operator=(const TermWriter& other);
~TermWriter();

/* -- callout preset registry ------------------------------- */
void define_callout(const std::string& name, const ElemStyle& style);
ElemStyle* get_callout(const std::string& name);
const ElemStyle* get_callout(const std::string& name) const;

/* -- imperative API (all return *this for chaining) -------- */
TermWriter& h1(const std::string& title);
TermWriter& h2(const std::string& title);
TermWriter& h3(const std::string& title);
TermWriter& text(const std::string& msg);
TermWriter& bold(const std::string& msg);
TermWriter& italic(const std::string& msg);
TermWriter& dim(const std::string& msg);
TermWriter& underline(const std::string& msg);
TermWriter& strike(const std::string& msg);
TermWriter& code(const std::string& msg);
TermWriter& quote(const std::string& msg);
TermWriter& info(const std::string& msg);
TermWriter& warn(const std::string& msg);
TermWriter& error(const std::string& msg);
TermWriter& success(const std::string& msg);
TermWriter& danger(const std::string& msg);
TermWriter& trace(const std::string& msg);
TermWriter& bullet(const std::string& msg);
TermWriter& ordered(int num, const std::string& msg);
TermWriter& sep();
TermWriter& hr();
TermWriter& nl();
TermWriter& section(const std::string& title, const std::string& body);

/* -- named callout ----------------------------------------- */
TermWriter& callout(const std::string& type,
const std::string& l1,
const std::string& l2 = "",
const std::string& l3 = "",
const std::string& l4 = "",
const std::string& l5 = "",
const std::string& l6 = "",
const std::string& l7 = "",
const std::string& l8 = "",
const std::string& l9 = "",
const std::string& l10 = "");

/* -- table (imperative) ------------------------------------ */
TermWriter& table(const Table& t);

/* -- table style access ------------------------------------ */
TableStyle& table_style();
const TableStyle& table_style() const;

/* -- markdown parser --------------------------------------- */
TermWriter& parse(const std::string& markdown);
TermWriter& parse_file(const std::string& path);

/* -- stream-style line feeder (operator<<) ----------------- */
TermWriter& operator<<(const std::string& line);
TermWriter& operator<<(const char* line);

/* -- simple direct-write API (non-buffered) ---------------- */
void write(const std::string& markdown);
void writeln(const std::string& markdown);
void write_raw(const std::string& txt);

/* -- output ------------------------------------------------ */
std::string str() const;
void flush();
void clear();

/* -- style access ------------------------------------------ */
TermStyle& style();
const TermStyle& style() const;

private:
TermStyle*                      _ts;
std::ostream*                   _os;
std::ostringstream              _buf;
bool                            _owns_ts;

/* callout registry */
struct CalloutEntry
{
std::string name;
ElemStyle   es;
bool        used;
CalloutEntry() : used(false) {}
};
CalloutEntry _callouts[MAX_CALLOUTS];
int          _callout_count;

/* parser state -- callout block */
bool        _in_callout;
std::string _callout_type;
std::string _callout_label;
std::string _callout_lines[10];
int         _callout_line_count;

/* parser state -- table block */
bool        _in_table;
Table*      _pending_table;
bool        _table_has_header;
TableStyle  _table_style;

/* helpers */
void _emit(const std::string& s);
void _parse_line(const std::string& line);
void _flush_callout();
void _flush_table();
void _register_default_callouts();

std::string _render_callout(const ElemStyle& es,
const std::string& label,
const std::string* lines,
int count) const;

static bool _is_table_line(const std::string& line);
static bool _is_sep_line(const std::string& line);
static int  _split_pipe(const std::string& line,
std::string* out, int max_cols);
static bool _starts_with(const std::string& s, const std::string& p);
static std::string _strip_prefix(const std::string& s, std::size_t n);
static std::string _ltrim(const std::string& s);
static bool _is_repeated(const std::string& s, char c);
static bool _is_ordered(const std::string& s, int& num, std::string& rest);
};

} /* namespace libcpp */

#endif /* LIBCPP_TERM_WRITER_HPP */
