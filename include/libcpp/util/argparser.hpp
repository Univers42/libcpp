/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argparser.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:04:27 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_UTIL_ARGPARSER_HPP
# define LIBCPP_UTIL_ARGPARSER_HPP

# include <string>
# include <cstdlib>

namespace libcpp
{
namespace util
{

/* ── ArgDef — definition of one CLI argument ───────────────────────────── */

struct ArgDef
{
	std::string short_flag;  /* e.g. "-v" */
	std::string long_flag;   /* e.g. "--verbose" */
	std::string description;
	bool        takes_value; /* true if it expects a following value */
	std::string default_val;

	ArgDef();
	ArgDef(const std::string& sf, const std::string& lf,
		   const std::string& desc, bool tv, const std::string& def);
	ArgDef(const ArgDef& o);
	ArgDef& operator=(const ArgDef& o);
	~ArgDef();
};

/* ── ArgValue — parsed argument value ──────────────────────────────────── */

struct ArgValue
{
	std::string key;
	std::string value;
	bool        present;

	ArgValue();
	ArgValue(const ArgValue& o);
	ArgValue& operator=(const ArgValue& o);
	~ArgValue();
};

/* ── ArgParser — CLI argument parser ───────────────────────────────────── */

class ArgParser
{
public:
	static const int MAX_DEFS = 64;
	static const int MAX_ARGS = 64;
	static const int MAX_POS  = 64;

	ArgParser();
	ArgParser(const std::string& program_name, const std::string& description);
	ArgParser(const ArgParser& o);
	ArgParser& operator=(const ArgParser& o);
	~ArgParser();

	/* Define arguments */
	ArgParser& flag(const std::string& short_f, const std::string& long_f,
					const std::string& desc);
	ArgParser& option(const std::string& short_f, const std::string& long_f,
					  const std::string& desc, const std::string& default_val);

	/* Parse */
	bool parse(int argc, char** argv);

	/* Query results */
	bool        has(const std::string& flag) const;
	std::string get(const std::string& flag) const;
	int         get_int(const std::string& flag) const;
	double      get_double(const std::string& flag) const;

	/* Positional args (anything not matched as flag/option) */
	int                positional_count() const;
	const std::string& positional(int i) const;

	/* Help */
	std::string help() const;
	std::string error() const;

private:
	std::string _program;
	std::string _description;
	ArgDef      _defs[MAX_DEFS];
	int         _def_count;
	ArgValue    _values[MAX_ARGS];
	int         _value_count;
	std::string _positional[MAX_POS];
	int         _pos_count;
	std::string _error;

	int _find_def(const std::string& flag) const;
	int _find_value(const std::string& key) const;
	void _set_value(const std::string& key, const std::string& val);
};

} /* namespace util */
} /* namespace libcpp */

#endif /* LIBCPP_UTIL_ARGPARSER_HPP */
