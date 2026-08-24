/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   level_color.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/24 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_LOG_LEVEL_COLOR_HPP
#define LIBCPP_LOG_LEVEL_COLOR_HPP

#include "libcpp/log/logger.hpp"
#include "libcpp/term/color.hpp"

namespace libcpp {
namespace log {

/*
** The conventional colour for a severity.  Split out of logger.hpp so that
** including the logger does not drag the whole term module into a consumer's
** include graph: a program that logs to a file or a syslog socket has no use
** for Srgb, and a build profile may not even compile term.
*/
Srgb level_color(Level lv);

} /* namespace log */
} /* namespace libcpp */

#endif /* LIBCPP_LOG_LEVEL_COLOR_HPP */
