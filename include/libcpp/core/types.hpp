/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_CORE_TYPES_HPP
# define LIBCPP_CORE_TYPES_HPP

# include <cstddef>
# include <string>

namespace libcpp
{
	/* Common typedefs for C++98 */
	typedef unsigned char		u8;
	typedef unsigned short		u16;
	typedef unsigned int		u32;
	typedef unsigned long long	u64;
	typedef signed char			i8;
	typedef short				i16;
	typedef int					i32;
	typedef long long			i64;
	typedef float				f32;
	typedef double				f64;
	typedef std::size_t			usize;

	/* Null pointer constant for C++98 */
	static const int NULLPTR = 0;

} /* namespace libcpp */

#endif /* LIBCPP_CORE_TYPES_HPP */
