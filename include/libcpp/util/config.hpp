/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:04:22 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_UTIL_CONFIG_HPP
# define LIBCPP_UTIL_CONFIG_HPP

# include <string>
# include <fstream>

namespace libcpp
{
namespace util
{

/* ── ConfigEntry — one key=value pair in a section ─────────────────────── */

struct ConfigEntry
{
	std::string section;
	std::string key;
	std::string value;

	ConfigEntry();
	ConfigEntry(const std::string& s, const std::string& k, const std::string& v);
	ConfigEntry(const ConfigEntry& o);
	ConfigEntry& operator=(const ConfigEntry& o);
	~ConfigEntry();
};

/* ── Config — INI-style configuration parser ───────────────────────────── */
/*
** Supports:
**   [section]
**   key = value
**   # comments
**   ; comments
*/

class Config
{
public:
	static const int MAX_ENTRIES = 512;

	Config();
	Config(const Config& o);
	Config& operator=(const Config& o);
	~Config();

	/* Load from file or string */
	bool load_file(const std::string& path);
	bool load_string(const std::string& ini);

	/* Query */
	bool        has(const std::string& section, const std::string& key) const;
	std::string get(const std::string& section, const std::string& key) const;
	std::string get(const std::string& section, const std::string& key,
					const std::string& default_val) const;
	int         get_int(const std::string& section, const std::string& key,
						int default_val) const;
	double      get_double(const std::string& section, const std::string& key,
						   double default_val) const;
	bool        get_bool(const std::string& section, const std::string& key,
						 bool default_val) const;

	/* Mutation */
	void set(const std::string& section, const std::string& key,
			 const std::string& value);

	/* Save to file */
	bool save(const std::string& path) const;

	/* Iteration */
	int                 entry_count() const;
	const ConfigEntry&  entry(int i) const;

	/* Clear all entries */
	void clear();

private:
	ConfigEntry _entries[MAX_ENTRIES];
	int         _count;

	int  _find(const std::string& section, const std::string& key) const;
	void _parse(const std::string& content);
};

} /* namespace util */
} /* namespace libcpp */

#endif /* LIBCPP_UTIL_CONFIG_HPP */
