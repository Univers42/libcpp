#ifndef TERMTABLE_HPP
# define TERMTABLE_HPP

# include <string>
# include "Srgb.hpp"
# include "TermUtils.hpp"

/* ═══════════════════════════════════════════════════════════
 *  TermTable — generic, styled Unicode table renderer
 *
 *  Usage (imperative):
 *    TermTable t(3);                    // 3 columns
 *    t.header("Name", "Type", "Sound");
 *    t.row("Rex", "Dog", "Woof!");
 *    t.row("Mimi", "Cat", "Meow");
 *    std::cout << t.render();
 *
 *  Usage (markdown, via TermWriter parser):
 *    | Name  | Type | Sound |
 *    |-------|------|-------|
 *    | Rex   | Dog  | Woof! |
 *    | Mimi  | Cat  | Meow  |
 *
 *  Limits: MAX_COLS=16, MAX_ROWS=128
 * ═══════════════════════════════════════════════════════════ */

class TermTable {
    public:
        static const int MAX_COLS = 16;
        static const int MAX_ROWS = 128;

        /* ── style config ──────────────────────────────── */
        struct Style {
            Srgb frameFg;           // box-drawing color
            Srgb headerFg;          // header text color
            Srgb cellFg;            // body text color
            Srgb titleFg;           // title text color (if set)
            int  cellPadL;          // spaces before cell text
            int  cellPadR;          // spaces after cell text
            int  maxColWidth;       // max per-column width (0=no limit)
            bool heavy;             // heavy (━┃) vs light (─│) lines
            bool roundCorners;      // rounded corners (╭╮╰╯)

            Style();
        };

        TermTable(int cols);
        TermTable(int cols, const Style& s);
        ~TermTable();

        /* ── title (optional, full-span row above headers) */
        void title(const std::string& t);

        /* ── header row ────────────────────────────────── */
        void header(const std::string& c0,
                    const std::string& c1 = "",
                    const std::string& c2 = "",
                    const std::string& c3 = "",
                    const std::string& c4 = "",
                    const std::string& c5 = "",
                    const std::string& c6 = "",
                    const std::string& c7 = "");

        /* ── data row ──────────────────────────────────── */
        void row(const std::string& c0,
                 const std::string& c1 = "",
                 const std::string& c2 = "",
                 const std::string& c3 = "",
                 const std::string& c4 = "",
                 const std::string& c5 = "",
                 const std::string& c6 = "",
                 const std::string& c7 = "");

        /* ── add a single cell (auto-advances to next row) */
        void cell(const std::string& val);

        /* ── render to string ──────────────────────────── */
        std::string render() const;

        /* ── accessors ─────────────────────────────────── */
        int cols() const;
        int rows() const;
        Style& style();
        const Style& style() const;

    private:
        int    _cols;
        int    _rowCount;
        bool   _hasTitle;
        bool   _hasHeader;
        std::string _title;
        std::string _headers[MAX_COLS];
        std::string _cells[MAX_ROWS][MAX_COLS];
        Style  _style;
        int    _curCol;     // for cell() auto-advance

        /* ── internal helpers ───────────────────────────── */
        void calcWidths(int* widths) const;
        int  totalSpan(const int* widths) const;

        std::string hline(const int* widths, int kind) const;
        std::string dataRow(const int* widths,
                            const std::string* cells,
                            const Srgb& fg,
                            bool bold) const;
        std::string fullSpanRow(const int* widths,
                                const std::string& text,
                                const Srgb& fg,
                                bool bold) const;

        /* disabled */
        TermTable(const TermTable&);
        TermTable& operator=(const TermTable&);
};

#endif
