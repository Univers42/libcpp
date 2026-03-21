/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:05:41 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/util/config.hpp"
#include <sstream>
#include <cstdlib>

namespace libcpp
{
namespace util
{

/* ── ConfigEntry ────────────────────────────────────────────────────────── */

ConfigEntry::ConfigEntry() : section(), key(), value() {}

ConfigEntry::ConfigEntry(const std::string& s, const std::string& k,
						 const std::string& v)
	: section(s), key(k), value(v) {}

ConfigEntry::ConfigEntry(const ConfigEntry& o)
	: section(o.section), key(o.key), value(o.value) {}

ConfigEntry& ConfigEntry::operator=(const ConfigEntry& o)
{
	if (this != &o)
	{
		section = o.section;
		key = o.key;
		value = o.value;
	}
	return *this;
}

ConfigEntry::~ConfigEntry() {}

/* ── helpers (file-scope) ───────────────────────────────────────────────── */

static std::string trim(const std::string& s)
{
	std::string::size_type start = s.find_first_not_of(" \t\r\n");
	if (start == std::string::npos)
		return "";
	std::string::size_type end = s.find_last_not_of(" \t\r\n");
	return s.substr(start, end - start + 1);
}

static std::string to_lower(const std::string& s)
{
	std::string r(s);
	for (std::string::size_type i = 0; i < r.size(); ++i)
	{
		if (r[i] >= 'A' && r[i] <= 'Z')
			r[i] = static_cast<char>(r[i] + 32);
	}
	return r;
}

/* ── Config ─────────────────────────────────────────────────────────────── */

Config::Config() : _count(0) {}

Config::Config(const Config& o) : _count(o._count)
{
	for (int i = 0; i < _count; ++i)
		_entries[i] = o._entries[i];
}

Config& Config::operator=(const Config& o)
{
	if (this != &o)
	{
		_count = o._count;
		for (int i = 0; i < _count; ++i)
			_entries[i] = o._entries[i];
	}
	return *this;
}

Config::~Config() {}

/* ── parse ──────────────────────────────────────────────────────────────── */

void Config::_parse(const std::string& content)
{
	std::istringstream iss(content);
	std::string line;
	std::string current_section;

	while (std::getline(iss, line))
	{
		std::string trimmed = trim(line);
		if (trimmed.empty())
			continue;
		/* comment */
		if (trimmed[0] == '#' || trimmed[0] == ';')
			continue;
		/* section header */
		if (trimmed[0] == '[')
		{
			std::string::size_type close = trimmed.find(']');
			if (close != std::string::npos)
				current_section = trim(trimmed.substr(1, close - 1));
			continue;
		}
		/* key = value */
		std::string::size_type eq = trimmed.find('=');
		if (eq == std::string::npos)
			continue;
		std::string k = trim(trimmed.substr(0, eq));
		std::string v = trim(trimmed.substr(eq + 1));

		/* Remove surrounding quotes from value */
		if (v.size() >= 2)
		{
			if ((v[0] == '"' && v[v.size() - 1] == '"') ||
				(v[0] == '\'' && v[v.size() - 1] == '\''))
			{
				v = v.substr(1, v.size() - 2);
			}
		}

		set(current_section, k, v);
	}
}

/* ── load / save ────────────────────────────────────────────────────────── */

bool Config::load_file(const std::string& path)
{
	std::ifstream ifs(path.c_str());
	if (!ifs.is_open())
		return false;
	std::string content;
	std::string line;
	while (std::getline(ifs, line))
	{
		content += line;
		content += '\n';
	}
	_parse(content);
	return true;
}

bool Config::load_string(const std::string& ini)
{
	_parse(ini);
	return true;
}

bool Config::save(const std::string& path) const
{
	std::ofstream ofs(path.c_str());
	if (!ofs.is_open())
		return false;

	/* Collect unique sections in order of first appearance */
	std::string sections[MAX_ENTRIES];
	int sec_count = 0;

	for (int i = 0; i < _count; ++i)
	{
		bool found = false;
		for (int j = 0; j < sec_count; ++j)
		{
			if (sections[j] == _entries[i].section)
			{
				found = true;
				break;
			}
		}
		if (!found && sec_count < MAX_ENTRIES)
			sections[sec_count++] = _entries[i].section;
	}

	for (int s = 0; s < sec_count; ++s)
	{
		if (!sections[s].empty())
			ofs << "[" << sections[s] << "]\n";
		for (int i = 0; i < _count; ++i)
		{
			if (_entries[i].section == sections[s])
				ofs << _entries[i].key << " = " << _entries[i].value << "\n";
		}
		ofs << "\n";
	}
	return true;
}

/* ── query ──────────────────────────────────────────────────────────────── */

int Config::_find(const std::string& section, const std::string& key) const
{
	for (int i = 0; i < _count; ++i)
	{
		if (_entries[i].section == section && _entries[i].key == key)
			return i;
	}
	return -1;
}

bool Config::has(const std::string& section, const std::string& key) const
{
	return _find(section, key) >= 0;
}

std::string Config::get(const std::string& section, const std::string& key) const
{
	int idx = _find(section, key);
	if (idx < 0)
		return "";
	return _entries[idx].value;
}

std::string Config::get(const std::string& section, const std::string& key,
						const std::string& default_val) const
{
	int idx = _find(section, key);
	if (idx < 0)
		return default_val;
	return _entries[idx].value;
}

int Config::get_int(const std::string& section, const std::string& key,
					int default_val) const
{
	int idx = _find(section, key);
	if (idx < 0)
		return default_val;
	char* end = 0;
	long val = std::strtol(_entries[idx].value.c_str(), &end, 10);
	if (end == _entries[idx].value.c_str())
		return default_val;
	return static_cast<int>(val);
}

double Config::get_double(const std::string& section, const std::string& key,
						  double default_val) const
{
	int idx = _find(section, key);
	if (idx < 0)
		return default_val;
	char* end = 0;
	double val = std::strtod(_entries[idx].value.c_str(), &end);
	if (end == _entries[idx].value.c_str())
		return default_val;
	return val;
}

bool Config::get_bool(const std::string& section, const std::string& key,
					  bool default_val) const
{
	int idx = _find(section, key);
	if (idx < 0)
		return default_val;
	std::string v = to_lower(trim(_entries[idx].value));
	if (v == "true" || v == "yes" || v == "1" || v == "on")
		return true;
	if (v == "false" || v == "no" || v == "0" || v == "off")
		return false;
	return default_val;
}

/* ── mutation ───────────────────────────────────────────────────────────── */

void Config::set(const std::string& section, const std::string& key,
				 const std::string& value)
{
	int idx = _find(section, key);
	if (idx >= 0)
	{
		_entries[idx].value = value;
		return;
	}
	if (_count >= MAX_ENTRIES)
		return;
	_entries[_count] = ConfigEntry(section, key, value);
	++_count;
}

/* ── iteration ──────────────────────────────────────────────────────────── */

int Config::entry_count() const { return _count; }

const ConfigEntry& Config::entry(int i) const
{
	if (i < 0 || i >= _count)
		return _entries[0];
	return _entries[i];
}

void Config::clear()
{
	for (int i = 0; i < _count; ++i)
		_entries[i] = ConfigEntry();
	_count = 0;
}

} /* namespace util */
} /* namespace libcpp */
