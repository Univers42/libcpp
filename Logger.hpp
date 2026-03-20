#ifndef LOGGER_HPP
# define LOGGER_HPP

# include "VerboseDecorator.hpp"
# include "TermStyle.hpp"
# include "TermConf.hpp"
# include "Srgb.hpp"
# include <iostream>

/*
 *  Logger.hpp — compile-time verbose macros + factory functions
 *
 *  VERBOSE 1  →  all macros expand, full styled output
 *  VERBOSE 0  →  all macros expand to ((void)0), zero overhead
 *
 *  Log factories:   InfoLog  WarnLog  ErrorLog  SuccessLog
 *  Block macros:    PRINT_HEADER  PRINT_H2  PRINT_H3
 *                   PRINT_SEP  PRINT_HR  PRINT_QUOTE
 *                   PRINT_SECTION  PRINT_TEXT
 *                   PRINT_BOLD  PRINT_ITALIC
 */

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

/* ── Compile-time macros ───────────────────────────────── */

# if VERBOSE

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

#  define PRINT_LOG(decorator)        ((void)0)
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
