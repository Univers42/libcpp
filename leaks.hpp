/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   leaks.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 21:19:57 by marvin            #+#    #+#             */
/*   Updated: 2025/12/23 21:19:57 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEAKS_HPP
#define LEAKS_HPP
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <dlfcn.h>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <string>
#include <chrono>

using std::endl;

namespace memcheck
{

	struct Allocation
	{
		void *ptr = nullptr;
		std::size_t size = 0;
		bool tracked = false;
		// new: thread id and optional tag
		std::string thread_id;
		std::string tag;
		// new: backtrace addresses (captured at allocation)
		std::vector<void *> backtrace;
		// capture timestamp if needed
		std::chrono::system_clock::time_point ts;

		Allocation() = default;
		Allocation(void *p, std::size_t s, bool t = true)
			: ptr(p), size(s), tracked(t), thread_id(), tag(), backtrace(), ts(std::chrono::system_clock::now()) {}
		bool operator==(Allocation const &o) const noexcept { return ptr == o.ptr; }
	};

	// Observer interface
	struct Observer
	{
		virtual ~Observer() = default;
		virtual void notify(std::string_view msg) = 0;
	};

	// simple stderr logger
	struct LoggerObserver : Observer
	{
		void notify(std::string_view msg) override;
	};

	// Decorator / Recorder observer: records notifications and simple classifications.
	// Useful to inspect logs after a run or to drive UI/CI logic.
	struct LogRecorder : Observer
	{
		// record notification
		void notify(std::string_view msg) override;

		// access recorded logs (thread-safe)
		static std::vector<std::string> getLogs();
		static void clearLogs();

		// simple count by keyword (thread-safe)
		static int countReason(std::string const &keyword);
	};

	// strategy interface
	struct MemCheckStrategy
	{
		virtual ~MemCheckStrategy() = default;
		virtual void start() = 0;
		virtual void stop() = 0;
		virtual void report() = 0;
	};

	// Leak-check strategy (uses the global allocation list)
	struct LeakCheck : MemCheckStrategy
	{
		LeakCheck() = default;
		void start() override;
		void stop() override;
		void report() override;
		// non-copyable
		LeakCheck(LeakCheck const &) = delete;
		LeakCheck &operator=(LeakCheck const &) = delete;
	};

	// Facade to manage strategies & observers
	class MemCheckFacade
	{
	public:
		MemCheckFacade() noexcept;
		~MemCheckFacade() = default;

		void addStrategy(std::unique_ptr<MemCheckStrategy> s);
		void addObserver(std::shared_ptr<Observer> o);

		void startAll();
		void stopAll();
		void reportAll();

		// convenience runner
		void runAll();

	private:
		std::vector<std::unique_ptr<MemCheckStrategy>> strategies_;
		std::vector<std::shared_ptr<Observer>> observers_;

		void notifyObservers(std::string_view msg);

		// added: track whether facade is started to make start/stop idempotent
		std::atomic<bool> started_{false};
	};

	// RAII collector: starts checks in ctor, stops and reports in dtor
	class RAIICollector
	{
	public:
		RAIICollector();
		~RAIICollector();

		RAIICollector(RAIICollector const &) = delete;
		RAIICollector &operator=(RAIICollector const &) = delete;

	private:
		MemCheckFacade facade_;
	};

	// --- global allocation bookkeeping hooks used by malloc/free overrides ---
	// Add/remove allocation entries (thread-safe)
	void onAlloc(void *p, std::size_t size);
	void onFree(void *p);

	// Control whether allocations should be recorded (set by LeakCheck)
	void setTrackingActive(bool active) noexcept;
	bool isTrackingActive() noexcept;

	// Inspect allocations for reporting
	std::vector<Allocation> snapshotAllocations();

	// Allow registering an allocation with an optional human tag (C API)
	void onAllocTag(void *p, std::size_t size, std::string const &tag);

} // namespace memcheck

// C API for querying recorded logs from C tests or scripts
#ifdef __cplusplus
extern "C"
{
#endif
	// return count of notifications containing 'kind' substring
	int memcheck_log_count(const char *kind);
	// print recorded logs to STDOUT
	void memcheck_dump_logs(void);
#ifdef __cplusplus
}
#endif

#endif