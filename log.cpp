/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 17:44:58 by marvin            #+#    #+#             */
/*   Updated: 2025/12/23 17:44:58 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "log.hpp"

// logger.hpp
# include <chrono>
# include <condition_variable>
# include <ctime>
# include <fstream>
# include <iomanip>
# include <iostream>
# include <memory>
# include <mutex>
# include <queue>
# include <sstream>
# include <string>
# include <thread>
# include <vector>
# include <functional>
# include <unordered_map>
# include <atomic>
# include "message.hpp"

// --------------------------- Core logger types (restored) ---------------------------
enum class LogLevel
{
    Trace = 0,
    Debug,
    Info,
    Warn,
    Error,
    Fatal
};

inline std::string level_to_string(LogLevel l)
{
    switch (l)
    {
    case LogLevel::Trace:
        return "TRACE";
    case LogLevel::Debug:
        return "DEBUG";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warn:
        return "WARN";
    case LogLevel::Error:
        return "ERROR";
    case LogLevel::Fatal:
        return "FATAL";
    }
    return "UNKNOWN";
}

class Logger
{
public:
    virtual ~Logger() = default;
    virtual void log(LogLevel level, const std::string &message) = 0;
};

// Console logger (simple)
class ConsoleLogger : public Logger
{
public:
    void log(LogLevel level, const std::string &message) override
    {
        (void)level;
        std::cout << message << std::endl;
    }
};

// Simple file logger (thread-safe on write)
class FileLogger : public Logger
{
public:
    explicit FileLogger(const std::string &path) : ofs(path, std::ios::app)
    {
        if (!ofs)
            throw std::runtime_error("Could not open log file: " + path);
    }
    void log(LogLevel level, const std::string &message) override
    {
        (void)level;
        std::lock_guard<std::mutex> lg(mtx);
        ofs << message << std::endl;
        ofs.flush();
    }

private:
    std::ofstream ofs;
    std::mutex mtx;
};

// --------------------------- Decorator base (restored) ---------------------------
class LoggerDecorator : public Logger
{
protected:
    std::unique_ptr<Logger> inner;

public:
    explicit LoggerDecorator(std::unique_ptr<Logger> inner_) : inner(std::move(inner_)) {}
    void log(LogLevel level, const std::string &message) override
    {
        inner->log(level, message);
    }
    virtual ~LoggerDecorator() = default;
};

// Timestamp decorator (restored)
class TimestampDecorator : public LoggerDecorator
{
public:
    using LoggerDecorator::LoggerDecorator;
    void log(LogLevel level, const std::string &message) override
    {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif
        std::ostringstream ss;
        ss << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] " << message;
        inner->log(level, ss.str());
    }
};

// --------------------------- Thread ID manager (prettier ids) ---------------------------
class ThreadIdManager
{
public:
    static void ensure_main()
    {
        std::call_once(init_flag, []
                       {
            std::lock_guard<std::mutex> lg(mtx);
            main_id = std::this_thread::get_id();
            labels[main_id] = "Main"; });
    }

    static std::string label_for(std::thread::id id)
    {
        std::lock_guard<std::mutex> lg(mtx);
        auto it = labels.find(id);
        if (it != labels.end())
            return it->second;
        std::ostringstream ss;
        ss << "T" << next_idx++;
        std::string lbl = ss.str();
        labels[id] = lbl;
        return lbl;
    }

    static std::string current_label()
    {
        ensure_main();
        return label_for(std::this_thread::get_id());
    }

private:
    static std::mutex mtx;
    static std::unordered_map<std::thread::id, std::string> labels;
    static std::once_flag init_flag;
    static std::thread::id main_id;
    static int next_idx;
};

std::mutex ThreadIdManager::mtx;
std::unordered_map<std::thread::id, std::string> ThreadIdManager::labels;
std::once_flag ThreadIdManager::init_flag;
std::thread::id ThreadIdManager::main_id;
int ThreadIdManager::next_idx = 1;

// --------------------------- ThreadIdDecorator (uses ThreadIdManager) ---------------------------
class ThreadIdDecorator : public LoggerDecorator
{
public:
    using LoggerDecorator::LoggerDecorator;
    void log(LogLevel level, const std::string &message) override
    {
        std::ostringstream ss;
        ss << "[" << ThreadIdManager::current_label() << "] " << message;
        inner->log(level, ss.str());
    }
};

// --------------------------- Color decorator (adjusted colors) ---------------------------
class LogColorDecorator : public LoggerDecorator
{
public:
    using LoggerDecorator::LoggerDecorator;
    void log(LogLevel level, const std::string &message) override
    {
        std::string code;
        switch (level)
        {
        case LogLevel::Trace:
            code = tester::colors::BRIGHT_BLACK; // light gray
            break;
        case LogLevel::Debug:
            code = tester::colors::BLUE;
            break;
        case LogLevel::Info:
            code = tester::colors::GREEN;
            break;
        case LogLevel::Warn:
            code = tester::colors::YELLOW;
            break;
        case LogLevel::Error:
            code = tester::colors::RED;
            break;
        case LogLevel::Fatal:
            code = std::string(tester::colors::BG_RED) + tester::colors::WHITE; // white on red bg
            break;
        default:
            code = tester::colors::RESET;
            break;
        }
        std::ostringstream ss;
        ss << code << "[" << level_to_string(level) << "] " << message << tester::colors::RESET;
        inner->log(level, ss.str());
    }
};

// --------------------------- FileSinkDecorator (file + forward to inner) ---------------------------
class FileSinkDecorator : public LoggerDecorator
{
public:
    FileSinkDecorator(std::unique_ptr<Logger> inner_, const std::string &path)
        : LoggerDecorator(std::move(inner_)), ofs(path, std::ios::app)
    {
        if (!ofs)
            throw std::runtime_error("Could not open log file: " + path);
    }

    void log(LogLevel level, const std::string &message) override
    {
        {
            std::lock_guard<std::mutex> lg(file_mtx);
            ofs << message << std::endl;
            ofs.flush();
        }
        inner->log(level, message);
    }

private:
    std::ofstream ofs;
    std::mutex file_mtx;
};

// --------------------------- LevelFilterDecorator (restored) ---------------------------
class LevelFilterDecorator : public LoggerDecorator
{
public:
    LevelFilterDecorator(std::unique_ptr<Logger> inner_, LogLevel min_level_)
        : LoggerDecorator(std::move(inner_)), min_level(min_level_) {}
    void log(LogLevel level, const std::string &message) override
    {
        if (level >= min_level)
            inner->log(level, message);
    }

private:
    LogLevel min_level;
};

// --------------------------- JSON formatter (improved structured JSON) ---------------------------
class JsonFormatterDecorator : public LoggerDecorator
{
public:
    using LoggerDecorator::LoggerDecorator;
    void log(LogLevel level, const std::string &message) override
    {
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

        std::ostringstream ss;
        ss << R"({"ts":)" << ms
           << R"(,"level":")" << level_to_string(level) << R"(",)"
           << R"("thread":")" << escape_json(ThreadIdManager::current_label()) << R"(",)"
           << R"("msg":")" << escape_json(message) << R"("})";
        inner->log(level, ss.str());
    }

private:
    static std::string escape_json(const std::string &s)
    {
        std::ostringstream o;
        for (char c : s)
        {
            switch (c)
            {
            case '"':
                o << "\\\"";
                break;
            case '\\':
                o << "\\\\";
                break;
            case '\b':
                o << "\\b";
                break;
            case '\f':
                o << "\\f";
                break;
            case '\n':
                o << "\\n";
                break;
            case '\r':
                o << "\\r";
                break;
            case '\t':
                o << "\\t";
                break;
            default:
                if (static_cast<unsigned char>(c) < 0x20)
                    o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                else
                    o << c;
            }
        }
        return o.str();
    }
};

// --------------------------- Async batch-flushing decorator ---------------------------
class AsyncBatchDecorator : public LoggerDecorator
{
public:
    AsyncBatchDecorator(std::unique_ptr<Logger> inner_, size_t batch_size = 8, unsigned flush_ms = 200)
        : LoggerDecorator(std::move(inner_)), batch_size(batch_size), flush_ms(flush_ms), stop_flag(false), worker(&AsyncBatchDecorator::run, this) {}

    ~AsyncBatchDecorator() override
    {
        {
            std::unique_lock<std::mutex> lk(mtx);
            stop_flag = true;
            cond.notify_all();
        }
        if (worker.joinable())
            worker.join();
        flush_remaining();
    }

    void log(LogLevel level, const std::string &message) override
    {
        std::unique_lock<std::mutex> lk(mtx);
        buffer.emplace_back(level, message);
        if (buffer.size() >= batch_size)
            cond.notify_one();
    }

private:
    size_t batch_size;
    unsigned flush_ms;
    std::mutex mtx;
    std::condition_variable cond;
    std::vector<std::pair<LogLevel, std::string>> buffer;
    bool stop_flag;
    std::thread worker;

    void run()
    {
        std::vector<std::pair<LogLevel, std::string>> local;
        while (true)
        {
            {
                std::unique_lock<std::mutex> lk(mtx);
                if (!stop_flag && buffer.empty())
                    cond.wait_for(lk, std::chrono::milliseconds(flush_ms));
                if (buffer.empty() && stop_flag)
                    break;
                buffer.swap(local);
            }
            for (auto &it : local)
                inner->log(it.first, it.second);
            local.clear();
            // loop continues until stop_flag and no buffer
        }
    }

    void flush_remaining()
    {
        std::vector<std::pair<LogLevel, std::string>> local;
        {
            std::lock_guard<std::mutex> lg(mtx);
            buffer.swap(local);
        }
        for (auto &it : local)
            inner->log(it.first, it.second);
    }
};

// --------------------------- Pattern formatter decorator ---------------------------
class PatternFormatterDecorator : public LoggerDecorator
{
public:
    explicit PatternFormatterDecorator(std::unique_ptr<Logger> inner_, const std::string &pattern_)
        : LoggerDecorator(std::move(inner_)), pattern(pattern_) {}

    void log(LogLevel level, const std::string &message) override
    {
        std::ostringstream out;
        auto t = std::chrono::system_clock::now();
        std::time_t tt = std::chrono::system_clock::to_time_t(t);
        std::tm tm{};
#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&tm, &tt);
#else
        localtime_r(&tt, &tm);
#endif

        for (size_t i = 0; i < pattern.size(); ++i)
        {
            if (pattern[i] == '%' && i + 1 < pattern.size())
            {
                char tok = pattern[++i];
                switch (tok)
                {
                case 'H':
                    out << std::setw(2) << std::setfill('0') << tm.tm_hour;
                    break;
                case 'M':
                    out << std::setw(2) << std::setfill('0') << tm.tm_min;
                    break;
                case 'S':
                    out << std::setw(2) << std::setfill('0') << tm.tm_sec;
                    break;
                case 't':
                    out << ThreadIdManager::current_label();
                    break;
                case 'l':
                    out << level_to_string(level);
                    break;
                case 'v':
                    out << message;
                    break;
                default:
                    out << '%' << tok;
                    break;
                }
            }
            else
                out << pattern[i];
        }
        inner->log(level, out.str());
    }

private:
    std::string pattern;
};

// --------------------------- LoggerBuilder (single consolidated) ---------------------------
class LoggerBuilder
{
public:
    LoggerBuilder() = default;
    using Factory = std::function<std::unique_ptr<Logger>(std::unique_ptr<Logger>)>;

    LoggerBuilder &addTimestamp()
    {
        factories.push_back([](std::unique_ptr<Logger> inner)
                            { return std::make_unique<TimestampDecorator>(std::move(inner)); });
        return *this;
    }
    LoggerBuilder &addThreadId()
    {
        factories.push_back([](std::unique_ptr<Logger> inner)
                            { return std::make_unique<ThreadIdDecorator>(std::move(inner)); });
        return *this;
    }
    LoggerBuilder &addColor()
    {
        factories.push_back([](std::unique_ptr<Logger> inner)
                            { return std::make_unique<LogColorDecorator>(std::move(inner)); });
        return *this;
    }
    LoggerBuilder &addJsonFormat()
    {
        factories.push_back([](std::unique_ptr<Logger> inner)
                            { return std::make_unique<JsonFormatterDecorator>(std::move(inner)); });
        return *this;
    }
    LoggerBuilder &addPattern(const std::string &pattern)
    {
        factories.push_back([pattern](std::unique_ptr<Logger> inner)
                            { return std::make_unique<PatternFormatterDecorator>(std::move(inner), pattern); });
        return *this;
    }
    LoggerBuilder &addLevelFilter(LogLevel minLevel)
    {
        factories.push_back([minLevel](std::unique_ptr<Logger> inner)
                            { return std::make_unique<LevelFilterDecorator>(std::move(inner), minLevel); });
        return *this;
    }
    LoggerBuilder &addFileSink(const std::string &path)
    {
        factories.push_back([path](std::unique_ptr<Logger> inner)
                            { return std::make_unique<FileSinkDecorator>(std::move(inner), path); });
        return *this;
    }
    LoggerBuilder &addAsyncBatch(size_t batchSize, unsigned flushMs)
    {
        factories.push_back([batchSize, flushMs](std::unique_ptr<Logger> inner)
                            { return std::make_unique<AsyncBatchDecorator>(std::move(inner), batchSize, flushMs); });
        return *this;
    }

    std::unique_ptr<Logger> buildConsole()
    {
        std::unique_ptr<Logger> base = std::make_unique<ConsoleLogger>();
        return applyFactories(std::move(base));
    }

    std::unique_ptr<Logger> buildFile(const std::string &path)
    {
        std::unique_ptr<Logger> base = std::make_unique<FileLogger>(path);
        return applyFactories(std::move(base));
    }

    std::unique_ptr<Logger> buildCustom(std::unique_ptr<Logger> base)
    {
        return applyFactories(std::move(base));
    }

private:
    std::vector<Factory> factories;

    std::unique_ptr<Logger> applyFactories(std::unique_ptr<Logger> base)
    {
        std::unique_ptr<Logger> current = std::move(base);
        for (auto it = factories.rbegin(); it != factories.rend(); ++it)
            current = (*it)(std::move(current));
        return current;
    }
};

// --------------------------- Demo runner (was main) ---------------------------
void run_logger_demo()
{
    ThreadIdManager::ensure_main();

    LoggerBuilder builder;
    auto logger = builder
                      .addPattern("%H:%M:%S [%t] [%l] %v")
                      .addThreadId()
                      .addColor()
                      .addLevelFilter(LogLevel::Debug)
                      .addFileSink("app.log")
                      .addAsyncBatch(6, 150)
                      .buildConsole();

    // Demonstrations using tester::Message
    {
        // formatted log
        tester::Message m;
        m << "server started at port=" << 8080 << " mode=" << "prod";
        logger->log(LogLevel::Info, m.GetString());
    }

    {
        // failure / error message with highlighted portion
        tester::Message fail;
        fail << "Failed to allocate buffer of size=" << 4096 << " bytes. "
             << tester::colors::BRIGHT_RED << "OOM" << tester::colors::RESET;
        logger->log(LogLevel::Error, fail.GetString());
    }

    {
        // debug string with structured values
        tester::Message dbg;
        dbg << "dbg: ptr=" << static_cast<void *>(nullptr) << " id=" << 12345;
        logger->log(LogLevel::Debug, dbg.GetString());
    }

    {
        // memory tracking style report
        tester::Message mem;
        mem << "mem: used=" << 123456 << " free=" << 654321;
        logger->log(LogLevel::Info, mem.GetString());
    }

    {
        // diff-like output
        tester::Message diff;
        diff << tester::colors::GREEN << "+ expected line" << tester::colors::RESET << "\n";
        diff << tester::colors::RED << "- actual line" << tester::colors::RESET;
        logger->log(LogLevel::Info, diff.GetString());
    }

    {
        // test assertion message
        tester::Message assertmsg;
        assertmsg << "assert failed: expected=" << 10 << " got=" << 9;
        logger->log(LogLevel::Fatal, assertmsg.GetString());
    }

    // spawn workers as before to show concurrent logs
    auto worker = [&](int id)
    {
        for (int i = 0; i < 5; ++i)
        {
            tester::Message ss;
            ss << "worker-" << id << " loop=" << i;
            logger->log(LogLevel::Info, ss.GetString());
            std::this_thread::sleep_for(std::chrono::milliseconds(50 + id * 10));
        }
    };

    std::thread t1(worker, 1);
    std::thread t2(worker, 2);

    {
        tester::Message m;
        m << "a debug message with details: x=" << 42;
        logger->log(LogLevel::Debug, m.GetString());
    }
    {
        tester::Message t;
        t << "this trace message is filtered by default";
        logger->log(LogLevel::Trace, t.GetString());
    }
    {
        tester::Message w;
        w << "watch out!";
        logger->log(LogLevel::Warn, w.GetString());
    }

    t1.join();
    t2.join();

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

// ── Public logging helpers declared in log.hpp ──────────────────────────────

void log_info(const std::string &msg)
{
    tester::Message m;
    m.SetColor(tester::colors::GREEN) << "[INFO] " << msg;
    std::cout << m << std::endl;
}

void log_warn(const std::string &msg)
{
    tester::Message m;
    m.SetColor(tester::colors::YELLOW) << "[WARN] " << msg;
    std::cout << m << std::endl;
}

void log_error(const std::string &msg)
{
    tester::Message m;
    m.SetColor(tester::colors::RED) << "[ERROR] " << msg;
    std::cerr << m << std::endl;
}

extern "C" {

void c_log_info(const char *msg)
{
    log_info(msg ? msg : "");
}

void c_print_message(const char *msg, const char *color_code)
{
    tester::Message m;
    if (color_code)
        m.SetColor(color_code);
    m << (msg ? msg : "");
    std::cout << m << std::endl;
}

void c_run_logger_demo()
{
    run_logger_demo();
}

} /* extern "C" */
