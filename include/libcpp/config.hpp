/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/22 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_CONFIG_HPP
#define LIBCPP_CONFIG_HPP

/*
** Language-level detection — the single place libcpp decides what the
** compiler in front of it can parse.
**
** libcpp ships in two build profiles (the Makefile's STD variable):
**
**   make            STD=c++17 (default)  every module        -> libftpp.a
**   make STD=c++98                       the C++98-clean subset
**                                                            -> libftpp98.a
**
** The split is not cosmetic and it is not something a macro can paper over.
** 27 of libcpp's headers use `= delete`, `= default`, default member
** initializers, or include <thread>/<chrono>/<atomic>. A C++98 compiler
** cannot *parse* those files, and an #if inside a header does not help:
** the file still has to tokenize before the preprocessor branch is taken.
** So the modern modules are excluded at the include site and at the source
** list, never guarded from within.
**
** What that buys a C++98 consumer (ft_irc, whose subject mandates C++98):
** it includes the same umbrella headers everyone else does, and the
** umbrellas leave the modern modules out on its behalf.
*/

/* 1 when the compiler is C++11 or later, 0 otherwise. */
#if defined(__cplusplus) && __cplusplus >= 201103L
# define LIBCPP_HAS_CXX11 1
#else
# define LIBCPP_HAS_CXX11 0
#endif

/* 1 when the compiler is C++17 or later, 0 otherwise. */
#if defined(__cplusplus) && __cplusplus >= 201703L
# define LIBCPP_HAS_CXX17 1
#else
# define LIBCPP_HAS_CXX17 0
#endif

/*
** LIBCPP_CXX98_PROFILE — "this translation unit belongs to libftpp98.a".
**
** The STD=c++98 build passes -DLIBCPP_CXX98_PROFILE on the command line, so
** library sources can tell the restricted build apart from merely being
** compiled by an old compiler. Consumers do not need to define it: a C++98
** compiler implies the profile, and the fallback below covers that case.
*/
#if !LIBCPP_HAS_CXX11 && !defined(LIBCPP_CXX98_PROFILE)
# define LIBCPP_CXX98_PROFILE 1
#endif

/*
** Deliberately NOT provided here: a LIBCPP_CXX11_ONLY(...) wrapper that
** swallows a C++11-only declaration inside an otherwise C++98 header. It
** needs a variadic macro to accept declarations containing commas, and
** variadic macros are a C99/C++11 feature -- so the helper warns under
** `-std=c++98 -pedantic`, in precisely the build it exists to serve. If a
** header needs C++11, list it as a modern module instead.
*/

#endif /* LIBCPP_CONFIG_HPP */
