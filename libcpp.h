/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libcpp.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/20 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_H
# define LIBCPP_H

/* ── Standard headers ──────────────────────────────────── */
#include <iostream>
#include <string>
#include <cctype>

/* ── Color & formatting ────────────────────────────────── */
#include "colors.hpp"          /* Colors::RGB, Palette, Gradient, tester::colors */
#include "Srgb.hpp"            /* Srgb — 24-bit RGB with ANSI conversion        */

/* ── Decorator pattern ─────────────────────────────────── */
#include "Decorator.hpp"       /* Base Decorator                                 */
#include "VerboseDecorator.hpp"/* Styled verbose output                          */
#include "ColorDecorator.hpp"  /* SRGB-based color decorator                     */

/* ── Terminal rendering ────────────────────────────────── */
#include "TermUtils.hpp"       /* Static UTF-8 / ANSI utilities                  */
#include "TermStyle.hpp"       /* Markdown-like terminal renderer                */
#include "TermConf.hpp"        /* Centralized TermStyle configuration            */
#include "TermTable.hpp"       /* Unicode table renderer                         */
#include "TermWriter.hpp"      /* Markdown parser with callouts                  */

/* ── Logging ───────────────────────────────────────────── */
#include "Logger.hpp"          /* PRINT_* macros, InfoLog/WarnLog/ErrorLog       */
#include "log.hpp"             /* log_info/warn/error, LoggerBuilder, demo       */
#include "message.hpp"         /* tester::Message — streamable colored messages  */

/* ── Testing & assertions ──────────────────────────────── */
#include "assertion.hpp"       /* testing::AssertionResult, C API wrappers       */
#include "postman.hpp"         /* TestReport, ASSERT_R macro, assertNoLeaks     */
#include "LeakGuard.hpp"       /* Allocation tracker with snapshot/diff          */
#include "test_data.hpp"       /* Gens::FmtGen — format string generator         */

/* ── Memory checking ───────────────────────────────────── */
#include "leaks.hpp"           /* memcheck::MemCheckFacade, RAIICollector        */

/* ── String utilities ──────────────────────────────────── */
#include "ft_string.hpp"       /* testing::internal::String, UTF-8 helpers       */

/* ── Misc ──────────────────────────────────────────────── */
#include "symbolizer.hpp"      /* symbolizer::symbolize — addr2line wrapper      */

#endif /* LIBCPP_H */
