/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buffered_socket.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/09 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP98_BUFFERED_SOCKET_HPP
#define LIBCPP98_BUFFERED_SOCKET_HPP

#include <cstddef>
#include <string>

#include "libcpp98/line_buffer.hpp"

namespace libcpp98 {

/* ── In/out buffer pair for a non-blocking line-protocol peer ──────────────
**
** Deliberately syscall-free: the owner performs recv()/send() and feeds or
** drains bytes here, which keeps this class unit-testable and keeps all
** network calls in the owner's source.
**
**   in:   feed(recv'd bytes) → nextLine() until it returns false
**   out:  queue(line) appends line + CRLF; the event loop sends outData()
**         and consume()s however much the kernel accepted.
**
** When sendqCap > 0 and the out buffer would exceed it, queue() drops the
** line and latches overflowed() — the owner's cue to disconnect the peer
** (classic SENDQ policy) instead of buffering without bound.
*/
class BufferedSocket {
public:
	explicit BufferedSocket(std::size_t maxLine = 0, std::size_t sendqCap = 0);

	/* ── input ── */
	void				feed(const char *data, std::size_t len);
	void				feed(const std::string &data);
	bool				nextLine(std::string &out);
	std::size_t			inSize() const;

	/* ── output ── */
	bool				queue(const std::string &line);  /* appends CRLF */
	bool				queueRaw(const std::string &bytes);
	const std::string	&outData() const;
	void				consume(std::size_t n);
	bool				hasPending() const;
	std::size_t			outSize() const;

	/* Sticky: set the first time a queue() would blow sendqCap. */
	bool				overflowed() const;

	void				clear();

private:
	LineBuffer	_in;
	std::string	_out;
	std::size_t	_sendqCap;
	bool		_overflow;

	bool		fits(std::size_t more);
};

} /* namespace libcpp98 */

#endif /* LIBCPP98_BUFFERED_SOCKET_HPP */
