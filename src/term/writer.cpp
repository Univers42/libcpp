/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   writer.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 20:46:22 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/term/writer.hpp"
#include <sstream>

namespace libcpp
{

/* ══════════════════════════════════════════════════════════════════════════
 *  TermWriter — OCF
 * ═════════════════════════════════════════════════════════════════════════ */

TermWriter::TermWriter()
	: _ts(0), _os(&std::cout), _owns_ts(true)
{
	_ts = new TermStyle();
}

TermWriter::TermWriter(TermStyle& ts)
	: _ts(&ts), _os(&std::cout), _owns_ts(false) {}

TermWriter::TermWriter(TermStyle& ts, std::ostream& os)
	: _ts(&ts), _os(&os), _owns_ts(false) {}

TermWriter::TermWriter(const TermWriter& o)
	: _ts(0), _os(o._os), _callouts(o._callouts), _owns_ts(o._owns_ts)
{
	if (_owns_ts && o._ts)
		_ts = new TermStyle(*o._ts);
	else
		_ts = o._ts;
}

TermWriter& TermWriter::operator=(const TermWriter& o)
{
	if (this != &o)
	{
		if (_owns_ts)
			delete _ts;
		_os = o._os;
		_callouts = o._callouts;
		_owns_ts = o._owns_ts;
		if (_owns_ts && o._ts)
			_ts = new TermStyle(*o._ts);
		else
			_ts = o._ts;
	}
	return *this;
}

TermWriter::~TermWriter()
{
	if (_owns_ts)
		delete _ts;
}

/* ── API ───────────────────────────────────────────────────────────────── */

void TermWriter::write(const std::string& markdown)
{
	std::istringstream iss(markdown);
	std::string line;
	while (std::getline(iss, line))
		_parse_line(line);
}

void TermWriter::writeln(const std::string& markdown)
{
	write(markdown);
	*_os << "\n";
}

void TermWriter::nl() { *_os << "\n"; }

void TermWriter::write_raw(const std::string& text) { *_os << text; }

void TermWriter::hr(int width, char ch)
{
	*_os << std::string(width, ch) << "\n";
}

void TermWriter::define_callout(const std::string& name, const ElemStyle& style)
{
	_callouts[name] = style;
}

/* ── parsing ───────────────────────────────────────────────────────────── */

void TermWriter::_parse_line(const std::string& line)
{
	if (!_ts) return;

	std::string trimmed = _trim(line);

	if (trimmed.empty())
	{
		*_os << "\n";
		return;
	}

	/* headings */
	if (_starts_with(trimmed, "### "))
		*_os << _ts->h3(trimmed.substr(4));
	else if (_starts_with(trimmed, "## "))
		*_os << _ts->h2(trimmed.substr(3));
	else if (_starts_with(trimmed, "# "))
		*_os << _ts->h1(trimmed.substr(2));
	/* blockquote */
	else if (_starts_with(trimmed, "> "))
		*_os << _ts->quote(trimmed.substr(2));
	/* horizontal rule */
	else if (trimmed == "---" || trimmed == "***" || trimmed == "___")
		*_os << _ts->hr();
	/* info/warn/error markers */
	else if (_starts_with(trimmed, "[info] "))
		*_os << _ts->info(trimmed.substr(7));
	else if (_starts_with(trimmed, "[warn] "))
		*_os << _ts->warn(trimmed.substr(7));
	else if (_starts_with(trimmed, "[error] "))
		*_os << _ts->error(trimmed.substr(8));
	/* callout */
	else if (_starts_with(trimmed, "[!") && trimmed.find(']') != std::string::npos)
	{
		std::size_t end = trimmed.find(']');
		std::string name = trimmed.substr(2, end - 2);
		std::string body = (end + 1 < trimmed.size()) ? _trim(trimmed.substr(end + 1)) : "";
		std::map<std::string, ElemStyle>::const_iterator it = _callouts.find(name);
		if (it != _callouts.end())
		{
			*_os << TermUtils::apply_fg(it->second.fg)
				 << TermUtils::apply_font(it->second.font);
			if (!it->second.glyph.empty())
				*_os << it->second.glyph;
			*_os << body << TermUtils::reset() << "\n";
		}
		else
			*_os << trimmed << "\n";
	}
	/* plain text */
	else
		*_os << trimmed << "\n";
}

bool TermWriter::_starts_with(const std::string& s, const std::string& prefix) const
{
	if (s.size() < prefix.size()) return false;
	return s.substr(0, prefix.size()) == prefix;
}

std::string TermWriter::_trim(const std::string& s) const
{
	std::size_t start = 0;
	while (start < s.size() && (s[start] == ' ' || s[start] == '\t'))
		++start;
	std::size_t end = s.size();
	while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t'))
		--end;
	return s.substr(start, end - start);
}

} /* namespace libcpp */
