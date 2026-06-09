/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   line_buffer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/09 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP98_LINE_BUFFER_HPP
#define LIBCPP98_LINE_BUFFER_HPP

#include <cstddef>
#include <string>

namespace libcpp98 {

/* ── Delimiter framing for line-oriented wire protocols ────────────────────
**
** Accumulates raw bytes (e.g. from a non-blocking recv()) and hands back
** complete lines, one at a time.  A line ends at '\n'; a preceding '\r' is
** stripped, so both CRLF and bare-LF peers are handled uniformly.  Partial
** fragments stay buffered until their terminator arrives.
**
** When maxLine > 0 and the buffer grows past it without a terminator, the
** first maxLine bytes are force-extracted as a line and the remainder is
** dropped — a flood guard for protocols with a fixed line limit (IRC: 512).
*/
class LineBuffer {
public:
	explicit LineBuffer(std::size_t maxLine = 0);

	void		append(const char *data, std::size_t len);
	void		append(const std::string &data);

	/* Extract the next complete line (terminator removed) into `out`.
	** Returns false when no complete line is buffered.  Empty lines are
	** returned as-is; filtering them is the caller's policy. */
	bool		next(std::string &out);

	std::size_t	size() const;
	void		clear();

private:
	std::string	_buf;
	std::size_t	_maxLine;
};

} /* namespace libcpp98 */

#endif /* LIBCPP98_LINE_BUFFER_HPP */
