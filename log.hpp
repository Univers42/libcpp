/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 17:45:12 by marvin            #+#    #+#             */
/*   Updated: 2025/12/23 17:45:12 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOG_HPP
# define LOG_HPP

#include <string>
#include <iostream>

/**
 * @brief Converts all characters in the string to uppercase if mod is 0.
 * @param s The string to be modified.
 * @param mod The mode of conversion.
 * @return void
 * @note
 * if mod is 0, converts all characters to uppercase.
 * if mode is 1, converts all charcters to lowercase.
 * if mode is 2, toggles the case of each character.
 */
void	strcase_toggle(std::string *s, int mod);

/* Public logging helpers used across the project */
void log_info(const std::string &msg);
void log_warn(const std::string &msg);
void log_error(const std::string &msg);

/* Run demo */
void run_logger_demo();

/* C API wrappers (implemented in log.cpp) */
extern "C" {
    void c_log_info(const char *msg);
    void c_print_message(const char *msg, const char *color_code);
    void c_run_logger_demo();
}

/* Small helper used across project */
template<typename T>
void print_line(const T& msg) {
    std::cout << msg << std::endl;
}

#endif