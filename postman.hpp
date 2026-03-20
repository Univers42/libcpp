/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   postman.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/06 20:18:16 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE04_EX00_POSTMAN_HPP_
#define CPP_MODULE04_EX00_POSTMAN_HPP_

/**
 * @file postman.hpp
 * @brief Provides a terminal-based, stylized test reporting framework.
 *
 * This file contains the definitions for ANSI color macros, internal data
 * structures, the Singleton `TestReport` class, and the `ASSERT_R` macro used
 * to evaluate and record test conditions with real-time output and a final
 * Unicode-formatted table summary.
 *
 * @section Overview
 * The TestReport system allows developers to:
 * - Group tests into suites with `beginSuite()`
 * - Record individual assertions with the `ASSERT_R` macro
 * - Immediately display PASS/FAIL to stdout/stderr
 * - Generate a final summary table with Unicode box-drawing characters
 *
 * @section Usage Example
 * @code
 * TestReport::instance().beginSuite("My Test Suite");
 * ASSERT_R(value == expected, "Value matches expected");
 * ASSERT_R(ptr != NULL, "Pointer is not null");
 * TestReport::instance().print();
 * @endcode
 *
 * @author dlesieur <dlesieur@student.42.fr>
 * @date 2026-03-06
 */

#include <iostream>
#include <string>

/** @defgroup ColorMacros ANSI Color & Style Macros */
/** @{ */

/** Reset all text formatting and colors. */
#define PM_RST   "\033[0m"

/** Bold/bright text modifier. */
#define PM_BOLD  "\033[1m"

/** Dim/dark text modifier. */
#define PM_DIM   "\033[2m"

/** Italic text modifier. */
#define PM_ITAL  "\033[3m"

/** Foreground: light gray (RGB 253). */
#define PM_FG    "\033[38;5;253m"

/** Frame elements: bold purple (RGB 141). */
#define PM_FRAME "\033[1;38;5;141m"

/** Header text: light purple (RGB 183). */
#define PM_HEAD  "\033[38;5;183m"

/** Title text: bright magenta (RGB 219). */
#define PM_TITLE "\033[38;5;219m"

/** Suite name: muted purple (RGB 103). */
#define PM_SUITE "\033[38;5;103m"

/** Row ID number: dark gray (RGB 245). */
#define PM_ID    "\033[38;5;245m"

/** Pass status: bright green (RGB 114). */
#define PM_PASS  "\033[38;5;114m"

/** Fail status: bright red-orange (RGB 210). */
#define PM_FAIL  "\033[38;5;210m"

/** @} */

/** Maximum number of assertion records that can be stored before overflow. */
#define PM_MAX_ROWS 256

/**
 * @struct PmRow
 * @brief Represents a single test assertion record.
 *
 * Each record captures the sequential assertion ID, the test suite it belongs
 * to, the textual label/message of the assertion, and its pass/fail status.
 *
 * @var int id
 *   Sequential ID number (1-indexed) for the assertion in the report.
 *
 * @var std::string suite
 *   Name of the test suite this assertion belongs to.
 *
 * @var std::string label
 *   Human-readable description of what the assertion tests.
 *
 * @var bool passed
 *   True if the assertion evaluated to true, false otherwise.
 */
struct PmRow {
  int         id;
  std::string suite;
  std::string label;
  bool        passed;
};

/**
 * @struct PmCols
 * @brief Stores dynamically calculated column widths for the report table.
 *
 * This struct holds the maximum visual widths (in terminal columns, accounting
 * for UTF-8 character widths) for each column of the test report table. The
 * `span()` method returns the total width of the entire table.
 *
 * @var int id
 *   Width of the "#" column (assertion ID).
 *
 * @var int suite
 *   Width of the "Suite" column (test suite names).
 *
 * @var int label
 *   Width of the "Assertion" column (assertion descriptions).
 *
 * @var int stat
 *   Width of the "Result" column (PASS/FAIL status).
 *
 * @see PmCols::span() for total table width calculation.
 */
struct PmCols {
  int id;
  int suite;
  int label;
  int stat;

  /**
   * @brief Calculate the total visual width of the table.
   * @return Total width including padding, column separators, and frame borders.
   */
  int span() const;
};

/**
 * @class TestReport
 * @brief A Singleton class that collects and prints formatted test results.
 *
 * The TestReport class maintains an internal array of up to PM_MAX_ROWS test
 * records. It provides methods to:
 * - Group tests into logical suites with beginSuite()
 * - Record individual pass/fail assertions with record()
 * - Render a formatted UTF-8 table with print()
 *
 * The table includes:
 * - A title row with total assertion counts
 * - Headers for (#, Suite, Assertion, Result)
 * - Data rows grouped by suite
 * - Suite summaries (X/Y passed)
 * - A final verdict (all passed or X failed)
 *
 * All text output is styled with ANSI 256-color codes and Unicode box-drawing
 * characters.
 *
 * @note This class uses the Singleton pattern. Access via TestReport::instance().
 *
 * @see ASSERT_R macro for typical usage.
 */
class TestReport {
 public:
  /**
   * @brief Get the Singleton instance of TestReport.
   * @return Reference to the global TestReport instance.
   */
  static TestReport& instance();

  void  snapshotMemory();
  void  assertNoLeaks(const char* label, int tolerance = 0);
  /**
   * @brief Begin a new test suite.
   *
   * All subsequent assertions recorded before the next beginSuite() call
   * will belong to this suite.
   *
   * @param name The name/description of the test suite.
   */
  void beginSuite(const std::string& name);

  /**
   * @brief Record a single assertion result.
   *
   * Stores the assertion in the internal buffer. If the buffer is full
   * (PM_MAX_ROWS reached), the assertion is silently ignored.
   *
   * @param label The description of what the assertion tests.
   * @param passed True if the assertion passed, false otherwise.
   */
  void record(const std::string& label, bool passed);

