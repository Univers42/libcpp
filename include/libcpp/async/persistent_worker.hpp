#pragma once
// persistent_worker.hpp — Named persistent background worker.
//
// Runs a queue of named tasks in a dedicated thread, looping forever
// until stop() is called. New tasks can be added at runtime.

#include "libcpp/async/thread_safe_queue.hpp"
#include <functional>
#include <map>
#include <string>
#include <thread>
#include <vector>

namespace libcpp {
namespace async {

class PersistentWorker {
public:
    explicit PersistentWorker(const std::string& name);
    PersistentWorker(const PersistentWorker&) = delete;
    PersistentWorker& operator=(const PersistentWorker&) = delete;
    ~PersistentWorker();

    // Add a named step to the task cycle
    void addTask(const std::string& name, std::function<void()> task);

    // Remove a named step
    void removeTask(const std::string& name);

    // Start the worker loop (throws if already running or no tasks)
    void start();

    // Request stop and join
    void stop();

    bool isRunning() const;
    const std::string& name() const;

private:
    void _loop();

    std::string                                               _name;
    std::vector<std::pair<std::string, std::function<void()>>> _tasks;
    std::thread                                               _thread;
    bool                                                      _running  = false;
    bool                                                      _stopFlag = false;
};

} // namespace async
} // namespace libcpp
