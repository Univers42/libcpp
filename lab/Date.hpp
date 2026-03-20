/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Date.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 01:56:01 by marvin            #+#    #+#             */
/*   Updated: 2025/12/24 01:56:01 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DATE_HPP
#define DATE_HPP

#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <stdexcept>
#include <vector>
#include <map>
#include <cstdlib>

// ============================================================================
// DATE CLASS - COMPREHENSIVE DATE HANDLING
// ============================================================================

class Date {
public:
    // Constructors
    Date() : _year(1970), _month(1), _day(1) {}
    
    Date(int year, int month, int day) 
        : _year(year), _month(month), _day(day) {
        if (!isValid()) {
            throw std::invalid_argument("Invalid date: " + toString());
        }
    }
    
    // Parse from string
    static Date parse(const std::string& str, const std::string& format = "YYYY-MM-DD") {
        if (format == "YYYY-MM-DD" || format == "ISO") {
            return parseISO(str);
        } else if (format == "DD/MM/YYYY") {
            return parseDMY(str);
        } else if (format == "MM/DD/YYYY") {
            return parseMDY(str);
        } else if (format == "YYYY/MM/DD") {
            return parseYMD(str, '/');
        }
        throw std::invalid_argument("Unknown format: " + format);
    }
    
    // Get current date
    static Date today() {
        std::time_t t = std::time(NULL);
        std::tm* now = std::localtime(&t);
        return Date(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
    }
    
    // Accessors
    int year() const { return _year; }
    int month() const { return _month; }
    int day() const { return _day; }
    
    void setYear(int year) { _year = year; validate(); }
    void setMonth(int month) { _month = month; validate(); }
    void setDay(int day) { _day = day; validate(); }
    
    // Validation
    bool isValid() const {
        if (_month < 1 || _month > 12) return false;
        if (_day < 1 || _day > daysInMonth(_year, _month)) return false;
        if (_year < 1) return false;
        return true;
    }
    
    // Weekday (0 = Sunday, 1 = Monday, ..., 6 = Saturday)
    int weekday() const {
        // Zeller's congruence
        int y = _year;
        int m = _month;
        if (m < 3) {
            m += 12;
            y -= 1;
        }
        int q = _day;
        int K = y % 100;
        int J = y / 100;
        
        int h = (q + (13 * (m + 1)) / 5 + K + K / 4 + J / 4 - 2 * J) % 7;
        // Convert to 0 = Sunday
        return (h + 6) % 7;
    }
    
    std::string weekdayName() const {
        static const char* names[] = {
            "Sunday", "Monday", "Tuesday", "Wednesday", 
            "Thursday", "Friday", "Saturday"
        };
        return names[weekday()];
    }
    
    std::string weekdayShort() const {
        static const char* names[] = {
            "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
        };
        return names[weekday()];
    }
    
    // Month name
    std::string monthName() const {
        static const char* names[] = {
            "January", "February", "March", "April", "May", "June",
            "July", "August", "September", "October", "November", "December"
        };
        return (_month >= 1 && _month <= 12) ? names[_month - 1] : "";
    }
    
    std::string monthShort() const {
        static const char* names[] = {
            "Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
        };
        return (_month >= 1 && _month <= 12) ? names[_month - 1] : "";
    }
    
    // Day of year (1-365/366)
    int dayOfYear() const {
        int days = _day;
        for (int m = 1; m < _month; ++m) {
            days += daysInMonth(_year, m);
        }
        return days;
    }
    
    // Week number (ISO 8601)
    int weekNumber() const {
        Date jan1(_year, 1, 1);
        int dayOfYear = this->dayOfYear();
        int jan1Weekday = jan1.weekday();
        
        // Adjust for ISO week (Monday = 0)
        int isoWeekday = (weekday() + 6) % 7;
        int isoJan1Weekday = (jan1Weekday + 6) % 7;
        
        int weekNum = (dayOfYear + isoJan1Weekday - isoWeekday + 6) / 7;
        
        if (weekNum == 0) {
            // Belongs to last week of previous year
            Date lastYear(_year - 1, 12, 31);
            return lastYear.weekNumber();
        }
        
        if (weekNum == 53) {
            // Check if this is really week 53 or week 1 of next year
            Date jan1NextYear(_year + 1, 1, 1);
            if (jan1NextYear.weekday() <= 4) {
                return 1;
            }
        }
        
        return weekNum;
    }
    
    // Date arithmetic
    Date addDays(int days) const {
        int y = _year, m = _month, d = _day + days;
        
        while (d > daysInMonth(y, m)) {
            d -= daysInMonth(y, m);
            ++m;
            if (m > 12) {
                m = 1;
                ++y;
            }
        }
        
        while (d < 1) {
            --m;
            if (m < 1) {
                m = 12;
                --y;
            }
            d += daysInMonth(y, m);
        }
        
        return Date(y, m, d);
    }
    
    Date subtractDays(int days) const {
        return addDays(-days);
    }
    
    Date addMonths(int months) const {
        int totalMonths = _month + months;
        int y = _year + (totalMonths - 1) / 12;
        int m = ((totalMonths - 1) % 12) + 1;
        
        int d = _day;
        int maxDay = daysInMonth(y, m);
        if (d > maxDay) d = maxDay;
        
        return Date(y, m, d);
    }
    
    Date subtractMonths(int months) const {
        return addMonths(-months);
    }
    
    Date addYears(int years) const {
        int y = _year + years;
        int d = _day;
        
        // Handle leap year edge case (Feb 29)
        if (_month == 2 && _day == 29 && !isLeapYear(y)) {
            d = 28;
        }
        
        return Date(y, _month, d);
    }
    
    Date subtractYears(int years) const {
        return addYears(-years);
    }
    
    // Difference in days
    int daysBetween(const Date& other) const {
        return toJulianDay() - other.toJulianDay();
    }
    
    // Comparison operators
    bool operator==(const Date& other) const {
        return _year == other._year && _month == other._month && _day == other._day;
    }
    
    bool operator!=(const Date& other) const {
        return !(*this == other);
    }
    
    bool operator<(const Date& other) const {
        if (_year != other._year) return _year < other._year;
        if (_month != other._month) return _month < other._month;
        return _day < other._day;
    }
    
    bool operator<=(const Date& other) const {
        return *this < other || *this == other;
    }
    
    bool operator>(const Date& other) const {
        return !(*this <= other);
    }
    
    bool operator>=(const Date& other) const {
        return !(*this < other);
    }
    
    // Arithmetic operators
    Date operator+(int days) const {
        return addDays(days);
    }
    
    Date operator-(int days) const {
        return subtractDays(days);
    }
    
    int operator-(const Date& other) const {
        return daysBetween(other);
    }
    
    Date& operator+=(int days) {
        *this = addDays(days);
        return *this;
    }
    
    Date& operator-=(int days) {
        *this = subtractDays(days);
        return *this;
    }
    
    // Formatting
    std::string toString(const std::string& format = "YYYY-MM-DD") const {
        if (format == "YYYY-MM-DD" || format == "ISO") {
            return toISO();
        } else if (format == "DD/MM/YYYY") {
            return toDMY();
        } else if (format == "MM/DD/YYYY") {
            return toMDY();
        } else if (format == "YYYY/MM/DD") {
            return toYMD('/');
        } else if (format == "Month DD, YYYY") {
            std::ostringstream oss; oss << monthName() << " " << _day << ", " << _year; return oss.str();
        } else if (format == "DD Month YYYY") {
            std::ostringstream oss; oss << _day << " " << monthName() << " " << _year; return oss.str();
        } else if (format == "Weekday, Month DD, YYYY") {
            std::ostringstream oss; oss << weekdayName() << ", " << monthName() << " " << _day << ", " << _year; return oss.str();
        }
        return toISO();
    }
    
    std::string toISO() const {
        std::ostringstream oss;
        oss << std::setfill('0') 
            << std::setw(4) << _year << "-"
            << std::setw(2) << _month << "-"
            << std::setw(2) << _day;
        return oss.str();
    }
    
    // Static helper functions
    static bool isLeapYear(int year) {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }
    
    static int daysInMonth(int year, int month) {
        static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (month < 1 || month > 12) return 0;
        if (month == 2 && isLeapYear(year)) return 29;
        return days[month - 1];
    }
    
    static int daysInYear(int year) {
        return isLeapYear(year) ? 366 : 365;
    }

private:
    int _year;
    int _month;
    int _day;
    
    void validate() {
        if (!isValid()) {
            throw std::invalid_argument("Invalid date");
        }
    }
    
    // Convert to Julian day number for calculations
    int toJulianDay() const {
        int a = (14 - _month) / 12;
        int y = _year + 4800 - a;
        int m = _month + 12 * a - 3;
        
        return _day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
    }
    
    // Parsing helpers
    static Date parseISO(const std::string& str) {
        // Format: YYYY-MM-DD
        if (str.length() < 10) throw std::invalid_argument("Invalid ISO date");
        int year = atoi(str.substr(0,4).c_str());
        int month = atoi(str.substr(5,2).c_str());
        int day = atoi(str.substr(8,2).c_str());
        return Date(year, month, day);
    }
    
    static Date parseDMY(const std::string& str) {
        // Format: DD/MM/YYYY or DD-MM-YYYY
        size_t sep1 = str.find_first_of("/-");
        size_t sep2 = str.find_first_of("/-", sep1 + 1);
        if (sep1 == std::string::npos || sep2 == std::string::npos) throw std::invalid_argument("Invalid date format");
        int day = atoi(str.substr(0, sep1).c_str());
        int month = atoi(str.substr(sep1 + 1, sep2 - sep1 - 1).c_str());
        int year = atoi(str.substr(sep2 + 1).c_str());
        return Date(year, month, day);
    }
    
    static Date parseMDY(const std::string& str) {
        // Format: MM/DD/YYYY
        size_t sep1 = str.find_first_of("/-");
        size_t sep2 = str.find_first_of("/-", sep1 + 1);
        if (sep1 == std::string::npos || sep2 == std::string::npos) throw std::invalid_argument("Invalid date format");
        int month = atoi(str.substr(0, sep1).c_str());
        int day = atoi(str.substr(sep1 + 1, sep2 - sep1 - 1).c_str());
        int year = atoi(str.substr(sep2 + 1).c_str());
        return Date(year, month, day);
    }
    
    static Date parseYMD(const std::string& str, char sep) {
        size_t sep1 = str.find(sep);
        size_t sep2 = str.find(sep, sep1 + 1);
        if (sep1 == std::string::npos || sep2 == std::string::npos) throw std::invalid_argument("Invalid date format");
        int year = atoi(str.substr(0, sep1).c_str());
        int month = atoi(str.substr(sep1 + 1, sep2 - sep1 - 1).c_str());
        int day = atoi(str.substr(sep2 + 1).c_str());
        return Date(year, month, day);
    }
    
    std::string toDMY() const {
        std::ostringstream oss;
        oss << std::setfill('0') 
            << std::setw(2) << _day << "/"
            << std::setw(2) << _month << "/"
            << std::setw(4) << _year;
        return oss.str();
    }
    
    std::string toMDY() const {
        std::ostringstream oss;
        oss << std::setfill('0') 
            << std::setw(2) << _month << "/"
            << std::setw(2) << _day << "/"
            << std::setw(4) << _year;
        return oss.str();
    }
    
    std::string toYMD(char sep) const {
        std::ostringstream oss;
        oss << std::setfill('0') 
            << std::setw(4) << _year << sep
            << std::setw(2) << _month << sep
            << std::setw(2) << _day;
        return oss.str();
    }
};

// ============================================================================
// DATE RANGE - REPRESENT PERIOD BETWEEN TWO DATES
// ============================================================================

class DateRange {
public:
    DateRange(const Date& start, const Date& end) : _start(start), _end(end) {
        if (end < start) throw std::invalid_argument("End date must be after start date");
    }

    const Date& start() const { return _start; }
    const Date& endDate() const { return _end; } // renamed accessor

    int days() const { return _end.daysBetween(_start); }
    bool contains(const Date& date) const { return date >= _start && date <= _end; }
    bool overlaps(const DateRange& other) const { return _start <= other._end && _end >= other._start; }

    class Iterator {
    public:
        Iterator(const Date& current, const Date& end) : _current(current), _end(end) {}
        const Date& operator*() const { return _current; }
        const Date* operator->() const { return &_current; }
        Iterator& operator++() { _current = _current.addDays(1); return *this; }
        bool operator!=(const Iterator& other) const { return !(_current == other._current); }
    private:
        Date _current;
        Date _end;
    };

    Iterator begin() const { return Iterator(_start, _end); }
    Iterator end() const { return Iterator(_end.addDays(1), _end); }

    std::vector<Date> toVector() const {
        std::vector<Date> dates;
        for (DateRange::Iterator it = begin(); it != end(); ++it) dates.push_back(*it);
        return dates;
    }

private:
    Date _start;
    Date _end;
};

// ============================================================================
// DATE UTILITIES
// ============================================================================

namespace DateUtils {
    // Get first day of month
    inline Date firstOfMonth(int year, int month) {
        return Date(year, month, 1);
    }
    
    // Get last day of month
    inline Date lastOfMonth(int year, int month) {
        int days = Date::daysInMonth(year, month);
        return Date(year, month, days);
    }
    
    // Get first day of year
    inline Date firstOfYear(int year) {
        return Date(year, 1, 1);
    }
    
    // Get last day of year
    inline Date lastOfYear(int year) {
        return Date(year, 12, 31);
    }
    
    // Get next weekday occurrence
    inline Date nextWeekday(const Date& from, int targetWeekday) {
        int currentWeekday = from.weekday();
        int daysToAdd = (targetWeekday - currentWeekday + 7) % 7;
        if (daysToAdd == 0) daysToAdd = 7;
        return from.addDays(daysToAdd);
    }
    
    // Get previous weekday occurrence
    inline Date previousWeekday(const Date& from, int targetWeekday) {
        int currentWeekday = from.weekday();
        int daysToSubtract = (currentWeekday - targetWeekday + 7) % 7;
        if (daysToSubtract == 0) daysToSubtract = 7;
        return from.subtractDays(daysToSubtract);
    }
    
    // Get all dates in month
    inline std::vector<Date> datesInMonth(int year, int month) {
        Date first = firstOfMonth(year, month);
        Date last = lastOfMonth(year, month);
        return DateRange(first, last).toVector();
    }
    
    // Get all dates in year
    inline std::vector<Date> datesInYear(int year) {
        Date first = firstOfYear(year);
        Date last = lastOfYear(year);
        return DateRange(first, last).toVector();
    }
    
    // Check if date is weekend
    inline bool isWeekend(const Date& date) {
        int wd = date.weekday();
        return wd == 0 || wd == 6;  // Sunday or Saturday
    }
    
    // Check if date is weekday
    inline bool isWeekday(const Date& date) {
        return !isWeekend(date);
    }
    
    // Count business days between two dates (Monday-Friday)
    inline int businessDaysBetween(const Date& start, const Date& end) {
        if (end < start) return 0;
        int count = 0;
        DateRange range(start, end);
        for (DateRange::Iterator it = range.begin(); it != range.end(); ++it) {
            Date d = *it;
            if (isWeekday(d)) ++count;
        }
        return count;
    }
    
    // Add business days (skip weekends)
    inline Date addBusinessDays(const Date& from, int days) {
        Date result = from;
        int added = 0;
        int direction = (days > 0) ? 1 : -1;
        int remaining = std::abs(days);
        
        while (added < remaining) {
            result = result.addDays(direction);
            if (isWeekday(result)) {
                ++added;
            }
        }
        
        return result;
    }
    
    // Get age from birth date
    inline int age(const Date& birthDate, const Date& currentDate = Date::today()) {
        int age = currentDate.year() - birthDate.year();
        
        if (currentDate.month() < birthDate.month() ||
            (currentDate.month() == birthDate.month() && currentDate.day() < birthDate.day())) {
            --age;
        }
        
        return age;
    }
    
    // Calculate quarters
    inline int quarter(const Date& date) {
        return (date.month() - 1) / 3 + 1;
    }
    
    inline Date quarterStart(int year, int quarter) {
        int month = (quarter - 1) * 3 + 1;
        return Date(year, month, 1);
    }
    
    inline Date quarterEnd(int year, int quarter) {
        int month = quarter * 3;
        return lastOfMonth(year, month);
    }
}

#endif // DATE_HPP