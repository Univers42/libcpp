#ifndef TERMWRITER_HPP
# define TERMWRITER_HPP

# include <string>
# include <sstream>
# include <fstream>
# include <iostream>
# include "TermStyle.hpp"
# include "TermTable.hpp"

/*
 *  TermWriter — buffered markdown-to-terminal renderer
 *
 *  ┌────────────────────────────────────────────────────────┐
 *  │  Two modes of operation:                                │
 *  │                                                         │
 *  │  1) IMPERATIVE — brick-by-brick                         │
 *  │     TermWriter w(ts);                                   │
 *  │     w.h1("Title");                                      │
 *  │     w.text("paragraph");                                │
 *  │     w.callout("info", "line1", "line2");                │
 *  │     w.flush();                                          │
 *  │                                                         │
 *  │  2) DECLARATIVE — markdown string parsed line-by-line   │
 *  │     w.parse("# Title\nparagraph\n>![info]\n> line");    │
 *  │     w.flush();                                          │
 *  │                                                         │
 *  │  Markdown syntax supported by parse():                  │
 *  │     #       h1 heading                                  │
 *  │     ##      h2 heading                                  │
 *  │     ###     h3 heading                                  │
 *  │     >       blockquote                                  │
 *  │     >![T]   callout with type T (open block)            │
 *  │     > text  callout body line (while block open)        │
 *  │     >       empty close → closes callout block          │
 *  │     ---     separator                                   │
 *  │     ===     heavy rule (hr)                             │
 *  │     - item  unordered list item (bullet)                │
 *  │     N. item ordered list item                           │
 *  │     !! text warning                                     │
 *  │     !x text error                                       │
 *  │     !v text success                                     │
 *  │     !i text info                                        │
 *  │     $$      section start: next two lines = title+body  │
 *  │     (blank) newline                                     │
 *  │     (other) plain text                                  │
 *  └────────────────────────────────────────────────────────┘
 *
 *  Custom callout styles:
 *     w.defineCallout("danger", style);
 *     w.callout("danger", "line1", "line2");
 *
 *  Everything is buffered.  Call flush() or str() to get output.
 */

class TermWriter {
    public:
        /* ── max user-defined callout presets ───────────── */
        static const int MAX_CALLOUTS = 16;

        TermWriter(TermStyle& ts);
        TermWriter(TermStyle& ts, std::ostream& os);
        ~TermWriter();

        /* ── callout preset registry ───────────────────── */
        void defineCallout(const std::string& name,
                           const TermStyle::ElemStyle& style);
        TermStyle::ElemStyle* getCallout(const std::string& name);
        const TermStyle::ElemStyle* getCallout(const std::string& name) const;

        /* ── imperative API (brick-by-brick) ───────────── */
        TermWriter& h1(const std::string& title);
        TermWriter& h2(const std::string& title);
        TermWriter& h3(const std::string& title);
        TermWriter& text(const std::string& msg);
        TermWriter& bold(const std::string& msg);
        TermWriter& italic(const std::string& msg);
        TermWriter& dim(const std::string& msg);
        TermWriter& underline(const std::string& msg);
        TermWriter& strike(const std::string& msg);
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
        TermWriter& section(const std::string& title,
                            const std::string& body);

        /* ── named callout (uses registered preset or default) */
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

        /* ── table (imperative — pass a fully built table) */
        TermWriter& table(const TermTable& t);

        /* ── table style access (for markdown-parsed tables) */
        TermTable::Style& tableStyle();
        const TermTable::Style& tableStyle() const;

        /* ── markdown parser ───────────────────────────── */
        TermWriter& parse(const std::string& markdown);
        TermWriter& parseFile(const std::string& path);

        /* ── stream-style line feeder ──────────────────── *
         *                                                   *
         *  Each << feeds ONE line through the parser.       *
         *  No \n needed — just write naturally:             *
         *                                                   *
         *    w << "# " + dog.getType()                      *
         *      << "> Sound: " + dog.makeSound()             *
         *      << "- Idea: " + brain.getIdea(0)             *
         *      << "---";                                    *
         *    w.flush();                                     *
         * ──────────────────────────────────────────────── */
        TermWriter& operator<<(const std::string& line);
        TermWriter& operator<<(const char* line);

        /* ── output ────────────────────────────────────── */
        std::string str() const;
        void flush();
        void clear();

        /* ── style access ──────────────────────────────── */
        TermStyle& style();
        const TermStyle& style() const;

    private:
        TermStyle&      _ts;
        std::ostream&   _os;
        std::ostringstream _buf;

        /* ── callout registry (indexed by name) ────────── */
        struct CalloutEntry {
            std::string         name;
            TermStyle::ElemStyle style;
            bool                used;
            CalloutEntry() : used(false) {}
        };
        CalloutEntry _callouts[MAX_CALLOUTS];
        int          _calloutCount;

        /* ── parser state ──────────────────────────────── */
        bool        _inCallout;
        std::string _calloutType;
        std::string _calloutLabel;
        std::string _calloutLines[10];
        int         _calloutLineCount;

        /* ── table parser state ────────────────────────── */
        bool        _inTable;
        TermTable*  _pendingTable;
        bool        _tableHasHeader;
        TermTable::Style _tableStyle;

        /* ── parser helpers ────────────────────────────── */
        void _parseLine(const std::string& line);
        void _flushCallout();
        void _flushTable();
        void _emit(const std::string& s);

        /* ── table line helpers ────────────────────────── */
        static bool _isTableLine(const std::string& line);
        static bool _isSepLine(const std::string& line);
        static int  _splitPipe(const std::string& line,
                               std::string* out, int maxCols);

        /* ── render a callout block using a specific style */
        std::string _renderCallout(const TermStyle::ElemStyle& es,
                                   const std::string& label,
                                   const std::string* lines,
                                   int count) const;

        /* disabled */
        TermWriter(const TermWriter&);
        TermWriter& operator=(const TermWriter&);
};

#endif
