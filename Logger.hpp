#ifndef LOGGER_HPP
# define LOGGER_HPP

# include "VerboseDecorator.hpp"
# include "TermStyle.hpp"
# include "TermConf.hpp"
# include "Srgb.hpp"
# include <iostream>
# include <string>
# include <sstream>
# include <cstring>
# include <ctime>

/*
 *  Logger.hpp — compile-time verbose macros + factory functions
 *
 *  ┌──────────────────────────────────────────────────────────────┐
 *  │  Log line format:                                            │
 *  │                                                              │
 *  │  [Class] {file.cpp:42} {Class::func()} {14:30:01} : desc    │
 *  │   ▲        ▲              ▲                ▲          ▲      │
 *  │   │        │              │                │          │      │
 *  │   HAS_CLASS HAS_FILE_LINE HAS_FUNCTION  HAS_TIMESTAMP msg   │
 *  │                                                              │
 *  │  Metadata fields are wrapped in dim {braces} for clarity.    │
 *  │  Each segment is toggled independently via HAS_* macros.     │
 *  │  Disable one and it vanishes from the output entirely.       │
 *  └──────────────────────────────────────────────────────────────┘
 *
 *  Feature toggles (define BEFORE including Logger.hpp):
 *
 *    #define HAS_CLASS      1   ← [ClassName] prefix
 *    #define HAS_FILE_LINE  1   ← file.cpp:42
 *    #define HAS_FUNCTION   1   ← Class::method()
 *    #define HAS_TIMESTAMP  1   ← HH:MM:SS
 *    #define HAS_DATE       0   ← YYYY-MM-DD (off by default)
 *    #define HAS_EPOCH      0   ← unix timestamp (off by default)
 *
 *  VERBOSE 1  →  all macros expand, full styled output
 *  VERBOSE 0  →  all macros expand to ((void)0), zero overhead
 *
 *  Semantic log levels:
 *
 *    Level     Glyph  Color     Use case
 *    ─────────────────────────────────────────────────────
 *    info      ℹ      cyan      constructor (object birth)
 *    trace     ◇      purple    copy constructor (cloning)
 *    success   ✔      green     operator= (assignment)
 *    warn      ⚠      yellow    general / methods
 *    danger    ☠      red       destructor (object death)
 *    error     ✗      red       actual errors only
 *
 *  Auto-logging:    LOG_CTOR()     — default constructor (info  ℹ)
 *                   LOG_COPY()     — copy constructor    (trace ◇)
 *                   LOG_ASSIGN()   — operator=           (success ✔)
 *                   LOG_DTOR()     — destructor          (danger ☠)
 *                   LOG_METHOD(m)  — any method          (warn  ⚠)
 */

/* ══════════════════════════════════════════════════════════
 *  HAS_* feature defaults — override before #include
 * ══════════════════════════════════════════════════════════ */

# ifndef HAS_CLASS
#  define HAS_CLASS      1
# endif
# ifndef HAS_FILE_LINE
#  define HAS_FILE_LINE  0
# endif
# ifndef HAS_FUNCTION
#  define HAS_FUNCTION   0
# endif
# ifndef HAS_TIMESTAMP
#  define HAS_TIMESTAMP  0
# endif
# ifndef HAS_DATE
#  define HAS_DATE       0
# endif
# ifndef HAS_EPOCH
#  define HAS_EPOCH      0
# endif

/* ══════════════════════════════════════════════════════════
 *  __PRETTY_FUNCTION__ parser
 *
 *  Extracts the class name from compiler-generated signatures:
 *    "Animal::Animal()"            →  "Animal"
 *    "Cat::Cat(const Cat &)"       →  "Cat"
 *    "Dog::~Dog()"                 →  "Dog"
 *    "Cat::operator=(const Cat &)" →  "Cat"
 *    "void Cat::makeSound() const" →  "Cat"
 *
 *  Works with GCC and Clang.  Falls back gracefully to the
 *  raw string if the pattern is unrecognised.
 * ══════════════════════════════════════════════════════════ */

inline std::string _lg_extract_class(const char* pretty) {
    std::string s(pretty);
    std::string::size_type scope = s.find("::");
    if (scope == std::string::npos)
        return s;
    std::string::size_type start = scope;
    while (start > 0 && (s[start - 1] == '_'
           || (s[start - 1] >= 'A' && s[start - 1] <= 'Z')
           || (s[start - 1] >= 'a' && s[start - 1] <= 'z')
           || (s[start - 1] >= '0' && s[start - 1] <= '9')))
        --start;
    return s.substr(start, scope - start);
}

/* ── Extract full "Class::method()" signature ──────────── */
inline std::string _lg_extract_func(const char* pretty) {
    std::string s(pretty);
    /* Find the first '(' — everything before it (minus return type) is the
       qualified function name. */
    std::string::size_type paren = s.find('(');
    if (paren == std::string::npos)
        return s;
    /* Walk backwards past spaces to trim */
    std::string qualified = s.substr(0, paren);
    /* Strip leading return type: find last space before the name */
    std::string::size_type sp = qualified.rfind(' ');
    if (sp != std::string::npos)
        qualified = qualified.substr(sp + 1);
    return qualified + "()";
}

