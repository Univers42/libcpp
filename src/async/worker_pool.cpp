// worker_pool.cpp — WorkerPool implementation.

#include "libcpp/async/worker_pool.hpp"
#include "libcpp/term/thread_safe_iostream.hpp"
#include <sstream>

namespace libcpp {
namespace async {

WorkerPool::WorkerPool(size_t workerCount) : _count(workerCount)
{
    _workers.reserve(_count);
    for (size_t i = 0; i < _count; ++i)
        _workers.emplace_back(&WorkerPool::_workerLoop, this, i);
}

WorkerPool::~WorkerPool()
{
    if (!_stopped)
        stop();
}

void WorkerPool::addJob(std::function<void()> job) { _jobs.push_back(std::move(job)); }

size_t WorkerPool::workerCount() const { return _count; }

void WorkerPool::stop()
{
    _stopped = true;
    _jobs.stop(); // wake all waiters
    for (auto& w : _workers) {
        if (w.joinable())
            w.join();
    }
}

void WorkerPool::_workerLoop(size_t id)
{
    // Set thread-local prefix
    std::ostringstream oss;
    oss << "Worker-" << id;
    threadSafeCout.setPrefix(oss.str());

    while (true) {
        try {
            auto job = _jobs.pop_front_wait();
            job();
        } catch (const std::runtime_error&) {
            // Queue stopped — exit loop
            break;
        }
    }
}

} // namespace async
} // namespace libcpp
