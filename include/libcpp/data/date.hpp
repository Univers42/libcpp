/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   date.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:33:17 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_DATA_DATE_HPP
# define LIBCPP_DATA_DATE_HPP

# include <string>

namespace libcpp
{
namespace data
{

/* ── Date — calendar date with arithmetic & formatting ─────────────────── */

class Date
{
public:
	/* OCF */
	Date();
	Date(int year, int month, int day);
	Date(const Date& o);
	Date& operator=(const Date& o);
	~Date();

	/* Factory */
	static Date today();
	static Date parse(const std::string& str, const std::string& fmt = "YYYY-MM-DD");

	/* Accessors */
	int year() const;
	int month() const;
	int day() const;

	/* Validation */
	bool is_valid() const;

	/* Calendar helpers */
	int  weekday() const;               /* 0=Sun … 6=Sat */
	int  day_of_week() const;           /* alias for weekday() */
	std::string weekday_name() const;
	std::string weekday_short() const;
	std::string month_name() const;
	std::string month_short() const;
	int  day_of_year() const;
	int  week_number() const;

	/* Arithmetic */
	Date add_days(int n) const;
	Date add_months(int n) const;
	Date add_years(int n) const;
	int  days_between(const Date& o) const;

	/* Formatting */
	std::string to_string(const std::string& fmt = "YYYY-MM-DD") const;
	std::string to_iso() const;

	/* Comparison */
	bool operator==(const Date& o) const;
	bool operator!=(const Date& o) const;
	bool operator<(const Date& o) const;
	bool operator<=(const Date& o) const;
	bool operator>(const Date& o) const;
	bool operator>=(const Date& o) const;

	/* Operators */
	Date  operator+(int days) const;
	Date  operator-(int days) const;
	int   operator-(const Date& o) const;
	Date& operator+=(int days);
	Date& operator-=(int days);

	/* Static utilities */
	static bool is_leap_year(int y);
	static int  days_in_month(int y, int m);
	static int  days_in_year(int y);

private:
	int _year;
	int _month;
	int _day;

	int  _to_julian() const;
	static Date _parse_iso(const std::string& s);
	static Date _parse_dmy(const std::string& s);
	static Date _parse_mdy(const std::string& s);
	static Date _parse_ymd(const std::string& s, char sep);
};

/* ── DateRange ─────────────────────────────────────────────────────────── */

class DateRange
{
public:
	DateRange();
	DateRange(const Date& start, const Date& end);
	DateRange(const DateRange& o);
	DateRange& operator=(const DateRange& o);
	~DateRange();

	const Date& start() const;
	const Date& end_date() const;
	int  days() const;
	bool contains(const Date& d) const;
	bool overlaps(const DateRange& o) const;

private:
	Date _start;
	Date _end;
};

/* ── free utilities ────────────────────────────────────────────────────── */

Date first_of_month(int year, int month);
Date last_of_month(int year, int month);
Date first_of_year(int year);
Date last_of_year(int year);
bool is_weekend(const Date& d);
bool is_weekday(const Date& d);
int  quarter(const Date& d);

} /* namespace data */
} /* namespace libcpp */

#endif /* LIBCPP_DATA_DATE_HPP */
