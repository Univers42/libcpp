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
** maxLine > 0 makes the line limit an INVARIANT, not a hint: next() never
** yields more than maxLine bytes, whether or not a terminator has arrived.
** An over-long line is cut at maxLine and the rest of it — everything up to
** and including its terminator — is discarded, so the tail can never
** resurface as a line of its own.  That last part is what makes the guard
** safe for a command protocol: without it, a peer could push a command past
** the limit simply by prefixing it with maxLine bytes of padding, and the
** parser would happily execute the remainder.
**
** maxLine == 0 disables the guard entirely (lines may be any length).
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
	/* True while the tail of an over-long line is still being thrown away:
	** every byte up to and including the next '\n' belongs to a line that
	** was already delivered (truncated) and must not be framed again. */
	bool		_discarding;

	/* Consumes buffered bytes up to and including the next '\n'.  Returns
	** true once the terminator was found and _discarding cleared. */
	bool		dropPendingTail();
};

} /* namespace libcpp98 */

#endif /* LIBCPP98_LINE_BUFFER_HPP */
