/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argparser.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:45:02 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/util/argparser.hpp"
#include <sstream>

namespace libcpp
{
namespace util
{

/* ── ArgDef ────────────────────────────────────────────────────────────── */

ArgDef::ArgDef() : takes_value(false) {}
ArgDef::ArgDef(const std::string& sf, const std::string& lf,
			   const std::string& desc, bool tv, const std::string& def)
	: short_flag(sf), long_flag(lf), description(desc),
	  takes_value(tv), default_val(def) {}
ArgDef::ArgDef(const ArgDef& o)
	: short_flag(o.short_flag), long_flag(o.long_flag),
	  description(o.description), takes_value(o.takes_value),
	  default_val(o.default_val) {}
ArgDef& ArgDef::operator=(const ArgDef& o)
{
	if (this != &o)
	{
		short_flag = o.short_flag; long_flag = o.long_flag;
		description = o.description; takes_value = o.takes_value;
		default_val = o.default_val;
	}
	return *this;
}
ArgDef::~ArgDef() {}

/* ── ArgValue ──────────────────────────────────────────────────────────── */

ArgValue::ArgValue() : present(false) {}
ArgValue::ArgValue(const ArgValue& o)
	: key(o.key), value(o.value), present(o.present) {}
ArgValue& ArgValue::operator=(const ArgValue& o)
{
	if (this != &o) { key = o.key; value = o.value; present = o.present; }
	return *this;
}
ArgValue::~ArgValue() {}

/* ── ArgParser ─────────────────────────────────────────────────────────── */

ArgParser::ArgParser() : _def_count(0), _value_count(0), _pos_count(0) {}

ArgParser::ArgParser(const std::string& program, const std::string& desc)
	: _program(program), _description(desc),
	  _def_count(0), _value_count(0), _pos_count(0) {}

ArgParser::ArgParser(const ArgParser& o)
	: _program(o._program), _description(o._description),
	  _def_count(o._def_count), _value_count(o._value_count),
	  _pos_count(o._pos_count), _error(o._error)
{
	for (int i = 0; i < _def_count; ++i) _defs[i] = o._defs[i];
	for (int i = 0; i < _value_count; ++i) _values[i] = o._values[i];
	for (int i = 0; i < _pos_count; ++i) _positional[i] = o._positional[i];
}

ArgParser& ArgParser::operator=(const ArgParser& o)
{
	if (this != &o)
	{
		_program = o._program; _description = o._description;
		_def_count = o._def_count; _value_count = o._value_count;
		_pos_count = o._pos_count; _error = o._error;
		for (int i = 0; i < _def_count; ++i) _defs[i] = o._defs[i];
		for (int i = 0; i < _value_count; ++i) _values[i] = o._values[i];
		for (int i = 0; i < _pos_count; ++i) _positional[i] = o._positional[i];
	}
	return *this;
}

ArgParser::~ArgParser() {}

ArgParser& ArgParser::flag(const std::string& sf, const std::string& lf,
						   const std::string& desc)
{
	if (_def_count < MAX_DEFS)
		_defs[_def_count++] = ArgDef(sf, lf, desc, false, "");
	return *this;
}

ArgParser& ArgParser::option(const std::string& sf, const std::string& lf,
							 const std::string& desc, const std::string& def)
{
	if (_def_count < MAX_DEFS)
		_defs[_def_count++] = ArgDef(sf, lf, desc, true, def);
	return *this;
}

int ArgParser::_find_def(const std::string& f) const
{
	for (int i = 0; i < _def_count; ++i)
	{
		if (_defs[i].short_flag == f || _defs[i].long_flag == f)
			return i;
	}
	return -1;
}

int ArgParser::_find_value(const std::string& key) const
{
	for (int i = 0; i < _value_count; ++i)
		if (_values[i].key == key) return i;
	return -1;
}

void ArgParser::_set_value(const std::string& key, const std::string& val)
{
	int idx = _find_value(key);
	if (idx >= 0)
	{
		_values[idx].value = val;
		_values[idx].present = true;
	}
	else if (_value_count < MAX_ARGS)
	{
		_values[_value_count].key = key;
		_values[_value_count].value = val;
		_values[_value_count].present = true;
		++_value_count;
	}
}

bool ArgParser::parse(int argc, char** argv)
{
	_value_count = 0;
	_pos_count = 0;
	_error.clear();

	/* Set defaults */
	for (int i = 0; i < _def_count; ++i)
	{
		std::string key = _defs[i].long_flag.empty() ? _defs[i].short_flag
													 : _defs[i].long_flag;
		if (_defs[i].takes_value && !_defs[i].default_val.empty())
			_set_value(key, _defs[i].default_val);
	}

	bool past_flags = false;

	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];

