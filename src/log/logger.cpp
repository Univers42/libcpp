/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/21 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/log/logger.hpp"
#include "libcpp/term/style.hpp"
#include <ctime>
#include <cstring>

namespace libcpp
{
namespace log
{

/* ── helpers ───────────────────────────────────────────────────────────── */

const char* level_name(Level lv)
{
	switch (lv)
	{
		case LTRACE: return "TRACE";
		case LDEBUG: return "DEBUG";
		case LINFO:  return "INFO";
		case LWARN:  return "WARN";
		case LERROR: return "ERROR";
		case LFATAL: return "FATAL";
	}
	return "???";
}

Srgb level_color(Level lv)
{
	switch (lv)
	{
		case LTRACE: return Srgb(128, 128, 128);
		case LDEBUG: return Srgb(0, 200, 255);
		case LINFO:  return Srgb(0, 255, 0);
		case LWARN:  return Srgb(255, 255, 0);
		case LERROR: return Srgb(255, 80, 80);
		case LFATAL: return Srgb(255, 0, 0);
	}
	return Srgb(255, 255, 255);
}

static std::string _timestamp()
{
	std::time_t now = std::time(0);
	char buf[64];
	std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
	return std::string(buf);
}

/* ── ILogger ───────────────────────────────────────────────────────────── */

ILogger::~ILogger() {}

/* ── ConsoleLogger ─────────────────────────────────────────────────────── */

ConsoleLogger::ConsoleLogger() : _min_level(LTRACE) {}
ConsoleLogger::ConsoleLogger(const ConsoleLogger& o) : ILogger(), _min_level(o._min_level) {}
ConsoleLogger& ConsoleLogger::operator=(const ConsoleLogger& o)
{
	if (this != &o) _min_level = o._min_level;
	return *this;
}
ConsoleLogger::~ConsoleLogger() {}

void ConsoleLogger::log(Level level, const std::string& msg)
{
	if (level < _min_level) return;
	Srgb c = level_color(level);
	std::cerr << c.to_ansi_fg() << "[" << level_name(level) << "] "
			  << TermUtils::reset() << msg << std::endl;
}

void ConsoleLogger::set_min_level(Level lv) { _min_level = lv; }

/* ── FileLogger ────────────────────────────────────────────────────────── */

FileLogger::FileLogger() : _file(0) {}

FileLogger::FileLogger(const std::string& path) : _path(path), _file(0)
{
	_open();
}

FileLogger::FileLogger(const FileLogger& o) : ILogger(), _path(o._path), _file(0)
{
	if (!_path.empty()) _open();
}

FileLogger& FileLogger::operator=(const FileLogger& o)
{
	if (this != &o)
	{
		_close();
		_path = o._path;
		if (!_path.empty()) _open();
	}
	return *this;
}

FileLogger::~FileLogger() { _close(); }

void FileLogger::log(Level level, const std::string& msg)
{
	if (!_file || !_file->is_open()) return;
	*_file << "[" << _timestamp() << "] [" << level_name(level) << "] "
		   << msg << std::endl;
}

void FileLogger::_open()
{
	_file = new std::ofstream(_path.c_str(), std::ios::app);
}

void FileLogger::_close()
{
	if (_file)
	{
		if (_file->is_open()) _file->close();
		delete _file;
		_file = 0;
	}
}

/* ── NullLogger ────────────────────────────────────────────────────────── */

NullLogger::NullLogger() {}
NullLogger::NullLogger(const NullLogger&) : ILogger() {}
NullLogger& NullLogger::operator=(const NullLogger&) { return *this; }
NullLogger::~NullLogger() {}
void NullLogger::log(Level, const std::string&) {}

/* ── TimestampDecorator ────────────────────────────────────────────────── */

TimestampDecorator::TimestampDecorator() : _inner(0), _owns(false) {}

TimestampDecorator::TimestampDecorator(ILogger* inner) : _inner(inner), _owns(false) {}

TimestampDecorator::TimestampDecorator(const TimestampDecorator& o)
	: ILogger(), _inner(o._inner), _owns(false) {}

TimestampDecorator& TimestampDecorator::operator=(const TimestampDecorator& o)
{
	if (this != &o) { _inner = o._inner; _owns = false; }
	return *this;
}

TimestampDecorator::~TimestampDecorator()
{
	if (_owns) delete _inner;
}

void TimestampDecorator::log(Level level, const std::string& msg)
{
	if (_inner)
	{
		std::string ts = _timestamp();
		_inner->log(level, "[" + ts + "] " + msg);
	}
}

/* ── LogColorDecorator ─────────────────────────────────────────────────── */

LogColorDecorator::LogColorDecorator() : _inner(0), _owns(false) {}

LogColorDecorator::LogColorDecorator(ILogger* inner) : _inner(inner), _owns(false) {}

LogColorDecorator::LogColorDecorator(const LogColorDecorator& o)
	: ILogger(), _inner(o._inner), _owns(false) {}

LogColorDecorator& LogColorDecorator::operator=(const LogColorDecorator& o)
{
	if (this != &o) { _inner = o._inner; _owns = false; }
	return *this;
}

LogColorDecorator::~LogColorDecorator()
{
	if (_owns) delete _inner;
}

void LogColorDecorator::log(Level level, const std::string& msg)
{
	if (_inner)
	{
		Srgb c = level_color(level);
		_inner->log(level, c.to_ansi_fg() + msg + TermUtils::reset());
	}
}

/* ── Global logger ─────────────────────────────────────────────────────── */

static ILogger* g_logger = 0;
static ConsoleLogger g_default_logger;

void set_global(ILogger* logger) { g_logger = logger; }

ILogger& global()
{
	if (g_logger) return *g_logger;
	return g_default_logger;
}

} /* namespace log */
} /* namespace libcpp */
