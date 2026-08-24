/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   date.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 22:06:08 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_DATA_DATE_HPP
#define LIBCPP_DATA_DATE_HPP

#include <string>

namespace libcpp {
namespace data {

/* ── Date — calendar date with arithmetic& formatting ─────────────────── */

class Date {
 public:
  /* OCF */
  Date();
  Date(int year, int month, int day);
  Date(const Date& o);
  Date& operator=(const Date& o);
  ~Date();

  /* Factory */
  static Date today();
  static Date parse(const std::string& str,
                    const std::string& fmt = "YYYY-MM-DD");
  static Date from_epoch(long seconds);

  /* Accessors */
  int year() const;
  int month() const;
  int day() const;

  /* Validation */
  bool is_valid() const;

  /* Calendar helpers */
  int weekday() const;     /* 0=Sun … 6=Sat */
  int day_of_week() const; /* alias for weekday() */
  bool is_weekend() const;
  bool is_weekday() const;
  std::string weekday_name() const;
  std::string weekday_short() const;
  std::string month_name() const;
  std::string month_short() const;
  int day_of_year() const;
  int week_number() const;

  /* Arithmetic */
  Date add_days(int n) const;
  Date add_months(int n) const;
  Date add_years(int n) const;
  int days_between(const Date& o) const;

  /* Formatting */
  std::string to_string(const std::string& fmt = "YYYY-MM-DD") const;
  std::string to_iso() const;
  long to_epoch() const;

  /* Comparison */
  bool operator==(const Date& o) const;
  bool operator!=(const Date& o) const;
  bool operator<(const Date& o) const;
  bool operator<=(const Date& o) const;
  bool operator>(const Date& o) const;
  bool operator>=(const Date& o) const;

  /* Operators */
  Date operator+(int days) const;
  Date operator-(int days) const;
  int operator-(const Date& o) const;
  Date& operator+=(int days);
  Date& operator-=(int days);

  /* Static utilities */
  static bool is_leap_year(int y);
  static int days_in_month(int y, int m);
  static int days_in_year(int y);

 private:
  int _year;
  int _month;
  int _day;

  int _to_julian() const;
  static Date _parse_iso(const std::string& s);
  static Date _parse_dmy(const std::string& s);
  static Date _parse_mdy(const std::string& s);
  static Date _parse_ymd(const std::string& s, char sep);
};

/* ── DateRange ─────────────────────────────────────────────────────────── */

class DateRange {
 public:
  DateRange();
  DateRange(const Date& start, const Date& end);
  DateRange(const DateRange& o);
  DateRange& operator=(const DateRange& o);
  ~DateRange();

  const Date& start() const;
  const Date& end_date() const;
  int days() const;
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
int quarter(const Date& d);

/* ── Wall-clock formatting ─────────────────────────────────────────────── */
/*
** strftime over localtime(time(NULL)).  These exist because "what time is it
** right now, as a string" is the one thing Date cannot answer: Date carries
** day resolution, and a log line or an audit row needs seconds.
**
** format_now returns an empty string when localtime fails or when fmt does
** not fit in 63 bytes.  time_hms substitutes a same-width placeholder
** instead, so a log column never changes width mid-stream; timestamp_iso
** returns empty rather than inventing a plausible-looking time, because a
** wrong timestamp in an audit trail is worse than a missing one.
*/
std::string format_now(const char* fmt);
std::string time_hms();      /* "15:04:05"  ->  "--:--:--" on failure */
std::string timestamp_iso(); /* "2026-08-24T15:04:05"  ->  "" on failure */

} /* namespace data */
} /* namespace libcpp */

#endif /* LIBCPP_DATA_DATE_HPP */
