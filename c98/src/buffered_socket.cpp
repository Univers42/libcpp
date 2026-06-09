/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buffered_socket.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/09 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp98/buffered_socket.hpp"

namespace libcpp98 {

BufferedSocket::BufferedSocket(std::size_t maxLine, std::size_t sendqCap)
	: _in(maxLine),
	  _out(),
	  _sendqCap(sendqCap),
	  _overflow(false)
{
}

/* ── input ── */

void BufferedSocket::feed(const char *data, std::size_t len)
{
	_in.append(data, len);
}

void BufferedSocket::feed(const std::string &data)
{
	_in.append(data);
}

bool BufferedSocket::nextLine(std::string &out)
{
	return _in.next(out);
}

std::size_t BufferedSocket::inSize() const
{
	return _in.size();
}

/* ── output ── */

bool BufferedSocket::fits(std::size_t more)
{
	if (_sendqCap != 0 && _out.size() + more > _sendqCap)
	{
		_overflow = true;
		return false;
	}
	return true;
}

bool BufferedSocket::queue(const std::string &line)
{
	if (!fits(line.size() + 2))
		return false;
	_out += line;
	_out += "\r\n";
	return true;
}

bool BufferedSocket::queueRaw(const std::string &bytes)
{
	if (!fits(bytes.size()))
		return false;
	_out += bytes;
	return true;
}

const std::string &BufferedSocket::outData() const
{
	return _out;
}

void BufferedSocket::consume(std::size_t n)
{
	_out.erase(0, n);
}

bool BufferedSocket::hasPending() const
{
	return !_out.empty();
}

std::size_t BufferedSocket::outSize() const
{
	return _out.size();
}

bool BufferedSocket::overflowed() const
{
	return _overflow;
}

void BufferedSocket::clear()
{
	_in.clear();
	_out.clear();
	_overflow = false;
}

} /* namespace libcpp98 */
