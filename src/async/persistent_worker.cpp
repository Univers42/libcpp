// persistent_worker.cpp — PersistentWorker implementation.

#include "libcpp/async/persistent_worker.hpp"
#include "libcpp/term/thread_safe_iostream.hpp"
#include <stdexcept>

namespace libcpp {
namespace async {

PersistentWorker::PersistentWorker(const std::string& name) : _name(name) {}

PersistentWorker::~PersistentWorker()
{
    if (_running)
        stop();
}

void PersistentWorker::addTask(const std::string& name, std::function<void()> task)
{
    _tasks.emplace_back(name, std::move(task));
}

void PersistentWorker::removeTask(const std::string& name)
{
    for (auto it = _tasks.begin(); it != _tasks.end(); ++it) {
        if (it->first == name) {
            _tasks.erase(it);
            return;
        }
    }
}

void PersistentWorker::start()
{
    if (_running)
        throw std::runtime_error("PersistentWorker: already running");
    if (_tasks.empty())
        throw std::runtime_error("PersistentWorker: no tasks registered");
    _stopFlag = false;
    _running  = true;
    _thread   = std::thread(&PersistentWorker::_loop, this);
}

void PersistentWorker::stop()
{
    _stopFlag = true;
    if (_thread.joinable())
        _thread.join();
    _running = false;
}

bool PersistentWorker::isRunning() const { return _running; }
const std::string& PersistentWorker::name() const { return _name; }

void PersistentWorker::_loop()
{
    threadSafeCout.setPrefix(_name);
    while (!_stopFlag) {
        for (const auto& [taskName, fn] : _tasks) {
            if (_stopFlag)
                break;
            fn();
        }
    }
}

} // namespace async
} // namespace libcpp
