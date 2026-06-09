/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   csv_writer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/09 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp98/csv_writer.hpp"

namespace libcpp98 {

CsvWriter::CsvWriter()
	: _out(),
	  _wasEmpty(true)
{
}

CsvWriter::~CsvWriter()
{
	close();
}

bool CsvWriter::open(const std::string &path)
{
	/* Probe before opening so the caller can write a header exactly once. */
	{
		std::ifstream probe(path.c_str());
		if (probe.good() && probe.peek() != std::ifstream::traits_type::eof())
			_wasEmpty = false;
	}
	_out.open(path.c_str(), std::ios::out | std::ios::app);
	return _out.is_open();
}

bool CsvWriter::isNewFile() const
{
	return _wasEmpty;
}

bool CsvWriter::ok() const
{
	return _out.is_open();
}

void CsvWriter::close()
{
	if (_out.is_open())
		_out.close();
}

std::string CsvWriter::escape(const std::string &field)
{
	bool needQuote = field.find(',') != std::string::npos
				  || field.find('"') != std::string::npos
				  || field.find('\n') != std::string::npos
				  || field.find('\r') != std::string::npos;
	if (!needQuote)
		return field;

	std::string out = "\"";
	for (std::string::size_type i = 0; i < field.size(); ++i)
	{
		if (field[i] == '"')
			out += "\"\""; /* CSV doubles embedded quotes */
		else
			out += field[i];
	}
	out += "\"";
	return out;
}

void CsvWriter::row(const std::vector<std::string> &fields)
{
	if (!_out.is_open())
		return;
	for (std::vector<std::string>::size_type i = 0; i < fields.size(); ++i)
	{
		if (i != 0)
			_out << ',';
		_out << escape(fields[i]);
	}
	_out << '\n' << std::flush;
	_wasEmpty = false;
}

} /* namespace libcpp98 */
