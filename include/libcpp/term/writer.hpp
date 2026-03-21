/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   writer.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 20:45:55 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_TERM_WRITER_HPP
# define LIBCPP_TERM_WRITER_HPP

# include <string>
# include <iostream>
# include <map>
# include "libcpp/term/style.hpp"
# include "libcpp/term/table.hpp"

namespace libcpp
{

/*
** TermWriter — markdown parser feeding into TermStyle
** Orthodox Canonical Form compliant.
*/
class TermWriter
{
public:
	/* ── OCF ───────────────────────────────────────────────────── */
	TermWriter();
	explicit TermWriter(TermStyle& ts);
	TermWriter(TermStyle& ts, std::ostream& os);
	TermWriter(const TermWriter& other);
	TermWriter& operator=(const TermWriter& other);
	~TermWriter();

	/* ── API ───────────────────────────────────────────────────── */
	void write(const std::string& markdown);
	void writeln(const std::string& markdown);
	void nl();

	/* ── callout registry ──────────────────────────────────────── */
	void define_callout(const std::string& name, const ElemStyle& style);

private:
	TermStyle*                      _ts;
	std::ostream*                   _os;
	std::map<std::string, ElemStyle> _callouts;
	bool                            _owns_ts;

	void _parse_line(const std::string& line);
	bool _starts_with(const std::string& s, const std::string& prefix) const;
	std::string _trim(const std::string& s) const;
};

} /* namespace libcpp */

#endif /* LIBCPP_TERM_WRITER_HPP */
