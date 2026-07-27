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
	  _maxLine(maxLine),
	  _discarding(false)
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

/* Throw away the remains of an over-long line, up to and including its
** terminator.  Everything buffered before that terminator is part of a line
** already delivered in truncated form. */
bool LineBuffer::dropPendingTail()
{
	std::string::size_type pos = _buf.find('\n');
	if (pos == std::string::npos)
	{
		_buf.clear(); /* all of it is still that same line */
		return false;
	}
	_buf.erase(0, pos + 1);
	_discarding = false;
	return true;
}

bool LineBuffer::next(std::string &out)
{
	if (_discarding && !dropPendingTail())
		return false;

	std::string::size_type pos = _buf.find('\n');

	if (pos != std::string::npos)
	{
		/* A terminated line can still blow the cap.  Deliver the first
		** maxLine bytes and drop the rest of it -- taking the whole line
		** here is what used to make maxLine advisory. The terminator is
		** already in hand, so nothing needs discarding afterwards. */
		if (_maxLine != 0 && pos > _maxLine)
		{
			out.assign(_buf, 0, _maxLine);
			_buf.erase(0, pos + 1);
			return true;
		}
		out.assign(_buf, 0, pos);
		_buf.erase(0, pos + 1);
		if (!out.empty() && out[out.size() - 1] == '\r')
			out.erase(out.size() - 1);
		return true;
	}

	/* Flood guard: the terminator has not arrived and the limit is already
	** blown.  Cut here and remember to swallow the rest of this line --
	** including bytes that have not been received yet. */
	if (_maxLine != 0 && _buf.size() > _maxLine)
	{
		out.assign(_buf, 0, _maxLine);
		_buf.clear(); /* the remainder is more of the same over-long line */
		_discarding = true;
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
	_discarding = false;
}

} /* namespace libcpp98 */
