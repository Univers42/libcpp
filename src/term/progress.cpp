/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   progress.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 20:45:26 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/term/progress.hpp"
#include "libcpp/term/style.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace libcpp
{

/* ══════════════════════════════════════════════════════════════════════════
 *  ProgressStyle
 * ═════════════════════════════════════════════════════════════════════════ */

ProgressStyle::ProgressStyle()
	: fill("\xe2\x96\x88"), empty("\xe2\x96\x91"),
	  left("["), right("]"),
	  fill_color(0, 200, 100), label_color(200, 200, 200),
	  width(40), show_percent(true), show_eta(true) {}

ProgressStyle::ProgressStyle(const ProgressStyle& o)
	: fill(o.fill), empty(o.empty), left(o.left), right(o.right),
	  fill_color(o.fill_color), label_color(o.label_color),
	  width(o.width), show_percent(o.show_percent), show_eta(o.show_eta) {}

ProgressStyle& ProgressStyle::operator=(const ProgressStyle& o)
{
	if (this != &o)
	{
		fill = o.fill; empty = o.empty; left = o.left; right = o.right;
		fill_color = o.fill_color; label_color = o.label_color;
		width = o.width; show_percent = o.show_percent; show_eta = o.show_eta;
	}
	return *this;
}

ProgressStyle::~ProgressStyle() {}

/* ══════════════════════════════════════════════════════════════════════════
 *  ProgressBar
 * ═════════════════════════════════════════════════════════════════════════ */

ProgressBar::ProgressBar()
	: _total(100), _current(0), _start(std::clock()) {}

ProgressBar::ProgressBar(std::size_t total, const std::string& label)
	: _total(total), _current(0), _label(label), _start(std::clock()) {}

ProgressBar::ProgressBar(std::size_t total, const ProgressStyle& style)
	: _total(total), _current(0), _style(style), _start(std::clock()) {}

ProgressBar::ProgressBar(const ProgressBar& o)
	: _total(o._total), _current(o._current), _style(o._style),
	  _label(o._label), _postfix(o._postfix), _start(o._start) {}

ProgressBar& ProgressBar::operator=(const ProgressBar& o)
{
	if (this != &o)
	{
		_total = o._total; _current = o._current; _style = o._style;
		_label = o._label; _postfix = o._postfix; _start = o._start;
	}
	return *this;
}

ProgressBar::~ProgressBar() {}

void ProgressBar::tick(std::size_t n)
{
	_current += n;
	if (_current > _total) _current = _total;
}

void ProgressBar::set(std::size_t n)
{
	_current = (n > _total) ? _total : n;
}

void ProgressBar::finish() { _current = _total; }

void ProgressBar::set_label(const std::string& label) { _label = label; }
void ProgressBar::set_postfix(const std::string& text) { _postfix = text; }

double ProgressBar::percent() const
{
	if (_total == 0) return 100.0;
	return 100.0 * _current / _total;
}

double ProgressBar::rate() const
{
	double elapsed = (double)(std::clock() - _start) / CLOCKS_PER_SEC;
	if (elapsed <= 0.0) return 0.0;
	return _current / elapsed;
}

double ProgressBar::eta_seconds() const
{
	double r = rate();
	if (r <= 0.0) return 0.0;
	return (_total - _current) / r;
}

std::string ProgressBar::render() const
{
	std::ostringstream out;

	if (!_label.empty())
		out << _style.label_color.to_ansi_fg() << _label << " " << TermUtils::reset();

	int filled = 0;
	if (_total > 0)
		filled = (int)(_style.width * _current / _total);
	int empty_count = _style.width - filled;
	if (empty_count < 0) empty_count = 0;

	out << _style.left;
	out << _style.fill_color.to_ansi_fg();
	for (int i = 0; i < filled; ++i)
		out << _style.fill;
	out << TermUtils::reset();
	for (int i = 0; i < empty_count; ++i)
		out << _style.empty;
	out << _style.right;

	if (_style.show_percent)
	{
		out << " ";
		out << std::fixed << std::setprecision(1) << percent() << "%";
	}

	if (_style.show_eta && _current > 0 && _current < _total)
	{
		double eta = eta_seconds();
		out << " ETA: " << (int)eta << "s";
	}

	if (!_postfix.empty())
		out << " " << _postfix;

	return out.str();
}

void ProgressBar::print_inline() const
{
	std::cout << "\r" << render() << std::flush;
}

void ProgressBar::println() const
{
	std::cout << render() << std::endl;
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Spinner
 * ═════════════════════════════════════════════════════════════════════════ */

Spinner::Spinner()
	: _kind(DOTS), _frame(0), _done(false) {}

Spinner::Spinner(const std::string& label, Kind kind)
	: _label(label), _kind(kind), _frame(0), _done(false) {}

Spinner::Spinner(const Spinner& o)
	: _label(o._label), _kind(o._kind), _frame(o._frame),
	  _done(o._done), _final_msg(o._final_msg) {}

Spinner& Spinner::operator=(const Spinner& o)
{
	if (this != &o)
	{
		_label = o._label; _kind = o._kind; _frame = o._frame;
		_done = o._done; _final_msg = o._final_msg;
	}
	return *this;
}

Spinner::~Spinner() {}

void Spinner::spin()
{
	if (!_done)
	{
		std::cout << "\r" << render() << std::flush;
		++_frame;
	}
}

void Spinner::done(const std::string& final_msg)
{
	_done = true;
	_final_msg = final_msg;
	std::cout << "\r" << render() << std::endl;
}

void Spinner::update_label(const std::string& label) { _label = label; }

std::string Spinner::render() const
{
	if (_done)
		return _final_msg.empty() ? _label + " done" : _final_msg;
	std::size_t total = 0;
	const char* f = _frames(_kind, _frame, total);
	std::string out;
	out += f;
	out += " ";
	out += _label;
	return out;
}

const char* Spinner::_frames(Kind k, std::size_t frame, std::size_t& total)
{
	static const char* dots[]   = {"\xe2\xa0\x8b", "\xe2\xa0\x99", "\xe2\xa0\xb9",
		"\xe2\xa0\xb8", "\xe2\xa0\xbc", "\xe2\xa0\xb4", "\xe2\xa0\xa6",
		"\xe2\xa0\xa7", "\xe2\xa0\x87", "\xe2\xa0\x8f"};
	static const char* line[]   = {"-", "\\", "|", "/"};
	static const char* arc[]    = {"\xe2\x97\x9c", "\xe2\x97\x9d", "\xe2\x97\x9e", "\xe2\x97\x9f"};
	static const char* arrow[]  = {"\xe2\x86\x90", "\xe2\x86\x91", "\xe2\x86\x92", "\xe2\x86\x93"};
	static const char* bounce[] = {"[=   ]", "[ =  ]", "[  = ]", "[   =]", "[  = ]", "[ =  ]"};

	switch (k)
	{
		case DOTS:   total = 10; return dots[frame % 10];
		case LINE:   total = 4;  return line[frame % 4];
		case ARC:    total = 4;  return arc[frame % 4];
		case ARROW:  total = 4;  return arrow[frame % 4];
		case BOUNCE: total = 6;  return bounce[frame % 6];
		default:     total = 4;  return line[frame % 4];
	}
}

} /* namespace libcpp */
