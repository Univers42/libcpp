#pragma once
// thread_safe_iostream.hpp — Thread-safe I/O stream with per-thread prefix.
//
// Each thread gets its own buffer (thread_local). Output is flushed atomically
// under a global mutex on newline or std::endl.

#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

namespace libcpp {

class ThreadSafeIOStream {
public:
    ThreadSafeIOStream() = default;
    ~ThreadSafeIOStream() = default;

    void setPrefix(const std::string& prefix);

    template <typename T>
    ThreadSafeIOStream& operator<<(const T& val)
    {
        _buffer << val;
        // Flush on newline character if it's a char or string
        return *this;
    }

    // Manipulator support (detects std::endl, std::flush)
    ThreadSafeIOStream& operator<<(std::ostream& (*manip)(std::ostream&));

    template <typename T>
    ThreadSafeIOStream& operator>>(T& dest)
    {
        std::lock_guard<std::mutex> lock(_globalMtx);
        std::cin >> dest;
        return *this;
    }

    template <typename T>
    void prompt(const std::string& question, T& dest)
    {
        {
            std::lock_guard<std::mutex> lock(_globalMtx);
            std::cout << _prefix << question;
            std::cout.flush();
        }
        std::lock_guard<std::mutex> lock(_globalMtx);
        std::cin >> dest;
    }

private:
    void _flush();

    static thread_local std::string        _prefix;
    static thread_local std::ostringstream _buffer;
    static std::mutex                      _globalMtx;
};

extern thread_local ThreadSafeIOStream threadSafeCout;

} // namespace libcpp
