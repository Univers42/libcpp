#pragma once
// thread_safe_queue.hpp — Thread-safe templated queue.
//
// Uses std::deque + std::mutex + std::condition_variable.
// Provides blocking pop_front_wait() and non-blocking try_pop_front().

#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>
#include <stdexcept>

namespace libcpp {
namespace async {

/// @brief Thread-safe deque with blocking and non-blocking pop.
template <typename TType>
class ThreadSafeQueue {
 public:
  ThreadSafeQueue() = default;
  ~ThreadSafeQueue() = default;

  void push_back(const TType& newElement) {
    {
      std::lock_guard<std::mutex> lock(_mtx);
      _queue.push_back(newElement);
    }
    _cv.notify_one();
  }

  void push_front(const TType& newElement) {
    {
      std::lock_guard<std::mutex> lock(_mtx);
      _queue.push_front(newElement);
    }
    _cv.notify_one();
  }

  TType pop_back() {
    std::lock_guard<std::mutex> lock(_mtx);
    if (_queue.empty())
      throw std::runtime_error("ThreadSafeQueue: pop from empty queue");
    TType val = std::move(_queue.back());
    _queue.pop_back();
    return val;
  }

  TType pop_front() {
    std::lock_guard<std::mutex> lock(_mtx);
    if (_queue.empty())
      throw std::runtime_error("ThreadSafeQueue: pop from empty queue");
    TType val = std::move(_queue.front());
    _queue.pop_front();
    return val;
  }

  // Blocking pop — waits until an element is available
  TType pop_front_wait() {
    std::unique_lock<std::mutex> lock(_mtx);
    _cv.wait(lock, [this]() { return !_queue.empty() || _stopped; });
    if (_queue.empty()) throw std::runtime_error("ThreadSafeQueue: stopped");
    TType val = std::move(_queue.front());
    _queue.pop_front();
    return val;
  }

  // Non-blocking attempt
  [[nodiscard]] std::optional<TType> try_pop_front() {
    std::lock_guard<std::mutex> lock(_mtx);
    if (_queue.empty()) return std::nullopt;
    TType val = std::move(_queue.front());
    _queue.pop_front();
    return val;
  }

  [[nodiscard]] size_t size() const {
    std::lock_guard<std::mutex> lock(_mtx);
    return _queue.size();
  }

  [[nodiscard]] bool empty() const {
    std::lock_guard<std::mutex> lock(_mtx);
    return _queue.empty();
  }

  void clear() {
    std::lock_guard<std::mutex> lock(_mtx);
    _queue.clear();
  }

  // Wake up all waiters(used during shutdown)
  void stop() {
    {
      std::lock_guard<std::mutex> lock(_mtx);
      _stopped = true;
    }
    _cv.notify_all();
  }

 private:
  std::deque<TType> _queue;
  mutable std::mutex _mtx;
  std::condition_variable _cv;
  bool _stopped = false;
};

}  // namespace async
}  // namespace libcpp
