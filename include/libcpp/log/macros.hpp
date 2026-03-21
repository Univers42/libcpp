/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macros.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 20:50:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_LOG_MACROS_HPP
# define LIBCPP_LOG_MACROS_HPP

# include "libcpp/log/logger.hpp"
# include <sstream>

/*
** Convenience logging macros that forward to the global logger.
** Each macro builds a std::string via std::ostringstream so you can
** stream multiple values:
**
**   LOG_INFO("loaded " << count << " items");
**   LOG_WARN("file not found: " << path);
*/

# define LIBCPP_LOG_(level, expr) \
	do { \
		std::ostringstream _libcpp_oss; \
		_libcpp_oss << expr; \
		libcpp::log::global().log(level, _libcpp_oss.str()); \
	} while (0)

# define LOG_TRACE(expr) LIBCPP_LOG_(libcpp::log::LTRACE, expr)
# define LOG_DEBUG(expr) LIBCPP_LOG_(libcpp::log::LDEBUG, expr)
# define LOG_INFO(expr)  LIBCPP_LOG_(libcpp::log::LINFO,  expr)
# define LOG_WARN(expr)  LIBCPP_LOG_(libcpp::log::LWARN,  expr)
# define LOG_ERROR(expr) LIBCPP_LOG_(libcpp::log::LERROR, expr)
# define LOG_FATAL(expr) LIBCPP_LOG_(libcpp::log::LFATAL, expr)

/*
** Constructor / destructor lifecycle tracing.
** Usage in class body:
**   MyClass::MyClass()  { LOG_CTOR("MyClass"); }
**   MyClass::~MyClass() { LOG_DTOR("MyClass"); }
*/

# define LOG_CTOR(name) LOG_TRACE(name << " constructed")
# define LOG_DTOR(name) LOG_TRACE(name << " destroyed")

#endif /* LIBCPP_LOG_MACROS_HPP */
