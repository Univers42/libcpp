// thread_safe_iostream.cpp — ThreadSafeIOStream implementation.

#include "libcpp/term/thread_safe_iostream.hpp"

namespace libcpp {

// Static definitions
thread_local std::string        ThreadSafeIOStream::_prefix;
thread_local std::ostringstream ThreadSafeIOStream::_buffer;
std::mutex                      ThreadSafeIOStream::_globalMtx;

thread_local ThreadSafeIOStream threadSafeCout;

void ThreadSafeIOStream::setPrefix(const std::string& prefix)
{
    _prefix = prefix;
}

ThreadSafeIOStream& ThreadSafeIOStream::operator<<(
    std::ostream& (*manip)(std::ostream&))
{
    // Apply the manipulator (e.g., std::endl) to buffer, then flush
    _buffer << manip;
    _flush();
    return *this;
}

void ThreadSafeIOStream::_flush()
{
    std::string content = _buffer.str();
    if (content.empty())
        return;

    std::lock_guard<std::mutex> lock(_globalMtx);

    // Prepend prefix to each line
    std::string output;
    bool atLineStart = true;
    for (size_t i = 0; i < content.size(); ++i) {
        if (atLineStart && !_prefix.empty()) {
            output += _prefix;
        }
        output += content[i];
        atLineStart = (content[i] == '\n');
    }

    std::cout << output;
    std::cout.flush();
    _buffer.str("");
    _buffer.clear();
}

} // namespace libcpp
