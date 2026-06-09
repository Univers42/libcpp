/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   csv_writer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/09 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP98_CSV_WRITER_HPP
#define LIBCPP98_CSV_WRITER_HPP

#include <fstream>
#include <string>
#include <vector>

namespace libcpp98 {

/* ── Streaming append-only CSV writer (RFC 4180 escaping) ──────────────────
**
** Unlike an in-memory CSV document, this writes (and flushes) one row at a
** time — suited to audit trails and long-running logs where the file must
** survive a crash and never be held in RAM.
*/
class CsvWriter {
public:
	CsvWriter();
	~CsvWriter();

	/* Opens `path` in append mode.  Returns false on failure. */
	bool				open(const std::string &path);

	/* True when the file was missing or empty before open() — the cue to
	** write a header row exactly once. */
	bool				isNewFile() const;

	bool				ok() const;
	void				close();

	/* Writes one escaped row terminated by '\n', then flushes. */
	void				row(const std::vector<std::string> &fields);

	/* RFC 4180: quote when the field holds ',', '"', CR or LF; double
	** embedded quotes. */
	static std::string	escape(const std::string &field);

private:
	std::ofstream	_out;
	bool			_wasEmpty;

	CsvWriter(const CsvWriter &);
	CsvWriter &operator=(const CsvWriter &);
};

} /* namespace libcpp98 */

#endif /* LIBCPP98_CSV_WRITER_HPP */
