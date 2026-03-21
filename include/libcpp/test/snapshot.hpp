/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   snapshot.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:27:50 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_TEST_SNAPSHOT_HPP
# define LIBCPP_TEST_SNAPSHOT_HPP

# include <string>
# include <fstream>
# include <sstream>

namespace libcpp
{
namespace test
{

/* ── DiffLine — a single line in a diff result ─────────────────────────── */

struct DiffLine
{
	enum Kind { KEEP, ADD, DEL };

	Kind        kind;
	std::string text;

	DiffLine();
	DiffLine(Kind k, const std::string& t);
	DiffLine(const DiffLine& o);
	DiffLine& operator=(const DiffLine& o);
	~DiffLine();
};

/* ── Diff — collection of diff lines ───────────────────────────────────── */

struct Diff
{
	static const int MAX_LINES = 4096;

	DiffLine lines[MAX_LINES];
	int      count;
	bool     equal;

	Diff();
	Diff(const Diff& o);
	Diff& operator=(const Diff& o);
	~Diff();

	std::string to_string() const;
};

/* ── Myers diff engine ─────────────────────────────────────────────────── */

Diff myers_diff(const std::string& old_text, const std::string& new_text);

/* ── Snapshot — file-backed snapshot comparison ────────────────────────── */

class Snapshot
{
public:
	Snapshot();
	Snapshot(const std::string& snap_dir);
	Snapshot(const Snapshot& o);
	Snapshot& operator=(const Snapshot& o);
	~Snapshot();

	/*
	** match() loads/creates the snapshot file for `name`.
	** If no snapshot exists, creates it with `actual` and returns true.
	** If snapshot exists and matches, returns true.
	** If mismatch and update mode, overwrites and returns true.
	** If mismatch and no update, returns false and stores the diff.
	*/
	bool match(const std::string& name, const std::string& actual);

	/* Access last diff result */
	const Diff& last_diff() const;

	/* Enable/disable update mode (overwrites on mismatch) */
	void set_update(bool on);
	bool update_mode() const;

	/* Enable/disable strip-ANSI mode (removes escape sequences before compare) */
	void set_strip_ansi(bool on);
	bool strip_ansi_mode() const;

private:
	std::string _snap_dir;
	Diff        _last_diff;
	bool        _update;
	bool        _strip_ansi;

	std::string _path(const std::string& name) const;
	std::string _read_file(const std::string& path) const;
	void        _write_file(const std::string& path, const std::string& data) const;
};

/* ── Snapshot assertion macro ──────────────────────────────────────────── */

# define ASSERT_SNAPSHOT(suite, snap, name, actual) \
	do { \
		if (!(snap).match((name), (actual))) { \
			std::string _diff_str = (snap).last_diff().to_string(); \
			(suite).assert_true(false, _diff_str.c_str(), __FILE__, __LINE__); \
		} \
	} while (0)

} /* namespace test */
} /* namespace libcpp */

#endif /* LIBCPP_TEST_SNAPSHOT_HPP */
