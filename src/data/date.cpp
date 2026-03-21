/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   date.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 22:09:50 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/data/date.hpp"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <stdexcept>

namespace libcpp
{
namespace data
{

/* ── Date OCF ───────────────────────────────────────────────────────────── */

Date::Date() : _year(1970), _month(1), _day(1) {}

Date::Date(int year, int month, int day)
	: _year(year), _month(month), _day(day)
{
	if (!is_valid())
		throw std::invalid_argument("Invalid date");
}

Date::Date(const Date& o)
	: _year(o._year), _month(o._month), _day(o._day) {}

Date& Date::operator=(const Date& o)
{
	if (this != &o)
	{
		_year = o._year;
		_month = o._month;
		_day = o._day;
	}
	return *this;
}

Date::~Date() {}

/* ── Factory ────────────────────────────────────────────────────────────── */

Date Date::today()
{
	std::time_t t = std::time(0);
	std::tm* now = std::localtime(&t);
	return Date(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
}

Date Date::from_epoch(long seconds)
{
	std::time_t t = static_cast<std::time_t>(seconds);
	std::tm* gm = std::gmtime(&t);
	return Date(gm->tm_year + 1900, gm->tm_mon + 1, gm->tm_mday);
}

Date Date::parse(const std::string& str, const std::string& fmt)
{
	if (fmt == "YYYY-MM-DD" || fmt == "ISO")
		return _parse_iso(str);
	if (fmt == "DD/MM/YYYY")
		return _parse_dmy(str);
	if (fmt == "MM/DD/YYYY")
		return _parse_mdy(str);
	if (fmt == "YYYY/MM/DD")
		return _parse_ymd(str, '/');
	throw std::invalid_argument("Unknown date format");
}

/* ── Accessors ──────────────────────────────────────────────────────────── */

int Date::year() const  { return _year; }
int Date::month() const { return _month; }
int Date::day() const   { return _day; }

/* ── Validation ─────────────────────────────────────────────────────────── */

bool Date::is_valid() const
{
	if (_month < 1 || _month > 12)
		return false;
	if (_day < 1 || _day > days_in_month(_year, _month))
		return false;
	if (_year < 1)
		return false;
	return true;
}

/* ── Calendar helpers ───────────────────────────────────────────────────── */

int Date::weekday() const
{
	int y = _year;
	int m = _month;
	if (m < 3) { m += 12; y -= 1; }
	int q = _day;
	int K = y % 100;
	int J = y / 100;
	int h = (q + (13 * (m + 1)) / 5 + K + K / 4 + J / 4 - 2 * J) % 7;
	return (h + 6) % 7;
}

int Date::day_of_week() const { return weekday(); }
bool Date::is_weekend() const { int w = weekday(); return w == 0 || w == 6; }
bool Date::is_weekday() const { return !is_weekend(); }

std::string Date::weekday_name() const
{
	static const char* n[] = {
		"Sunday", "Monday", "Tuesday", "Wednesday",
		"Thursday", "Friday", "Saturday"
	};
	return n[weekday()];
}

std::string Date::weekday_short() const
{
	static const char* n[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	return n[weekday()];
}

std::string Date::month_name() const
{
	static const char* n[] = {
		"January", "February", "March", "April", "May", "June",
		"July", "August", "September", "October", "November", "December"
	};
	return (_month >= 1 && _month <= 12) ? n[_month - 1] : "";
}

std::string Date::month_short() const
{
	static const char* n[] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};
	return (_month >= 1 && _month <= 12) ? n[_month - 1] : "";
}

int Date::day_of_year() const
{
	int d = _day;
	for (int m = 1; m < _month; ++m)
		d += days_in_month(_year, m);
	return d;
}

int Date::week_number() const
{
	Date jan1(_year, 1, 1);
	int doy = day_of_year();
	int jan1wd = jan1.weekday();
	int iwd = (weekday() + 6) % 7;
	int ij1 = (jan1wd + 6) % 7;
	int wn = (doy + ij1 - iwd + 6) / 7;
	if (wn == 0)
	{
		Date ly(_year - 1, 12, 31);
		return ly.week_number();
	}
	if (wn == 53)
	{
		Date ny(_year + 1, 1, 1);
		if (ny.weekday() <= 4)
			return 1;
	}
	return wn;
}

/* ── Arithmetic ─────────────────────────────────────────────────────────── */

Date Date::add_days(int n) const
{
	int y = _year;
	int m = _month;
	int d = _day + n;
	while (d > days_in_month(y, m))
	{
		d -= days_in_month(y, m);
		++m;
		if (m > 12) { m = 1; ++y; }
	}
	while (d < 1)
	{
		--m;
		if (m < 1) { m = 12; --y; }
		d += days_in_month(y, m);
	}
	return Date(y, m, d);
}

Date Date::add_months(int n) const
{
	int total = _month + n;
	int y = _year + (total - 1) / 12;
	int m = ((total - 1) % 12) + 1;
	if (m < 1) { m += 12; --y; }
	int d = _day;
	int mx = days_in_month(y, m);
	if (d > mx)
		d = mx;
	return Date(y, m, d);
}

Date Date::add_years(int n) const
{
	int y = _year + n;
	int d = _day;
	if (_month == 2 && _day == 29 && !is_leap_year(y))
		d = 28;
	return Date(y, _month, d);
}

int Date::days_between(const Date& o) const
{
	return _to_julian() - o._to_julian();
}

/* ── Formatting ─────────────────────────────────────────────────────────── */

std::string Date::to_iso() const
{
	std::ostringstream oss;
	oss << std::setfill('0')
		<< std::setw(4) << _year << "-"
		<< std::setw(2) << _month << "-"
		<< std::setw(2) << _day;
	return oss.str();
}

long Date::to_epoch() const
{
	/* Manual calculation — days from 1970-01-01 to this date, then *86400 */
	int y = _year;
	int m = _month;
	int d = _day;
	/* Adjust so March=1 for easier leap handling */
	if (m <= 2) { y--; m += 12; }
	long era_days = 365L * (y - 1970);
	/* add leap days from 1970 to y */
	era_days += (y / 4 - 1970 / 4) - (y / 100 - 1970 / 100) + (y / 400 - 1970 / 400);
	/* add days for months March..m (shifted calendar) */
	static const int cum[] = {0, 31, 61, 92, 122, 153, 184, 214, 245, 275, 306, 337};
	era_days += cum[m - 3];
	era_days += d - 1;
	/* Un-shift: we shifted so year starts in March. Add Jan+Feb days back */
	/* Actually simpler: use Julian day approach */
	/* Rewrite with _to_julian */
	long jd = static_cast<long>(_to_julian());
	long epoch_jd = 2440588L; /* JD of 1970-01-01 */
	return (jd - epoch_jd) * 86400L;
}

std::string Date::to_string(const std::string& fmt) const
{
	if (fmt == "YYYY-MM-DD" || fmt == "ISO")
		return to_iso();
	if (fmt == "DD/MM/YYYY")
	{
		std::ostringstream o;
		o << std::setfill('0') << std::setw(2) << _day << "/"
		  << std::setw(2) << _month << "/" << std::setw(4) << _year;
		return o.str();
	}
	if (fmt == "MM/DD/YYYY")
	{
		std::ostringstream o;
		o << std::setfill('0') << std::setw(2) << _month << "/"
		  << std::setw(2) << _day << "/" << std::setw(4) << _year;
		return o.str();
	}
	if (fmt == "Month DD, YYYY")
	{
		std::ostringstream o;
		o << month_name() << " " << _day << ", " << _year;
		return o.str();
	}
	return to_iso();
}

/* ── Comparison ─────────────────────────────────────────────────────────── */

bool Date::operator==(const Date& o) const
{ return _year == o._year && _month == o._month && _day == o._day; }

bool Date::operator!=(const Date& o) const { return !(*this == o); }

bool Date::operator<(const Date& o) const
{
	if (_year != o._year)  return _year < o._year;
	if (_month != o._month) return _month < o._month;
	return _day < o._day;
}

bool Date::operator<=(const Date& o) const { return *this < o || *this == o; }
bool Date::operator>(const Date& o) const  { return !(*this <= o); }
bool Date::operator>=(const Date& o) const { return !(*this < o); }

Date  Date::operator+(int d) const   { return add_days(d); }
Date  Date::operator-(int d) const   { return add_days(-d); }
int   Date::operator-(const Date& o) const { return days_between(o); }
Date& Date::operator+=(int d) { *this = add_days(d); return *this; }
Date& Date::operator-=(int d) { *this = add_days(-d); return *this; }

/* ── Static utilities ───────────────────────────────────────────────────── */

bool Date::is_leap_year(int y)
{ return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0); }

int Date::days_in_month(int y, int m)
{
	static const int d[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if (m < 1 || m > 12) return 0;
	if (m == 2 && is_leap_year(y)) return 29;
	return d[m - 1];
}

int Date::days_in_year(int y) { return is_leap_year(y) ? 366 : 365; }

/* ── Private helpers ────────────────────────────────────────────────────── */

int Date::_to_julian() const
{
	int a = (14 - _month) / 12;
	int y = _year + 4800 - a;
	int m = _month + 12 * a - 3;
	return _day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
}

Date Date::_parse_iso(const std::string& s)
{
	if (s.length() < 10)
		throw std::invalid_argument("Invalid ISO date");
	int y = std::atoi(s.substr(0, 4).c_str());
	int m = std::atoi(s.substr(5, 2).c_str());
	int d = std::atoi(s.substr(8, 2).c_str());
	return Date(y, m, d);
}

Date Date::_parse_dmy(const std::string& s)
{
	std::string::size_type s1 = s.find_first_of("/-");
	std::string::size_type s2 = s.find_first_of("/-", s1 + 1);
	if (s1 == std::string::npos || s2 == std::string::npos)
		throw std::invalid_argument("Invalid date format");
	int d = std::atoi(s.substr(0, s1).c_str());
	int m = std::atoi(s.substr(s1 + 1, s2 - s1 - 1).c_str());
	int y = std::atoi(s.substr(s2 + 1).c_str());
	return Date(y, m, d);
}

Date Date::_parse_mdy(const std::string& s)
{
	std::string::size_type s1 = s.find_first_of("/-");
	std::string::size_type s2 = s.find_first_of("/-", s1 + 1);
	if (s1 == std::string::npos || s2 == std::string::npos)
		throw std::invalid_argument("Invalid date format");
	int m = std::atoi(s.substr(0, s1).c_str());
	int d = std::atoi(s.substr(s1 + 1, s2 - s1 - 1).c_str());
	int y = std::atoi(s.substr(s2 + 1).c_str());
	return Date(y, m, d);
}

Date Date::_parse_ymd(const std::string& s, char sep)
{
	std::string::size_type s1 = s.find(sep);
	std::string::size_type s2 = s.find(sep, s1 + 1);
	if (s1 == std::string::npos || s2 == std::string::npos)
		throw std::invalid_argument("Invalid date format");
	int y = std::atoi(s.substr(0, s1).c_str());
	int m = std::atoi(s.substr(s1 + 1, s2 - s1 - 1).c_str());
	int d = std::atoi(s.substr(s2 + 1).c_str());
	return Date(y, m, d);
}

/* ── DateRange ──────────────────────────────────────────────────────────── */

DateRange::DateRange() : _start(), _end() {}

DateRange::DateRange(const Date& start, const Date& end)
	: _start(start), _end(end)
{
	if (end < start)
		throw std::invalid_argument("End date must be after start date");
}

DateRange::DateRange(const DateRange& o)
	: _start(o._start), _end(o._end) {}

DateRange& DateRange::operator=(const DateRange& o)
{
	if (this != &o)
	{
		_start = o._start;
		_end = o._end;
	}
	return *this;
}

DateRange::~DateRange() {}

const Date& DateRange::start() const    { return _start; }
const Date& DateRange::end_date() const { return _end; }
int  DateRange::days() const            { return _end.days_between(_start); }

bool DateRange::contains(const Date& d) const
{ return d >= _start && d <= _end; }

bool DateRange::overlaps(const DateRange& o) const
{ return _start <= o._end && _end >= o._start; }

/* ── Free utilities ─────────────────────────────────────────────────────── */

Date first_of_month(int y, int m) { return Date(y, m, 1); }
Date last_of_month(int y, int m)  { return Date(y, m, Date::days_in_month(y, m)); }
Date first_of_year(int y)         { return Date(y, 1, 1); }
Date last_of_year(int y)          { return Date(y, 12, 31); }

bool is_weekend(const Date& d) { int w = d.weekday(); return w == 0 || w == 6; }
bool is_weekday(const Date& d) { return !is_weekend(d); }
int  quarter(const Date& d)    { return (d.month() - 1) / 3 + 1; }

} /* namespace data */
} /* namespace libcpp */