/* ── Strip directory path, keep only filename ──────────── */
inline const char* _lg_basename(const char* path) {
    const char* f = path;
    for (const char* p = path; *p; ++p) {
        if (*p == '/')
            f = p + 1;
    }
    return f;
}

/* ══════════════════════════════════════════════════════════
 *  _lg_build_meta — assemble the metadata prefix string
 *
 *  Only includes the segments whose HAS_* macros are 1.
 *  Called by the LOG_* macros with __FILE__, __LINE__,
 *  and __PRETTY_FUNCTION__ forwarded from the call site.
 * ══════════════════════════════════════════════════════════ */
/* ── ANSI helpers for metadata styling ──────────────────── */
# define _LG_DIM   "\033[2m"
# define _LG_RST   "\033[0m"

inline std::string _lg_build_meta(const char* pretty,
                                  const char* file,
                                  int line) {
    std::ostringstream os;
    bool has_any = false;

    /*  [ClassName] — bold, no dim  */
# if HAS_CLASS
    os << "[" << _lg_extract_class(pretty) << "]";
    has_any = true;
# endif

    /*  {file.cpp:42} — dim braces  */
# if HAS_FILE_LINE
    if (has_any) os << " ";
    os << _LG_DIM << "{" << _lg_basename(file) << ":" << line << "}" << _LG_RST;
    has_any = true;
# endif

    /*  {Class::method()} — dim braces  */
# if HAS_FUNCTION
    if (has_any) os << " ";
    os << _LG_DIM << "{" << _lg_extract_func(pretty) << "}" << _LG_RST;
    has_any = true;
# endif

# if HAS_DATE || HAS_TIMESTAMP || HAS_EPOCH
    {
        std::time_t now = std::time(0);
        /*  {2026-03-20} — dim braces  */
#  if HAS_DATE
        {
            char buf[20];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d", std::localtime(&now));
            if (has_any) os << " ";
            os << _LG_DIM << "{" << buf << "}" << _LG_RST;
            has_any = true;
        }
#  endif
        /*  {14:30:01} — dim braces  */
#  if HAS_TIMESTAMP
        {
            char buf[12];
            std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&now));
            if (has_any) os << " ";
            os << _LG_DIM << "{" << buf << "}" << _LG_RST;
            has_any = true;
        }
#  endif
        /*  {1742490601} — dim braces  */
#  if HAS_EPOCH
        {
            if (has_any) os << " ";
            os << _LG_DIM << "{" << static_cast<long>(now) << "}" << _LG_RST;
            has_any = true;
        }
#  endif
    }
# endif

    (void)file; (void)line; (void)pretty; /* suppress unused warnings */
    std::string m = os.str();
    if (!m.empty())
        m += " : ";
    return m;
}

/* ── Factory functions ─────────────────────────────────── */

inline VerboseDecorator InfoLog(const std::string& msg) {
    return VerboseDecorator(msg, Srgb(0, 200, 255), VerboseDecorator::DIM);
}

inline VerboseDecorator WarnLog(const std::string& msg) {
    return VerboseDecorator(msg, Srgb(255, 200, 0),
                            VerboseDecorator::BOLD | VerboseDecorator::ITALIC);
}

inline VerboseDecorator ErrorLog(const std::string& msg) {
    return VerboseDecorator(msg, Srgb(255, 60, 60),
                            VerboseDecorator::BOLD | VerboseDecorator::UNDERLINE);
}

inline VerboseDecorator SuccessLog(const std::string& msg) {
    return VerboseDecorator(msg, Srgb(0, 230, 120), VerboseDecorator::BOLD);
}

inline VerboseDecorator DangerLog(const std::string& msg) {
    return VerboseDecorator(msg, Srgb(255, 80, 80), VerboseDecorator::BOLD);
}

inline VerboseDecorator TraceLog(const std::string& msg) {
    return VerboseDecorator(msg, Srgb(180, 130, 255),
                            VerboseDecorator::DIM | VerboseDecorator::ITALIC);
}

/* ══════════════════════════════════════════════════════════
 *  Semantic log levels
 *
 *  Instead of guessing from style flags, each LOG_* macro
 *  dispatches to the correct TermStyle method directly.
 *
 *    Level     Method           Glyph   Color
 *    ──────────────────────────────────────────────
 *    info      ts.info()        ℹ       cyan
 *    trace     ts.trace()       ◇       purple
 *    success   ts.success()     ✔       green
 *    warn      ts.warn()        ⚠       yellow
 *    danger    ts.danger()      ☠       red
 *    error     ts.error()       ✗       red
 * ══════════════════════════════════════════════════════════ */

/* ── Compile-time macros ───────────────────────────────── */

# if VERBOSE

/*  PRINT_LOG_LEVEL(level, msg)
 *  level is one of: info, trace, success, warn, danger, error
 */
#  define PRINT_LOG_LEVEL(level, msg) do { \
       static TermStyle _ts; \
       std::cout << _ts.level(msg) << std::endl; \
   } while (0)

