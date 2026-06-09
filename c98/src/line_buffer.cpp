/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   line_buffer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/09 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp98/line_buffer.hpp"

namespace libcpp98 {

LineBuffer::LineBuffer(std::size_t maxLine)
	: _buf(),
	  _maxLine(maxLine)
{
}

void LineBuffer::append(const char *data, std::size_t len)
{
	_buf.append(data, len);
}

void LineBuffer::append(const std::string &data)
{
	_buf += data;
}

bool LineBuffer::next(std::string &out)
{
	std::string::size_type pos = _buf.find('\n');

	if (pos != std::string::npos)
	{
		out.assign(_buf, 0, pos);
		_buf.erase(0, pos + 1);
		if (!out.empty() && out[out.size() - 1] == '\r')
			out.erase(out.size() - 1);
		return true;
	}
	/* Flood guard: terminator never arrived but the line limit is blown. */
	if (_maxLine != 0 && _buf.size() > _maxLine)
	{
		out.assign(_buf, 0, _maxLine);
		_buf.clear();
		return true;
	}
	return false;
}

std::size_t LineBuffer::size() const
{
	return _buf.size();
}

void LineBuffer::clear()
{
	_buf.clear();
}

} /* namespace libcpp98 */
