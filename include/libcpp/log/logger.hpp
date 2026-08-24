/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 21:47:56 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBCPP_LOG_LOGGER_HPP
#define LIBCPP_LOG_LOGGER_HPP

#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace libcpp {
namespace log {

/* ── Log levels ────────────────────────────────────────────────────────── */
enum Level {
  LTRACE = 0,
  LDEBUG = 1,
  LINFO = 2,
  LWARN = 3,
  LERROR = 4,
  LFATAL = 5
};

const char* level_name(Level lv);
std::string level_string(Level lv);
/* Srgb level_color(Level) lives in log/level_color.hpp, so that logging
** does not pull term/ into every consumer that includes this header. */

/* ══════════════════════════════════════════════════════════════════════════
 *  ILogger — abstract logger interface
 * ═════════════════════════════════════════════════════════════════════════ */
class ILogger {
 public:
  virtual ~ILogger();
  virtual void log(Level level, const std::string& msg) = 0;
};

/* ══════════════════════════════════════════════════════════════════════════
 *  ConsoleLogger — logs to std::cerr with color
 *  OCF compliant.
 * ═════════════════════════════════════════════════════════════════════════ */
class ConsoleLogger : public ILogger {
 public:
  ConsoleLogger();
  ConsoleLogger(const ConsoleLogger& other);
  ConsoleLogger& operator=(const ConsoleLogger& other);
  ~ConsoleLogger();

  void log(Level level, const std::string& msg);
  void set_min_level(Level lv);
  Level min_level() const;

 private:
  Level _min_level;
};

/* ══════════════════════════════════════════════════════════════════════════
 *  FileLogger — logs to a file
 *  OCF compliant.
 * ═════════════════════════════════════════════════════════════════════════ */
class FileLogger : public ILogger {
 public:
  FileLogger();
  explicit FileLogger(const std::string& path);
  FileLogger(const FileLogger& other);
  FileLogger& operator=(const FileLogger& other);
  ~FileLogger();

  void log(Level level, const std::string& msg);

 private:
  std::string _path;
  std::ofstream* _file;

  void _open();
  void _close();
};

/* ══════════════════════════════════════════════════════════════════════════
 *  NullLogger — discards all messages
 * ═════════════════════════════════════════════════════════════════════════ */
class NullLogger : public ILogger {
 public:
  NullLogger();
  NullLogger(const NullLogger& other);
  NullLogger& operator=(const NullLogger& other);
  ~NullLogger();
  void log(Level level, const std::string& msg);
};

/* ══════════════════════════════════════════════════════════════════════════
 *  TimestampDecorator — prepends timestamp to log messages
 * ═════════════════════════════════════════════════════════════════════════ */
class TimestampDecorator : public ILogger {
 public:
  TimestampDecorator();
  explicit TimestampDecorator(ILogger* inner);
  TimestampDecorator(const TimestampDecorator& other);
  TimestampDecorator& operator=(const TimestampDecorator& other);
  ~TimestampDecorator();

  void log(Level level, const std::string& msg);

 private:
  ILogger* _inner;
  bool _owns;
};

/* ══════════════════════════════════════════════════════════════════════════
 *  LogColorDecorator — adds color to log messages
 * ═════════════════════════════════════════════════════════════════════════ */
class LogColorDecorator : public ILogger {
 public:
  LogColorDecorator();
  explicit LogColorDecorator(ILogger* inner);
  LogColorDecorator(const LogColorDecorator& other);
  LogColorDecorator& operator=(const LogColorDecorator& other);
  ~LogColorDecorator();

  void log(Level level, const std::string& msg);

 private:
  ILogger* _inner;
  bool _owns;
};

/* ══════════════════════════════════════════════════════════════════════════
 *  Global logger
 * ═════════════════════════════════════════════════════════════════════════ */
void set_global(ILogger* logger);
ILogger& global();

} /* namespace log */
} /* namespace libcpp */

#endif /* LIBCPP_LOG_LOGGER_HPP */