  /**
   * @brief Print the complete formatted test report to stdout.
   *
   * Generates and prints:
   * - Title row with summary counts
   * - Column headers
   * - All recorded assertions grouped by suite
   * - Suite summaries
   * - Final verdict
   *
   * Uses UTF-8 box-drawing characters and ANSI color codes.
   */
  void print() const;

 private:
  /** @brief Private constructor (Singleton pattern). */
  TestReport();

  /** @brief Deleted copy constructor (non-copyable). */
  TestReport(const TestReport&);

  /** @brief Deleted assignment operator (non-assignable). */
  TestReport& operator=(const TestReport&);

  /** Array of test assertion records. */
  PmRow       _rows[PM_MAX_ROWS];

  /** Current number of recorded assertions. */
  int         _count;

  /** Name of the current test suite. */
  std::string _currentSuite;

  /**
   * @brief Calculate the visual width of a UTF-8 string.
   *
   * Accounts for multi-byte UTF-8 characters (e.g., box-drawing glyphs),
   * returning the number of terminal columns occupied.
   *
   * @param s The string to measure.
   * @return Visual width in terminal columns.
   */
  static int         visWidth(const std::string& s);

  /**
   * @brief Fit a string to a given width, truncating or padding as needed.
   *
   * If the string is longer than the desired width, it is truncated with a
   * trailing '~'. If shorter, it is right-padded with spaces.
   *
   * @param s The string to fit.
   * @param w The desired visual width.
   * @return The fitted string.
   */
  static std::string fitTo(const std::string& s, int w);

  /**
   * @brief Repeat a UTF-8 character n times.
   *
   * Useful for drawing horizontal lines with box-drawing characters.
   *
   * @param u UTF-8 character(s) as a C-string (may be multi-byte).
   * @param n Number of repetitions.
   * @return The repeated string.
   */
  static std::string repCh(const char* u, int n);

  /**
   * @brief Calculate column widths based on data content.
   *
   * Analyzes all rows to determine the maximum width needed for each column,
   * subject to configurable limits (e.g., suite <= 40, label <= 44).
   *
   * @param rows Array of test records.
   * @param n Number of records in the array.
   * @return A PmCols struct with calculated widths.
   */
  static PmCols      calcCols(const PmRow* rows, int n);

  /**
   * @brief Draw a horizontal line (rule) in the table.
   *
   * Supports various junction styles for different sections:
   * - 0: Top (round corners)
   * - 3: Bottom (round corners)
   * - 4: Merge (full-span, no column junctions)
   * - 7: Cross (columns above and below)
   * - 8: Column open (columns start below)
   * - 9: Column close (columns end above)
   *
   * @param c Column width configuration.
   * @param kind Junction style (0, 3, 4, 5, 7, 8, or 9).
   */
  static void drawHLine(const PmCols& c, int kind);
  /**
   * @brief Draw the table header row.
   * @param c Column width configuration.
   */
  static void drawHeader(const PmCols& c);

  /**
   * @brief Draw a single data row for an assertion.
   * @param c Column width configuration.
   * @param r The assertion record to display.
   */
  static void drawRow(const PmCols& c, const PmRow& r);

  /**
   * @brief Draw a suite summary line.
   * @param c Column width configuration.
   * @param s Suite name.
   * @param p Number of passed assertions in the suite.
   * @param f Number of failed assertions in the suite.
   */
  static void drawSuiteSum(const PmCols& c,
                           const std::string& s, int p, int f);

  /**
   * @brief Draw the title section of the report.
   * @param c Column width configuration.
   * @param p Total number of passed assertions.
   * @param f Total number of failed assertions.
   * @param t Total number of assertions.
   */
  static void drawTitle(const PmCols& c, int p, int f, int t);

  /**
   * @brief Draw the final verdict section.
   * @param c Column width configuration.
   * @param p Total number of passed assertions.
   * @param f Total number of failed assertions.
   */
  static void drawVerdict(const PmCols& c, int p, int f);
};

/**
 * @def ASSERT_R(condition, message)
 * @brief Evaluate a test condition and record the result in real-time.
 *
 * This macro is the primary way to record test assertions. It:
 * 1. Evaluates the boolean `condition`
 * 2. Immediately prints to stdout (PASS in green) or stderr (FAIL in red)
 * 3. Records the result in TestReport::instance() for the final summary table
 *
 * The printed output is prefixed with "[PASS]" or "[FAIL]" in color, followed
 * by the message and (for failures) the C expression that was tested.
 *
 * @param condition The boolean expression to evaluate.
 * @param message A human-readable description of what is being tested.
 *
 * @example
 * @code
 * ASSERT_R(myVar == 42, "Variable is 42");
 * ASSERT_R(ptr != NULL, "Pointer is not null");
 * ASSERT_R(someCondition, "Some important invariant holds");
 * @endcode
 */
#define ASSERT_R(condition, message)                                          \
  do {                                                                        \
    bool _ok = (condition);                                                   \
    TestReport::instance().record((message), _ok);                            \
    if (!_ok)                                                                 \
      std::cerr << PM_FAIL << PM_BOLD << "  [FAIL] " << PM_RST              \
                << (message) << "  (" << #condition << ")\n";                \
    else                                                                      \
      std::cout << PM_PASS << PM_BOLD << "  [PASS] " << PM_RST              \
                << (message) << "\n";                                        \
  } while (0)

#define ASSERT_NO_LEAKS(message) \
  TestReport::instance().assertNoLeaks(message)

#define ASSERT_NO_LEAKS_T(message, tolerance) \
  TestReport::instance().assertNoLeaks(message, tolerance)
  
#endif