		if (arg == "--")
		{
			past_flags = true;
			continue;
		}

		if (past_flags || arg.empty() || arg[0] != '-')
		{
			if (_pos_count < MAX_POS)
				_positional[_pos_count++] = arg;
			continue;
		}

		/* Check if it's a known flag */
		int def_idx = _find_def(arg);
		if (def_idx < 0)
		{
			/* Check for --key=value syntax */
			std::string::size_type eq = arg.find('=');
			if (eq != std::string::npos)
			{
				std::string key = arg.substr(0, eq);
				std::string val = arg.substr(eq + 1);
				def_idx = _find_def(key);
				if (def_idx >= 0)
				{
					std::string dk = _defs[def_idx].long_flag.empty()
							? _defs[def_idx].short_flag
							: _defs[def_idx].long_flag;
					_set_value(dk, val);
					continue;
				}
			}
			_error = "unknown argument: " + arg;
			return false;
		}

		std::string key = _defs[def_idx].long_flag.empty()
				? _defs[def_idx].short_flag
				: _defs[def_idx].long_flag;

		if (_defs[def_idx].takes_value)
		{
			if (i + 1 >= argc)
			{
				_error = "missing value for " + arg;
				return false;
			}
			_set_value(key, std::string(argv[++i]));
		}
		else
		{
			_set_value(key, "true");
		}
	}
	return true;
}

bool ArgParser::has(const std::string& f) const
{
	/* Resolve to canonical key */
	int di = _find_def(f);
	if (di >= 0)
	{
		std::string key = _defs[di].long_flag.empty()
				? _defs[di].short_flag : _defs[di].long_flag;
		int vi = _find_value(key);
		return (vi >= 0 && _values[vi].present);
	}
	int vi = _find_value(f);
	return (vi >= 0 && _values[vi].present);
}

std::string ArgParser::get(const std::string& f) const
{
	int di = _find_def(f);
	if (di >= 0)
	{
		std::string key = _defs[di].long_flag.empty()
				? _defs[di].short_flag : _defs[di].long_flag;
		int vi = _find_value(key);
		if (vi >= 0) return _values[vi].value;
		return _defs[di].default_val;
	}
	int vi = _find_value(f);
	if (vi >= 0) return _values[vi].value;
	return "";
}

int ArgParser::get_int(const std::string& f) const
{
	return std::atoi(get(f).c_str());
}

double ArgParser::get_double(const std::string& f) const
{
	return std::atof(get(f).c_str());
}

int ArgParser::positional_count() const { return _pos_count; }

const std::string& ArgParser::positional(int i) const
{
	static std::string empty;
	if (i >= 0 && i < _pos_count) return _positional[i];
	return empty;
}

std::string ArgParser::help() const
{
	std::ostringstream oss;
	oss << _program;
	if (!_description.empty())
		oss << " - " << _description;
	oss << "\n\nUsage: " << _program << " [options] [args...]\n\nOptions:\n";

	for (int i = 0; i < _def_count; ++i)
	{
		oss << "  ";
		if (!_defs[i].short_flag.empty())
			oss << _defs[i].short_flag;
		if (!_defs[i].short_flag.empty() && !_defs[i].long_flag.empty())
			oss << ", ";
		if (!_defs[i].long_flag.empty())
			oss << _defs[i].long_flag;
		if (_defs[i].takes_value)
			oss << " <value>";
		oss << "\n      " << _defs[i].description;
		if (!_defs[i].default_val.empty())
			oss << " (default: " << _defs[i].default_val << ")";
		oss << "\n";
	}

	oss << "  -h, --help\n      Show this help message\n";
	return oss.str();
}

std::string ArgParser::error() const { return _error; }

ArgParser& ArgParser::version(const std::string& ver)
{
	_version = ver;
	return *this;
}

std::string ArgParser::version_string() const
{
	std::string r = _program;
	if (!_version.empty()) r += " v" + _version;
	return r;
}

} /* namespace util */
} /* namespace libcpp */