/*  Legacy PRINT_LOG — still works via style-flag heuristic  */
#  define PRINT_LOG(decorator) do { \
       static TermStyle _ts; \
       std::string _tag = (decorator).getMessage(); \
       int  _f = (decorator).getStyle(); \
       if ((_f & VerboseDecorator::BOLD) && (_f & VerboseDecorator::UNDERLINE)) \
           std::cout << _ts.error(_tag) << std::endl; \
       else if ((_f & VerboseDecorator::BOLD) && (_f & VerboseDecorator::ITALIC)) \
           std::cout << _ts.warn(_tag) << std::endl; \
       else if (_f & VerboseDecorator::BOLD) \
           std::cout << _ts.success(_tag) << std::endl; \
       else \
           std::cout << _ts.info(_tag) << std::endl; \
   } while (0)

/* ── Helper: build meta prefix from call-site ──────────── */
#  define _LG_META() \
       _lg_build_meta(__PRETTY_FUNCTION__, __FILE__, __LINE__)

/* ══════════════════════════════════════════════════════════
 *  Auto-logging macros — semantic lifecycle logging
 *
 *  Usage:
 *    Animal::Animal()             { LOG_CTOR();   }   // ℹ  info
 *    Animal::Animal(const A& o)   { LOG_COPY();   }   // ◇  trace
 *    Animal& Animal::operator=()  { LOG_ASSIGN(); }   // ✔  success
 *    Animal::~Animal()            { LOG_DTOR();   }   // ☠  danger
 *    void Animal::makeSound()     { LOG_METHOD("called"); } // ⚠ warn
 *
 *  Output (all HAS_* on):
 *    ℹ  [Cat] {Cat.cpp:19} {Cat::Cat()} {14:30:01} : default constructor called
 *    ☠  [Cat] {Cat.cpp:36} {Cat::~Cat()} {14:30:01} : destructor called
 * ══════════════════════════════════════════════════════════ */

#  define LOG_CTOR()      PRINT_LOG_LEVEL(info,    _LG_META() + "default constructor called")
#  define LOG_COPY()      PRINT_LOG_LEVEL(trace,   _LG_META() + "copy constructor called")
#  define LOG_ASSIGN()    PRINT_LOG_LEVEL(success, _LG_META() + "assignment operator called")
#  define LOG_DTOR()      PRINT_LOG_LEVEL(danger,  _LG_META() + "destructor called")
#  define LOG_METHOD(msg) PRINT_LOG_LEVEL(warn,    _LG_META() + (msg))

#  define PRINT_HEADER(title) do { \
       static TermStyle _ts; \
       std::cout << _ts.h1(title) << std::endl; \
   } while (0)

#  define PRINT_H2(title) do { \
       static TermStyle _ts; \
       std::cout << _ts.h2(title) << std::endl; \
   } while (0)

#  define PRINT_H3(title) do { \
       static TermStyle _ts; \
       std::cout << _ts.h3(title) << std::endl; \
   } while (0)

#  define PRINT_SEP() do { \
       static TermStyle _ts; \
       std::cout << _ts.separator() << std::endl; \
   } while (0)

#  define PRINT_HR() do { \
       static TermStyle _ts; \
       std::cout << _ts.hr() << std::endl; \
   } while (0)

#  define PRINT_QUOTE(msg) do { \
       static TermStyle _ts; \
       std::cout << _ts.quote(msg) << std::endl; \
   } while (0)

#  define PRINT_SECTION(title, body) do { \
       static TermStyle _ts; \
       std::cout << _ts.section(title, body) << std::endl; \
   } while (0)

#  define PRINT_TEXT(msg) do { \
       static TermStyle _ts; \
       std::cout << _ts.text(msg) << std::endl; \
   } while (0)

#  define PRINT_BOLD(msg) do { \
       static TermStyle _ts; \
       std::cout << _ts.bold(msg) << std::endl; \
   } while (0)

#  define PRINT_ITALIC(msg) do { \
       static TermStyle _ts; \
       std::cout << _ts.italic(msg) << std::endl; \
   } while (0)

# else /* VERBOSE == 0 */

#  define PRINT_LOG_LEVEL(level, msg)  ((void)0)
#  define PRINT_LOG(decorator)         ((void)0)
#  define LOG_CTOR()                   ((void)0)
#  define LOG_COPY()                   ((void)0)
#  define LOG_ASSIGN()                 ((void)0)
#  define LOG_DTOR()                   ((void)0)
#  define LOG_METHOD(msg)              ((void)0)
#  define PRINT_HEADER(title)         ((void)0)
#  define PRINT_H2(title)             ((void)0)
#  define PRINT_H3(title)             ((void)0)
#  define PRINT_SEP()                 ((void)0)
#  define PRINT_HR()                  ((void)0)
#  define PRINT_QUOTE(msg)            ((void)0)
#  define PRINT_SECTION(title, body)  ((void)0)
#  define PRINT_TEXT(msg)             ((void)0)
#  define PRINT_BOLD(msg)             ((void)0)
#  define PRINT_ITALIC(msg)           ((void)0)

# endif

#endif
