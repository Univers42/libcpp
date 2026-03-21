/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   progress.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 20:45:27 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_TERM_PROGRESS_HPP
# define LIBCPP_TERM_PROGRESS_HPP

# include <string>
# include <ctime>
# include "libcpp/term/color.hpp"

namespace libcpp
{

/*
** ProgressStyle — visual configuration for progress bar
*/
struct ProgressStyle
{
	std::string fill;
	std::string empty;
	std::string left;
	std::string right;
	Srgb        fill_color;
	Srgb        label_color;
	int         width;
	bool        show_percent;
	bool        show_eta;

	ProgressStyle();
	ProgressStyle(const ProgressStyle& other);
	ProgressStyle& operator=(const ProgressStyle& other);
	~ProgressStyle();
};

/*
** ProgressBar — terminal progress visualization
** Orthodox Canonical Form compliant.
*/
class ProgressBar
{
public:
	/* ── OCF ───────────────────────────────────────────────────── */
	ProgressBar();
	ProgressBar(std::size_t total, const std::string& label = "");
	ProgressBar(std::size_t total, const ProgressStyle& style);
	ProgressBar(const ProgressBar& other);
	ProgressBar& operator=(const ProgressBar& other);
	~ProgressBar();

	/* ── control ───────────────────────────────────────────────── */
	void tick(std::size_t n = 1);
	void set(std::size_t n);
	void finish();
	void set_label(const std::string& label);
	void set_postfix(const std::string& text);

	/* ── queries ───────────────────────────────────────────────── */
	double percent() const;
	double rate() const;
	double eta_seconds() const;
	bool   is_complete() const;
	std::size_t current() const;
	std::size_t total() const;
	void   set_width(int w);

	/* ── output ────────────────────────────────────────────────── */
	std::string render() const;
	void print_inline() const;
	void println() const;

private:
	std::size_t     _total;
	std::size_t     _current;
	ProgressStyle   _style;
	std::string     _label;
	std::string     _postfix;
	std::clock_t    _start;
};

/*
** Spinner — simple text spinner (non-threaded for C++98)
** Call spin() in a loop to advance the animation frame.
*/
class Spinner
{
public:
	enum Kind { DOTS = 0, LINE = 1, ARC = 2, ARROW = 3, BOUNCE = 4 };

	/* ── OCF ───────────────────────────────────────────────────── */
	Spinner();
	Spinner(const std::string& label, Kind kind = DOTS);
	Spinner(const Spinner& other);
	Spinner& operator=(const Spinner& other);
	~Spinner();

	void spin();
	void done(const std::string& final_msg = "");
	void update_label(const std::string& label);
	std::string render() const;

private:
	std::string _label;
	Kind        _kind;
	std::size_t _frame;
	bool        _done;
	std::string _final_msg;

	static const char* _frames(Kind k, std::size_t frame, std::size_t& total);
};

} /* namespace libcpp */

#endif /* LIBCPP_TERM_PROGRESS_HPP */
