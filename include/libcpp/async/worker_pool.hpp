#pragma once
// worker_pool.hpp — Thread pool with configurable worker count.
//
// Workers pull jobs from a ThreadSafeQueue and execute them.

#include "libcpp/async/thread_safe_queue.hpp"
#include <functional>
#include <string>
#include <thread>
#include <vector>

namespace libcpp {
namespace async {

/// @brief Thread pool pulling jobs from a shared ThreadSafeQueue.
class WorkerPool {
public:
    explicit WorkerPool(size_t workerCount);
    WorkerPool(const WorkerPool&) = delete;
    WorkerPool& operator=(const WorkerPool&) = delete;
    ~WorkerPool();

    // Enqueue a job for execution by any available worker
    void addJob(std::function<void()> job);

    // Number of workers in the pool
    size_t workerCount() const;

    // Graceful shutdown: finish queued jobs, then stop
    void stop();

private:
    void _workerLoop(size_t id);

    ThreadSafeQueue<std::function<void()>> _jobs;
    std::vector<std::thread>               _workers;
    size_t                                 _count;
    bool                                   _stopped = false;
};

} // namespace async
} // namespace libcpp
