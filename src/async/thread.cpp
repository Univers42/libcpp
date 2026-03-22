// thread.cpp — Thread wrapper implementation.

#include "libcpp/async/thread.hpp"
#include <stdexcept>
#include "libcpp/term/thread_safe_iostream.hpp"

namespace libcpp {
namespace async {

Thread::Thread() = default;

Thread::~Thread() {
  if (_running) stop();
}

void Thread::setName(const std::string& name) { _name = name; }
const std::string& Thread::name() const { return _name; }

void Thread::setCallback(std::function<void()> callback) {
  if (_running)
    throw std::runtime_error("Thread: cannot set callback while running");
  _callback = std::move(callback);
}

void Thread::start() {
  if (_running) throw std::runtime_error("Thread: already running");
  if (!_callback) throw std::runtime_error("Thread: no callback set");
  _running = true;
  _thread = std::thread([this]() {
    // Set the thread-local prefix so threadSafeCout prints the name
    threadSafeCout.setPrefix(_name);
    _callback();
  });
}

void Thread::stop() {
  if (!_running) return;
  if (_thread.joinable()) _thread.join();
  _running = false;
}

bool Thread::isRunning() const { return _running; }

}  // namespace async
}  // namespace libcpp
