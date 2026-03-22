#pragma once
// persistent_worker.hpp — Named persistent background worker.
//
// Runs a queue of named tasks in a dedicated thread, looping forever
// until stop() is called. New tasks can be added at runtime.

#include <atomic>
#include <functional>
#include <map>
#include <string>
#include <thread>
#include <vector>
#include "libcpp/async/thread_safe_queue.hpp"

namespace libcpp {
namespace async {

/// @brief Named persistent worker that cycles through registered tasks.
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

  // Start the worker loop(throws if already running or no tasks)
  void start();

  // Request stop and join
  void stop();

  bool isRunning() const;
  const std::string& name() const;

 private:
  void _loop();

  std::string _name;
  std::vector<std::pair<std::string, std::function<void()>>> _tasks;
  std::thread _thread;
  std::atomic<bool> _running{false};
  std::atomic<bool> _stopFlag{false};
};

}  // namespace async
}  // namespace libcpp
