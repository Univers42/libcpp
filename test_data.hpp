/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_data.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 21:19:11 by marvin            #+#    #+#             */
/*   Updated: 2025/12/23 21:19:11 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEST_DATA_HPP
#define TEST_DATA_HPP
#pragma once

#include <string>
#include <vector>

namespace Gens
{
	class FmtGen
	{
	public:
		std::vector<std::string> gen_all() const
		{
			std::vector<std::string> out;
			for (const auto &f : FLAGS)
			for (const auto &w : WIDTHS)
			for (const auto &p : PRECISIONS)
			for (const auto &l : LENGTHS)
			for (const auto &s : SPECS)
			{
				out.push_back(build(f, w, p, l, s));
			}
			return out;
		}

		std::vector<std::string> generate_invalid() const
		{
			return {
				"%--10d",
				"%++d",
				"%..5f",
				"%#s",
				"%*.",
				"%*f",
				"%0-+ 5d",
				"%#.s",
				"%.*",
				"%-#0+ d",
				"%5.5.5d"
			};
		}

		std::vector<std::string> generate_dynamic() const
		{
			return {
				"%*d", "%.*d", "%*.*d", "%*f", "%.*f", "%*.*f",
				"%*s", "%.*s"
			};
		}

	private:
		std::string build(
			const std::string &flags,
			const std::string &width,
			const std::string &precision,
			const std::string &length,
			const std::string &spec) const
		{
			return std::string("%") + flags + width + precision + length + spec;
		}

		static const inline std::vector<std::string> FLAGS {
			"", "-", "+", " ", "0", "#",
			"-0", "+0", " 0", "#0",
			"-#", "+#", " #", "-+#"
		};

		static const inline std::vector<std::string> WIDTHS {
			"", "1", "2", "5", "10", "*"
		};

		static const inline std::vector<std::string> PRECISIONS {
			"", ".0", ".5", ".10", ".*", ".00005"
		};

		static const inline std::vector<std::string> LENGTHS {
			"", "h", "hh", "l", "ll", "L"
		};

		static const inline std::vector<std::string> SPECS {
			"d", "i", "u", "x", "X",
			"s", "c", "p",
			"f", "F", "e", "E", "g", "G",
			"o", "%", "n"
		};
	};
}

#endif