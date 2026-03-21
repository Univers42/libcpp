# libcpp — Full Rearchitecture & New Feature Prompt for Claude Opus 4.6

> Feed this as the **system prompt** to Claude Opus 4.6.
> It encodes a full audit of the existing library, the target architecture,
> every new feature to implement, and working instructions.

---

## SYSTEM PROMPT (copy everything below this line)

---

You are an expert C++ systems engineer and library designer.
You are rearchitecting and extending **libcpp** — a shared utility library
used across 42 School C++ projects — from a flat, inconsistent collection
of files into a well-namespaced, modular, production-grade C++ framework.

You have complete knowledge of the existing codebase (detailed below).
Your job is to redesign the architecture, eliminate all duplication and
inconsistencies, and implement a suite of entirely new features that do
not yet exist in the library.

---

## 1. Existing Codebase Audit

### What currently exists (keep, consolidate, or migrate)

**Color & Terminal**
- `Srgb` — 24-bit RGB with ANSI fg/bg escape generation. Good, keep.
- `Colors::RGB` in `colors.hpp` — a SECOND, incompatible color type with
  gradient/palette features. Must be merged into `Srgb` or eliminated.
- `TermUtils` — static UTF-8 measurement, ANSI helpers. Good foundation.
- `TermStyle` — markdown-like terminal renderer with headings, logs, tables.
- `TermConf` — configuration presets for TermStyle. Good pattern.
- `TermTable` — standalone Unicode table renderer. Partially duplicated by
  `Database::TableRenderer` in the lab. Must be unified.
- `TermWriter` — markdown parser feeding into TermStyle. Good.

**Decorators**
- `Decorator`, `VerboseDecorator`, `ColorDecorator` — thin decorator hierarchy.
  Useful but currently independent from the rest of the logging system.

**Logging**
- `Logger.hpp` — compile-time macros (`LOG_CTOR`, `LOG_DTOR`, etc.) + factory
  functions (`InfoLog`, `WarnLog`, etc.). Uses `static TermStyle` inside macros
  which is fragile.
- `log.cpp` — a complete async logger with a full decorator chain (`Logger`
  interface, `TimestampDecorator`, `ThreadIdDecorator`, `LogColorDecorator`,
  `FileSinkDecorator`, `AsyncBatchDecorator`, `PatternFormatterDecorator`,
  `LoggerBuilder`). This is excellent but trapped in a `.cpp` with no clean
  header interface.

**Testing**
- `postman.hpp/cpp` — `TestReport` singleton with `ASSERT_R` macro and a
  beautiful UTF-8 table summary. Good but tightly coupled to global state.
- `assertion.hpp/cpp` — `testing::AssertionResult` with operator chaining.
- `test_data.hpp` — `Gens::FmtGen` generating printf format strings.
  Narrow scope — only useful for printf testing.

**Memory**
- `LeakGuard.hpp/cpp` — overrides global `operator new/delete`, maintains a
  fixed-size ledger, supports snapshots. Works but is globally invasive.
- `leaks.hpp` — `memcheck` namespace with strategy/observer/RAII patterns.
  Uses C++17 (`std::string_view`, `std::atomic`, `std::unique_ptr`).
  Conflicts with lab code which targets C++98.

**Strings**
- `ft_string.hpp/cpp` — UTF-8 decoding, `decode_utf8_at`, `append_utf8`,
  `strcase_toggle`, `ft_str_to_wstr`. Also contains `testing::internal::String`
  (unrelated — should be separated).

**Data (lab)**
- `Database.hpp` — full in-memory database with `Table`, `Row`, `Column`,
  `TableRenderer`, `CsvParser`, `RenderConfig`. Excellent but in `lab/`.
- `Database_utils.hpp` — `Query`, `Transform`, `Statistics`, `Export::toCsv/Html/Markdown`.
- `csv.hpp` — separate CSV parser with type inference. Duplicates `CsvParser`.
- `Date.hpp`, `DateRange`, `DateUtils` — comprehensive date library.

**Misc**
- `message.hpp` — `tester::Message`: a streamable colored message builder. Good.
- `symbolizer.hpp` — `symbolizer::symbolize(void*)` via addr2line. Good.
- `libcpp.h` — master include. Will be regenerated for new structure.

### Critical Problems to Fix

1. **Two incompatible color systems**: `Srgb` and `Colors::RGB` that cannot
   interoperate. `Colors::RGB` must be absorbed into `Srgb`.
2. **No unified namespace**: Code uses `tester`, `testing`, `testing::internal`,
   `Colors`, `memcheck`, `CSV`, `Unicode`, `Style`, `Query`, `Transform`
   simultaneously.
3. **Logger split**: Logger.hpp macros and log.cpp async chain are disconnected.
   `static TermStyle _ts` inside macros is a hidden singleton.
4. **Dual table renderers**: `TermTable` and `Database::TableRenderer` implement
   the same feature independently.
5. **No error type**: No `Result<T,E>` or `Expected<T,E>`. Errors propagate
   through return codes, exceptions, or stderr print-and-continue.
6. **No interfaces for critical abstractions**: `LeakGuard` is not swappable.
   `TestReport` is a global singleton. No dependency injection anywhere.
7. **Standard mismatch**: `leaks.hpp` uses C++17, lab uses C++98, main lib
   uses C++17. Pick one: **C++17 throughout**.
8. **`testing::internal::String` living inside `ft_string.hpp`**: misplaced.
9. **`Colors::RGB::toAnsi()` returns a 4-bit ANSI code** while `Srgb::toAnsi()`
   returns a 24-bit true-color code. The fallback quality difference is hidden.
10. **No build system**: currently just a flat Makefile with a hardcoded file list.

---

## 2. Target Architecture

### Directory Layout

```
libcpp/
├── CMakeLists.txt                   ← modern CMake, installs as a proper lib
├── Makefile                         ← 42-style fallback (delegates to cmake)
├── include/
│   └── libcpp/
│       ├── libcpp.hpp               ← master include (auto-generated)
│       ├── core/
│       │   ├── result.hpp           ← Result<T,E>, Error, chain operators
│       │   ├── option.hpp           ← Option<T> (nullable without nullptr)
│       │   ├── signal.hpp           ← Signal<Args...> typed synchronous slots
│       │   ├── property.hpp         ← Property<T> observable reactive value
│       │   ├── arena.hpp            ← Arena<T> index-based allocator
│       │   └── types.hpp            ← common typedefs, platform macros
│       ├── term/
│       │   ├── color.hpp            ← unified Srgb (absorbs Colors::RGB)
│       │   ├── style.hpp            ← TermStyle + TermConf (merged)
│       │   ├── table.hpp            ← unified TermTable (replaces both)
│       │   ├── writer.hpp           ← TermWriter markdown parser
│       │   ├── progress.hpp         ← NEW: ProgressBar, Spinner
│       │   └── tree.hpp             ← NEW: terminal tree renderer
│       ├── log/
│       │   ├── logger.hpp           ← Logger interface + all decorators
│       │   ├── builder.hpp          ← LoggerBuilder (fluent API)
│       │   ├── macros.hpp           ← LOG_CTOR / LOG_DTOR etc.
│       │   └── sinks.hpp            ← Console, File, Null sinks
│       ├── test/
│       │   ├── suite.hpp            ← TestSuite (replaces global TestReport)
│       │   ├── assert.hpp           ← ASSERT_R + AssertionResult
│       │   ├── snapshot.hpp         ← NEW: Snapshot / golden-file testing
│       │   ├── mock.hpp             ← NEW: Mock<Sig> + Spy<F>
│       │   ├── fuzzer.hpp           ← NEW: property-based fuzzing
│       │   └── coverage.hpp         ← NEW: branch coverage tracker
│       ├── bench/
│       │   ├── timer.hpp            ← ScopeTimer, HighResTimer
│       │   ├── benchmark.hpp        ← NEW: Benchmark runner with statistics
│       │   └── profiler.hpp         ← NEW: Hierarchical call profiler
│       ├── mem/
│       │   ├── leak_guard.hpp       ← LeakGuard (refactored, non-global mode)
│       │   ├── arena.hpp            ← Arena allocator (forwarded from core)
│       │   └── pool.hpp             ← NEW: fixed-size object pool
│       ├── async/
│       │   ├── thread_pool.hpp      ← NEW: ThreadPool<N>
│       │   ├── event_bus.hpp        ← NEW: EventBus publish-subscribe
│       │   └── channel.hpp          ← NEW: Channel<T> producer-consumer
│       ├── util/
│       │   ├── arg_parser.hpp       ← NEW: CLI argument parser
│       │   ├── config.hpp           ← NEW: INI/key-value config
│       │   ├── date.hpp             ← Date (promoted from lab)
│       │   └── csv.hpp              ← unified CSV (merged csv.hpp + CsvParser)
│       ├── str/
│       │   ├── utf8.hpp             ← decode_utf8_at, append_utf8, visWidth
│       │   ├── format.hpp           ← Message (promoted from tester::Message)
│       │   └── case.hpp             ← strcase_toggle, to_upper, to_lower
│       └── data/
│           ├── database.hpp         ← Database + Table (promoted from lab)
│           ├── query.hpp            ← Query, Transform, Statistics
│           └── export.hpp           ← Export (CSV, HTML, Markdown)
└── src/
    ├── term/color.cpp
    ├── term/style.cpp
    ├── term/table.cpp
    ├── term/writer.cpp
    ├── term/progress.cpp
    ├── term/tree.cpp
    ├── log/logger.cpp
    ├── test/suite.cpp
    ├── test/snapshot.cpp
    ├── bench/benchmark.cpp
    ├── bench/profiler.cpp
    ├── mem/leak_guard.cpp
    ├── async/thread_pool.cpp
    ├── async/event_bus.cpp
    ├── util/arg_parser.cpp
    ├── util/config.cpp
    ├── str/utf8.cpp
    └── data/database.cpp
```

### Unified Namespace Strategy

Everything lives under `libcpp::`. Sub-namespaces are minimal and meaningful:

```cpp
libcpp::           // core, term, log, str primitives (flat access)
libcpp::test::     // testing framework
libcpp::bench::    // benchmarking
libcpp::async::    // concurrency
libcpp::data::     // database, csv, query
libcpp::util::     // argparser, config, date
```

No more `tester`, `testing::internal`, `Colors`, `memcheck`, `CSV`, `Unicode`,
`Style`, `Query`, `Transform`, `Gens` namespaces.

---

## 3. New Features (Nothing Below Exists Yet)

### 3.1 `libcpp::Result<T, E>` — Error Propagation

Rust-inspired error type. No exceptions required.

```cpp
// include/libcpp/core/result.hpp
namespace libcpp {

template<typename T, typename E = std::string>
class Result {
public:
    static Result ok(T val);
    static Result err(E error);

    bool is_ok() const;
    bool is_err() const;

    T& unwrap();               // terminates if err
    T& unwrap_or(T& fallback);
    E& error();

    template<typename F>
    auto map(F fn) -> Result<decltype(fn(std::declval<T>())), E>;

    template<typename F>
    auto and_then(F fn) -> decltype(fn(std::declval<T>()));

    template<typename F>
    Result map_err(F fn);

private:
    bool _ok;
    union { T _val; E _err; };
};

// Convenience macro (like Rust's ?)
#define TRY(expr) \
    ({ auto _r = (expr); if (_r.is_err()) return Result<decltype(_r.unwrap()),decltype(_r.error())>::err(_r.error()); _r.unwrap(); })

} // namespace libcpp
```

**Usage in libcpp itself**: every function that can fail (file I/O, config parsing,
CSV loading, arg parsing) returns `Result<T>` instead of throwing or returning bool.

---

### 3.2 `libcpp::Signal<Args...>` — Typed Synchronous Signal-Slot

Different from EventBus (see 3.9). Signal is typed, synchronous, and owned.

```cpp
// include/libcpp/core/signal.hpp
namespace libcpp {

template<typename... Args>
class Signal {
public:
    using Slot = std::function<void(Args...)>;
    using ConnectionId = size_t;

    ConnectionId connect(Slot slot);
    void disconnect(ConnectionId id);
    void emit(Args... args);
    size_t slot_count() const;
    void clear();

private:
    std::map<ConnectionId, Slot> _slots;
    ConnectionId _next_id = 0;
};

} // namespace libcpp
```

**Example usage in libcpp**:
```cpp
libcpp::Signal<int, std::string> on_log_entry;
on_log_entry.connect([](int level, const std::string& msg) {
    // react to each log line without coupling to logger
});
```

---

### 3.3 `libcpp::Property<T>` — Observable Reactive Value

A typed value that notifies observers on change. Think Qt's properties, minimal.

```cpp
// include/libcpp/core/property.hpp
namespace libcpp {

template<typename T>
class Property {
public:
    explicit Property(T initial);

    // Read
    const T& get() const;
    operator const T&() const;

    // Write (fires on_change if value actually differs)
    Property& set(T val);
    Property& operator=(T val);

    // Observe
    Signal<T, T> on_change;  // emits (old_val, new_val)

    // Computed property (derived from another)
    template<typename U, typename F>
    static Property<U> computed(const Property<T>& source, F transform);

private:
    T _val;
};

} // namespace libcpp
```

---

### 3.4 `libcpp::test::Snapshot` — Golden-File Snapshot Testing

Capture output, store it as a reference file, diff on subsequent runs.
Like Jest snapshots or ApprovalTests, but lightweight and terminal-aware.

```cpp
// include/libcpp/test/snapshot.hpp
namespace libcpp::test {

class Snapshot {
public:
    // Configuration
    explicit Snapshot(const std::string& snapshot_dir = ".snapshots");
    void set_update_mode(bool update);   // overwrite snapshots on run
    void set_strip_ansi(bool strip);     // ignore ANSI codes in comparison

    // Core: call with a named test and the actual output
    bool match(const std::string& test_name, const std::string& actual);

    // Convenience: capture stdout during fn() and snapshot it
    template<typename F>
    bool capture_and_match(const std::string& test_name, F fn);

    // Report
    int pass_count() const;
    int fail_count() const;
    int new_count() const;  // snapshots created for the first time
    void print_summary() const;

private:
    std::string _snapshot_dir;
    bool _update_mode;
    bool _strip_ansi;

    std::string load_snapshot(const std::string& name) const;
    void save_snapshot(const std::string& name, const std::string& content);
    std::string diff(const std::string& expected, const std::string& actual) const;
    std::string myers_diff(const std::string& a, const std::string& b) const;
    std::string strip_ansi_codes(const std::string& s) const;
};

} // namespace libcpp::test
```

**This requires implementing the Myers diff algorithm** (O(ND) shortest edit script)
to produce readable colored diffs when a snapshot fails.

---

### 3.5 `libcpp::test::Mock<Sig>` — Function Mock & Spy

Minimal mock/spy framework for C++ — no virtual required for simple cases.

```cpp
// include/libcpp/test/mock.hpp
namespace libcpp::test {

// Spy<F> wraps any callable and records every invocation
template<typename R, typename... Args>
class Spy {
public:
    explicit Spy(std::function<R(Args...)> impl = nullptr);

    // Call operator — records args and delegates to impl
    R operator()(Args... args);

    // Inspection
    int call_count() const;
    bool was_called() const;
    bool was_called_once() const;
    std::tuple<Args...> last_args() const;
    std::vector<std::tuple<Args...>> all_args() const;

    // Return value control
    Spy& returns(R val);
    Spy& returns_sequence(std::initializer_list<R> vals);  // each call next val
    Spy& throws(std::exception_ptr ex);

    // Expectations (checked at end of test)
    Spy& expect_called_times(int n);
    Spy& expect_called_with(Args... args);  // at least once
    bool verify() const;

    void reset();
};

// Mock<Interface> — virtual-dispatch mock (requires interface)
// Uses CRTP to auto-generate delegation
template<typename Interface>
class Mock : public Interface {
public:
    template<typename R, typename... Args>
    Spy<R, Args...>& on(R (Interface::*method)(Args...));

    bool verify_all() const;

private:
    std::map<void*, std::unique_ptr<SpyBase>> _spies;
};

} // namespace libcpp::test
```

---

### 3.6 `libcpp::test::Fuzzer` — Property-Based Test Generator

Generalizes the narrow `Gens::FmtGen` into a full property-based testing framework.

```cpp
// include/libcpp/test/fuzzer.hpp
namespace libcpp::test {

// Generators
namespace gen {
    Generator<int>         integers(int min = INT_MIN, int max = INT_MAX);
    Generator<double>      doubles(double min = -1e9, double max = 1e9);
    Generator<std::string> strings(size_t max_len = 100);
    Generator<std::string> ascii_strings(size_t max_len = 100);
    Generator<std::string> utf8_strings(size_t max_len = 50);
    Generator<bool>        bools();

    template<typename T>
    Generator<std::vector<T>> vectors(Generator<T> elem, size_t max_size = 20);

    template<typename T>
    Generator<T> one_of(std::initializer_list<T> choices);

    template<typename T>
    Generator<T> filtered(Generator<T> base, std::function<bool(T)> pred);
}

// Property runner
template<typename... Ts>
class Property {
public:
    explicit Property(std::function<bool(Ts...)> predicate);

    Property& with(Generator<Ts>... gens);
    Property& iterations(size_t n);          // default: 100
    Property& seed(uint64_t s);              // reproducible runs
    Property& on_failure(std::function<void(Ts...)> reporter);

    struct Result {
        bool passed;
        int  runs;
        int  shrink_steps;
        std::tuple<Ts...> counterexample;  // first failing input
    };

    Result check() const;
};

// Shrinking: when a counterexample is found, automatically reduce it
// to the minimal failing case (binary search style)

} // namespace libcpp::test
```

---

### 3.7 `libcpp::bench::Benchmark` — Statistical Benchmark Runner

A proper benchmark framework with warmup, iterations, statistical analysis,
and terminal output. Nothing like this exists in the current lib.

```cpp
// include/libcpp/bench/benchmark.hpp
namespace libcpp::bench {

struct BenchResult {
    std::string name;
    size_t iterations;
    double mean_ns;
    double median_ns;
    double stddev_ns;
    double min_ns;
    double max_ns;
    double p95_ns;
    double p99_ns;
    double throughput_ops_per_sec;
};

class Benchmark {
public:
    explicit Benchmark(const std::string& name);

    Benchmark& warmup(size_t iters);       // default: 10
    Benchmark& iterations(size_t iters);   // default: 1000
    Benchmark& setup(std::function<void()> fn);
    Benchmark& teardown(std::function<void()> fn);

    template<typename F>
    BenchResult run(F fn);

    static void compare(std::initializer_list<BenchResult> results);
    static void print_result(const BenchResult& r);
    static void print_comparison_table(const std::vector<BenchResult>& results);
};

class BenchSuite {
public:
    explicit BenchSuite(const std::string& name);

    template<typename F>
    BenchSuite& add(const std::string& name, F fn);

    std::vector<BenchResult> run_all();
    void print_all() const;
};

// RAII scope timer for quick inline benchmarks
class ScopeTimer {
public:
    explicit ScopeTimer(const std::string& label, bool auto_print = true);
    ~ScopeTimer();
    double elapsed_ns() const;
    double elapsed_us() const;
    double elapsed_ms() const;

private:
    std::string _label;
    bool _auto_print;
    std::chrono::high_resolution_clock::time_point _start;
};

} // namespace libcpp::bench
```

---

### 3.8 `libcpp::bench::Profiler` — Hierarchical Call Profiler

Instrument code paths and get a call-tree with timing breakdown.

```cpp
// include/libcpp/bench/profiler.hpp
namespace libcpp::bench {

class Profiler {
public:
    static Profiler& global();

    void begin(const std::string& scope_name);
    void end(const std::string& scope_name);

    struct Node {
        std::string name;
        double total_ms;
        double self_ms;
        size_t call_count;
        std::vector<Node> children;
        double pct_of_parent;
    };

    Node root() const;
    void reset();
    void print_tree(int max_depth = 10) const;
    void print_flat(size_t top_n = 20) const;

    // Export to Chrome trace format (chrome://tracing)
    std::string to_chrome_trace() const;

    // RAII scope guard
    struct Scope {
        explicit Scope(const std::string& name);
        ~Scope();
    private:
        std::string _name;
    };
};

// Macro for zero-overhead when disabled
#ifdef LIBCPP_PROFILING
  #define PROFILE_SCOPE(name) libcpp::bench::Profiler::Scope _prof_##__LINE__(name)
  #define PROFILE_BEGIN(name) libcpp::bench::Profiler::global().begin(name)
  #define PROFILE_END(name)   libcpp::bench::Profiler::global().end(name)
#else
  #define PROFILE_SCOPE(name) ((void)0)
  #define PROFILE_BEGIN(name) ((void)0)
  #define PROFILE_END(name)   ((void)0)
#endif

} // namespace libcpp::bench
```

---

### 3.9 `libcpp::async::EventBus` — Publish-Subscribe

Type-safe event bus. Different from `Signal` (which is owned/synchronous).
EventBus is global, decoupled, and optionally async.

```cpp
// include/libcpp/async/event_bus.hpp
namespace libcpp::async {

// Events are plain structs tagged by type
struct LogEvent   { int level; std::string msg; };
struct ErrorEvent { std::string what; std::string file; int line; };
struct TestEvent  { std::string suite; std::string label; bool passed; };

class EventBus {
public:
    static EventBus& global();

    template<typename Event>
    using Handler = std::function<void(const Event&)>;

    template<typename Event>
    size_t subscribe(Handler<Event> handler);

    template<typename Event>
    void unsubscribe(size_t id);

    template<typename Event>
    void publish(Event event);

    template<typename Event>
    void publish_async(Event event);  // enqueues for background delivery

    // Pause/resume all delivery
    void pause();
    void resume();

    // For testing: capture events
    template<typename Event>
    std::vector<Event> captured() const;
    void start_capture();
    void stop_capture();

private:
    std::map<std::type_index, std::vector<std::pair<size_t, std::any>>> _subscribers;
    std::mutex _mtx;
    bool _paused = false;
};

} // namespace libcpp::async
```

---

### 3.10 `libcpp::async::ThreadPool` — Task Execution

```cpp
// include/libcpp/async/thread_pool.hpp
namespace libcpp::async {

class ThreadPool {
public:
    explicit ThreadPool(size_t n_workers = std::thread::hardware_concurrency());
    ~ThreadPool();

    template<typename F, typename... Args>
    auto submit(F fn, Args... args) -> std::future<decltype(fn(args...))>;

    template<typename F>
    void submit_and_forget(F fn);

    // Parallel map over a collection
    template<typename It, typename F>
    void parallel_for(It first, It last, F fn);

    void wait_all();
    void stop();

    size_t worker_count() const;
    size_t pending_tasks() const;
    size_t completed_tasks() const;

private:
    std::vector<std::thread> _workers;
    std::queue<std::function<void()>> _tasks;
    std::mutex _mtx;
    std::condition_variable _cv;
    std::atomic<bool> _stop;
    std::atomic<size_t> _pending;
    std::atomic<size_t> _completed;
};

} // namespace libcpp::async
```

---

### 3.11 `libcpp::async::Channel<T>` — Typed Producer-Consumer

```cpp
// include/libcpp/async/channel.hpp
namespace libcpp::async {

template<typename T>
class Channel {
public:
    explicit Channel(size_t capacity = 0);  // 0 = unbounded

    // Producer
    void send(T val);
    bool try_send(T val);                   // non-blocking (bounded only)
    void close();

    // Consumer
    std::optional<T> recv();                // blocks; returns nullopt if closed
    bool try_recv(T& out);                  // non-blocking
    std::vector<T> recv_all();              // drain all available

    bool is_closed() const;
    size_t size() const;

private:
    std::queue<T> _queue;
    std::mutex _mtx;
    std::condition_variable _cv_send;
    std::condition_variable _cv_recv;
    size_t _capacity;
    bool _closed = false;
};

} // namespace libcpp::async
```

---

### 3.12 `libcpp::util::ArgParser` — CLI Argument Parsing

No CLI parser exists anywhere in libcpp. Every program reimplements this.

```cpp
// include/libcpp/util/arg_parser.hpp
namespace libcpp::util {

class ArgParser {
public:
    explicit ArgParser(const std::string& program_name);
    ArgParser& description(const std::string& desc);
    ArgParser& epilog(const std::string& text);

    // Define arguments
    ArgParser& flag(const std::string& name,
                    const std::string& shorthand,
                    const std::string& help);

    ArgParser& option(const std::string& name,
                      const std::string& shorthand,
                      const std::string& help,
                      const std::string& default_val = "");

    ArgParser& positional(const std::string& name,
                          const std::string& help,
                          bool required = true);

    ArgParser& subcommand(const std::string& name,
                          const std::string& help,
                          std::function<void(ArgParser&)> configure);

    // Parse
    Result<void> parse(int argc, char** argv);

    // Access results
    bool has(const std::string& name) const;
    std::string get(const std::string& name) const;
    int         get_int(const std::string& name) const;
    double      get_double(const std::string& name) const;
    std::string subcommand_used() const;

    // Help
    std::string help_text() const;
    void print_help() const;

private:
    struct Arg { std::string name, shorthand, help, default_val, type; bool required; };
    std::vector<Arg> _flags;
    std::vector<Arg> _options;
    std::vector<Arg> _positionals;
    std::map<std::string, std::string> _values;
    std::string _program, _description, _epilog, _subcommand_used;
};

} // namespace libcpp::util
```

---

### 3.13 `libcpp::util::Config` — INI/Key-Value Config Parser

```cpp
// include/libcpp/util/config.hpp
namespace libcpp::util {

class Config {
public:
    // Load from different sources
    static Result<Config> from_file(const std::string& path);
    static Result<Config> from_string(const std::string& content);
    static Config empty();

    // Merge configs (later overrides earlier)
    Config& merge(const Config& other);

    // Section access
    bool has_section(const std::string& section) const;
    std::vector<std::string> sections() const;

    // Key access (with optional section)
    bool has(const std::string& key) const;
    bool has(const std::string& section, const std::string& key) const;

    std::string      get(const std::string& key, const std::string& def = "") const;
    int              get_int(const std::string& key, int def = 0) const;
    double           get_double(const std::string& key, double def = 0.0) const;
    bool             get_bool(const std::string& key, bool def = false) const;
    std::vector<std::string> get_list(const std::string& key) const; // comma-separated

    // Write
    void set(const std::string& key, const std::string& val);
    void set(const std::string& section, const std::string& key, const std::string& val);

    // Save
    Result<void> save(const std::string& path) const;
    std::string to_string() const;

private:
    std::map<std::string, std::map<std::string, std::string>> _data;
    std::string _default_section = "global";
};

} // namespace libcpp::util
```

---

### 3.14 `libcpp::term::ProgressBar` & `libcpp::term::Spinner`

Terminal progress visualization. Nothing like this exists in the lib.

```cpp
// include/libcpp/term/progress.hpp
namespace libcpp::term {

class ProgressBar {
public:
    struct Style {
        std::string fill;        // "█" default
        std::string empty;       // "░" default
        std::string left;        // "[" default
        std::string right;       // "]" default
        Srgb fill_color;
        Srgb label_color;
        int width = 40;
        bool show_percent = true;
        bool show_eta = true;
        bool show_rate = false;
    };

    ProgressBar(size_t total, const std::string& label = "");
    ProgressBar(size_t total, const Style& style);

    void tick(size_t n = 1);
    void set(size_t n);
    void finish();

    void set_label(const std::string& label);
    void set_postfix(const std::string& text);  // extra text after bar

    double percent() const;
    double rate() const;             // units per second
    double eta_seconds() const;

    std::string render() const;      // full bar as string
    void print_inline() const;       // \r to overwrite current line
    void println() const;            // prints and advances line

private:
    size_t _total, _current;
    Style _style;
    std::string _label, _postfix;
    std::chrono::high_resolution_clock::time_point _start;
};

class Spinner {
public:
    enum class Kind { Dots, Line, Arc, Arrow, Bounce };

    explicit Spinner(const std::string& label = "", Kind kind = Kind::Dots);

    void start();
    void stop(const std::string& final_msg = "");
    void update_label(const std::string& label);

    std::string render() const;

private:
    std::string _label;
    Kind _kind;
    size_t _frame;
    std::thread _thread;
    std::atomic<bool> _running;
    mutable std::mutex _mtx;
};

} // namespace libcpp::term
```

---

### 3.15 `libcpp::term::Tree` — Terminal Tree Renderer

Displays hierarchical data (file trees, profiler output, test results) using
box-drawing characters. Nothing like this exists in the lib.

```cpp
// include/libcpp/term/tree.hpp
namespace libcpp::term {

struct TreeNode {
    std::string label;
    std::string detail;        // optional dim text after label
    Srgb label_color;
    Srgb detail_color;
    std::vector<TreeNode> children;

    // Builder API
    TreeNode& add(const std::string& label, const std::string& detail = "");
    TreeNode& child(TreeNode node);
};

class TreeRenderer {
public:
    struct Style {
        std::string branch    = "├── ";
        std::string last      = "└── ";
        std::string pipe      = "│   ";
        std::string blank     = "    ";
        bool show_counts      = false;   // append (N children)
        Srgb branch_color;
        int max_depth         = -1;      // -1 = unlimited
    };

    explicit TreeRenderer(Style style = {});
    std::string render(const TreeNode& root) const;
    void print(const TreeNode& root) const;

private:
    Style _style;
    void render_node(std::ostringstream& out,
                     const TreeNode& node,
                     const std::string& prefix,
                     bool is_last,
                     int depth) const;
};

} // namespace libcpp::term
```

---

### 3.16 `libcpp::mem::Pool<T>` — Fixed-Size Object Pool

```cpp
// include/libcpp/mem/pool.hpp
namespace libcpp::mem {

template<typename T, size_t Capacity>
class Pool {
public:
    Pool();
    ~Pool();

    template<typename... Args>
    T* alloc(Args&&... args);

    void free(T* ptr);

    size_t available() const;
    size_t used() const;
    bool empty() const;
    bool full() const;

    void reset();   // destruct all, reclaim all slots

private:
    alignas(T) char _storage[Capacity * sizeof(T)];
    bool _used[Capacity];
    size_t _free_count;
};

} // namespace libcpp::mem
```

---

### 3.17 `libcpp::test::Coverage` — Branch Coverage Tracker

Instrument code to track which branches were exercised during a test run.

```cpp
// include/libcpp/test/coverage.hpp
namespace libcpp::test {

class Coverage {
public:
    static Coverage& global();

    void hit(const char* file, int line, const char* label = "");
    void branch(const char* file, int line, bool taken, const char* label = "");

    struct CoverageEntry {
        std::string file;
        int line;
        std::string label;
        size_t hit_count;
    };

    struct BranchEntry {
        std::string file;
        int line;
        std::string label;
        size_t true_count;
        size_t false_count;
        bool fully_covered() const;
    };

    std::vector<CoverageEntry> uncovered_lines() const;
    std::vector<BranchEntry>  uncovered_branches() const;

    double line_coverage_pct() const;
    double branch_coverage_pct() const;

    void print_report() const;
    Result<void> save_lcov(const std::string& path) const;
    void reset();
};

// Macros (zero overhead when LIBCPP_COVERAGE is not defined)
#ifdef LIBCPP_COVERAGE
  #define COV_HIT(label)    libcpp::test::Coverage::global().hit(__FILE__, __LINE__, label)
  #define COV_BRANCH(taken) libcpp::test::Coverage::global().branch(__FILE__, __LINE__, (taken))
#else
  #define COV_HIT(label)    ((void)0)
  #define COV_BRANCH(taken) ((void)0)
#endif

} // namespace libcpp::test
```

---

### 3.18 `libcpp::test::TestSuite` — Decoupled Test Runner

Replace the global `TestReport` singleton with composable, nestable test suites.

```cpp
// include/libcpp/test/suite.hpp
namespace libcpp::test {

class TestSuite {
public:
    explicit TestSuite(const std::string& name);

    // Registration (no macros needed — just lambdas)
    TestSuite& test(const std::string& name, std::function<void()> fn);
    TestSuite& before_each(std::function<void()> fn);
    TestSuite& after_each(std::function<void()> fn);
    TestSuite& before_all(std::function<void()> fn);
    TestSuite& after_all(std::function<void()> fn);
    TestSuite& suite(TestSuite sub);   // nested suites

    // Execution
    struct RunResult {
        int total, passed, failed, skipped;
        double duration_ms;
        std::vector<std::string> failures;
    };

    RunResult run() const;
    void run_and_print() const;   // runs + shows postman-style table

    // Assertions that integrate with the suite
    static void expect_true(bool val, const std::string& msg = "");
    static void expect_eq(auto a, auto b, const std::string& msg = "");
    static void expect_throws(std::function<void()> fn, const std::string& msg = "");
    static void fail(const std::string& msg);
    static void skip(const std::string& msg);

private:
    struct TestCase { std::string name; std::function<void()> fn; };
    std::string _name;
    std::vector<TestCase> _tests;
    std::vector<TestSuite> _suites;
    std::function<void()> _before_each, _after_each, _before_all, _after_all;
};

} // namespace libcpp::test
```

---

## 4. Refactoring Requirements (Existing Code)

### 4.1 Unify Color Systems

Absorb `Colors::RGB` features into `Srgb`:

```cpp
class Srgb {
public:
    // --- existing ---
    Srgb(); Srgb(unsigned int rgb); Srgb(unsigned char r, unsigned char g, unsigned char b);

    // --- absorb from Colors::RGB ---
    static Srgb from_hex(const std::string& hex);
    static Srgb parse(const std::string& s);  // hex, rgb(r,g,b), or name
    static Srgb mix(const Srgb& a, const Srgb& b, double ratio = 0.5);

    Srgb lighten(double factor) const;
    Srgb darken(double factor) const;
    Srgb saturate(double factor) const;
    Srgb complement() const;
    double luminance() const;
    int to_ansi_256() const;   // approximate to nearest 256-color index

    // --- absorb Colors::Palette ---
    static Srgb named(const std::string& name);  // "red", "cyan", etc.

    // --- existing ---
    std::string to_ansi_fg() const;   // rename from toAnsi()
    std::string to_ansi_bg() const;   // rename from toBgAnsi()
    std::string to_hex() const;       // rename from toHex()
};

// Gradient stays as standalone class but works with Srgb
class Gradient { /* ... */ };
```

Delete `colors.hpp` entirely after migration.

### 4.2 Consolidate Logger

Expose the full async chain from `log.cpp` through a clean header:

```cpp
// include/libcpp/log/logger.hpp
namespace libcpp::log {

// The Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(int level, const std::string& msg) = 0;
};

// LoggerBuilder returns std::shared_ptr<ILogger>
class LoggerBuilder { /* ... all existing decorators ... */ };

// Global logger (replaces static TermStyle pattern in Logger.hpp macros)
void set_global(std::shared_ptr<ILogger> logger);
ILogger& global();

// Level constants
constexpr int TRACE = 0, DEBUG = 1, INFO = 2, WARN = 3, ERROR = 4, FATAL = 5;

} // namespace libcpp::log
```

Macros in `log/macros.hpp` then route through `libcpp::log::global()`:
```cpp
#define LOG_CTOR()  libcpp::log::global().log(libcpp::log::INFO, \
    _lg_build_meta(__PRETTY_FUNCTION__, __FILE__, __LINE__) + "constructor called")
```

### 4.3 Unify Table Renderers

`TermTable` (terminal-native, index-based) and `Database::TableRenderer`
(Database-aware, style-rich) must share a common rendering backend.
Extract a `libcpp::term::TableCore` that both delegate to.

### 4.4 Make LeakGuard Non-Invasive

Add an opt-in mode that doesn't override global `operator new/delete`:

```cpp
class LeakGuard {
public:
    enum class Mode {
        GLOBAL_OVERRIDE,    // current behavior: intercepts all new/delete
        SCOPED_ONLY,        // only tracks allocations through LeakGuard::alloc<T>()
    };
    static void set_mode(Mode m);

    // Scoped allocation API (for SCOPED_ONLY mode)
    template<typename T, typename... Args>
    static T* alloc(Args&&... args);
    static void free_ptr(void* ptr);

    // ... existing snapshot/report API unchanged
};
```

### 4.5 Separate `testing::internal::String` from `ft_string`

Move `testing::internal::String` into `include/libcpp/test/assert.hpp`
where it belongs. `ft_string.hpp` becomes pure UTF-8/string utilities only.

---

## 5. Integration: How the New Features Connect

### 5.1 EventBus hooks into Logger

```cpp
// logger publishes TestEvent on pass/fail for downstream consumers
libcpp::async::EventBus::global().subscribe<libcpp::async::TestEvent>(
    [](const auto& ev) {
        if (!ev.passed) send_slack_alert(ev.suite + ": FAILED " + ev.label);
    }
);
```

### 5.2 Benchmark + Profiler integration

`Benchmark::run()` automatically wraps the function in a `Profiler::Scope`
if `LIBCPP_PROFILING` is defined, so you get call tree + timing in one pass.

### 5.3 Snapshot + DiffUtil for test output

```cpp
libcpp::test::Snapshot snap(".snapshots");
snap.match("database_render", db.render(cfg));
// On failure: prints Myers diff with +/- lines colored green/red
```

### 5.4 ArgParser drives Config

```cpp
parser.option("--config", "-c", "Path to config file", "~/.libcpp.ini");
auto cfg = libcpp::util::Config::from_file(parser.get("config")).unwrap_or(Config::empty());
```

### 5.5 Coverage integrates with TestSuite

```cpp
Coverage::global().reset();
suite.run();
Coverage::global().print_report();
// Output: "87.3% line coverage, 72.1% branch coverage"
```

---

## 6. Commit Plan (200 commits)

### Phase 1 — CMake + Namespace + Core Types (1–30)
```
1.  chore: set up CMakeLists.txt with libcpp as INTERFACE/static target
2.  chore: create full directory skeleton for include/libcpp/
3.  chore: add Makefile delegating to cmake for 42 compatibility
4.  refactor: create include/libcpp/core/types.hpp with common typedefs
5.  feat(core): implement Result<T,E> with ok/err static constructors
6.  feat(core): add Result::map, Result::and_then chain operators
7.  feat(core): add Result::map_err, Result::unwrap_or
8.  test(core): unit tests for Result success and error paths
9.  test(core): verify Result chain operators compose correctly
10. feat(core): implement Option<T> with some/none constructors
11. feat(core): add Option::map, Option::or_else, Option::unwrap_or
12. test(core): unit tests for Option
13. feat(core): implement Signal<Args...> with connect/disconnect/emit
14. test(core): verify Signal fires all connected slots
15. test(core): verify Signal disconnect removes specific slot
16. feat(core): implement Property<T> with on_change Signal
17. feat(core): implement Property<T>::computed derived property
18. test(core): verify Property fires on_change only on actual change
19. feat(core): implement Arena<T> index-based allocator (for ft_lex reuse)
20. test(core): Arena alloc/get/get_mut roundtrip
21. refactor: migrate Srgb into libcpp:: namespace, rename methods snake_case
22. feat(term): absorb Colors::RGB::lighten/darken/saturate/complement into Srgb
23. feat(term): absorb Colors::Gradient into libcpp::term with Srgb stops
24. feat(term): add Srgb::from_hex, Srgb::parse, Srgb::named
25. feat(term): add Srgb::to_ansi_256 fallback
26. feat(term): add Srgb::luminance and Srgb::mix
27. test(term): Srgb color math roundtrips
28. chore: delete colors.hpp, update all includes
29. refactor: move testing::internal::String out of ft_string into test/assert.hpp
30. refactor: ft_string.hpp becomes pure libcpp::str UTF-8 utilities
```

### Phase 2 — Terminal Rework (31–60)
```
31. refactor: migrate TermUtils into libcpp::term, snake_case API
32. refactor: migrate TermStyle into libcpp::term with new color types
33. refactor: migrate TermConf into libcpp::term
34. feat(term): extract TableCore shared rendering backend
35. refactor: rewrite TermTable on top of TableCore
36. refactor: rewrite Database::TableRenderer on top of TableCore
37. test(term): TermTable and Database TableRenderer produce identical output
38. refactor: migrate TermWriter into libcpp::term
39. test(term): TermWriter markdown parser roundtrip
40. feat(term): implement ProgressBar with fill/empty/percent display
41. feat(term): implement ProgressBar ETA calculation
42. test(term): ProgressBar renders correctly at 0%, 50%, 100%
43. feat(term): implement ProgressBar::rate() ops/sec
44. feat(term): implement Spinner with Dots/Line/Arc/Arrow/Bounce kinds
45. feat(term): Spinner background thread for live animation
46. test(term): Spinner renders distinct frames
47. feat(term): implement TreeNode builder API
48. feat(term): implement TreeRenderer with box-drawing characters
49. feat(term): TreeRenderer depth limiting
50. test(term): TreeRenderer file tree output matches expected
51. feat(term): TreeRenderer exports to JSON
52. refactor: consolidate Decorator/VerboseDecorator/ColorDecorator under libcpp::term
53. test(term): decorated messages render correct ANSI codes
54. feat(term): add libcpp::term::strip_ansi(str) utility function
55. feat(term): add libcpp::term::visible_width(str) (ANSI-aware)
56. test(term): visible_width ignores ANSI escape sequences
57. refactor: migrate log.cpp Logger chain into include/libcpp/log/logger.hpp
58. refactor: expose LoggerBuilder in include/libcpp/log/builder.hpp
59. refactor: set_global / global() in libcpp::log for macro routing
60. refactor: migrate LOG_CTOR/LOG_DTOR etc. macros to route via global()
```

### Phase 3 — Testing Framework (61–100)
```
61. refactor: create libcpp::test::TestSuite class
62. feat(test): TestSuite::test() registration with lambdas
63. feat(test): TestSuite::before_each / after_each hooks
64. feat(test): TestSuite::before_all / after_all hooks
65. feat(test): TestSuite nested sub-suites
66. feat(test): TestSuite::run() returns RunResult struct
67. feat(test): TestSuite::run_and_print() integrates postman table
68. test(test): verify nested suite results aggregate correctly
69. feat(test): TestSuite static assertion helpers (expect_true, expect_eq)
70. feat(test): TestSuite expect_throws helper
71. feat(test): TestSuite skip and fail helpers
72. refactor: TestReport becomes output backend for TestSuite (not standalone)
73. refactor: ASSERT_R macro updated to use libcpp::test::TestSuite::expect_true
74. test(test): ASSERT_R macro backward compatibility
75. feat(test): implement Snapshot class with snapshot_dir configuration
76. feat(test): Snapshot::match saves golden file on first run
77. feat(test): Snapshot::match compares against golden file on subsequent runs
78. feat(test): implement Myers diff algorithm in Snapshot
79. feat(test): Snapshot diff output with colored +/- lines
80. feat(test): Snapshot::strip_ansi mode for ANSI-agnostic comparison
81. feat(test): Snapshot update mode (overwrite all snapshots)
82. feat(test): Snapshot::print_summary shows new/pass/fail counts
83. test(test): Snapshot passes on identical output
84. test(test): Snapshot fails and shows diff on changed output
85. feat(test): implement Spy<R, Args...> callable wrapper
86. feat(test): Spy records call_count, last_args, all_args
87. feat(test): Spy::returns and Spy::returns_sequence
88. feat(test): Spy::expect_called_times and verify()
89. feat(test): Spy::expect_called_with matcher
90. test(test): Spy records invocations correctly
91. test(test): Spy verify() detects unmet expectations
92. feat(test): implement Generator<T> base class with next() + seed
93. feat(test): gen::integers, gen::doubles, gen::bools
94. feat(test): gen::strings, gen::ascii_strings, gen::utf8_strings
95. feat(test): gen::vectors, gen::one_of, gen::filtered
96. feat(test): implement Property runner with iterations + seed
97. feat(test): Property finds and reports counterexample on failure
98. feat(test): Property shrinks counterexample to minimal failing case
99. test(test): property always(n >= 0 for abs(n)) finds negative input
100. test(test): shrinker reduces "hello world" to "a" (minimal failing string)
```

### Phase 4 — Benchmarking & Profiling (101–130)
```
101. feat(bench): implement ScopeTimer with RAII auto-print
102. feat(bench): ScopeTimer elapsed_ns/us/ms accessors
103. test(bench): ScopeTimer measures sleep_for accurately to within 10%
104. feat(bench): implement Benchmark runner with warmup + iterations
105. feat(bench): Benchmark::run calculates mean, median, stddev, p95, p99
106. feat(bench): Benchmark::run calculates throughput ops/sec
107. feat(bench): Benchmark::print_result renders colored table row
108. feat(bench): Benchmark::compare renders speedup/slowdown table
109. test(bench): Benchmark mean is within 5% of known-duration function
110. feat(bench): implement BenchSuite with add() + run_all()
111. feat(bench): BenchSuite::print_all renders comparison table
112. feat(bench): implement Profiler with hierarchical scope tracking
113. feat(bench): Profiler thread-local call stack
114. feat(bench): Profiler::root() returns Node tree with total/self ms
115. feat(bench): Profiler::print_tree renders tree via libcpp::term::TreeRenderer
116. feat(bench): Profiler::print_flat lists top N hotspots
117. feat(bench): Profiler::to_chrome_trace() exports JSON for chrome://tracing
118. feat(bench): PROFILE_SCOPE / PROFILE_BEGIN / PROFILE_END macros
119. test(bench): nested scopes produce correct parent/child accounting
120. test(bench): self_ms = total_ms - sum of children ms
121. refactor: Profiler integrates with Benchmark::run when LIBCPP_PROFILING defined
122. feat(bench): Profiler::reset() clears all recorded data
123. test(bench): chrome trace output is valid JSON
124. feat(bench): Profiler captures call_count per scope
125. feat(bench): Profiler::pct_of_parent computed correctly
126. test(bench): 100% of total time accounted by root node
127. refactor: move run_logger_demo() into bench::BenchSuite example
128. feat(bench): Benchmark stores min/max observed values
129. feat(bench): BenchSuite output respects TermConf width setting
130. docs: add benchmark README with example usage
```

### Phase 5 — Async (131–155)
```
131. feat(async): implement ThreadPool with n_workers
132. feat(async): ThreadPool::submit returns std::future
133. feat(async): ThreadPool::submit_and_forget (fire and forget)
134. feat(async): ThreadPool::parallel_for over iterator range
135. feat(async): ThreadPool::wait_all blocks until queue drains
136. test(async): parallel_for processes all elements exactly once
137. test(async): submit future resolves with correct return value
138. feat(async): ThreadPool::pending_tasks and completed_tasks counters
139. feat(async): ThreadPool::stop terminates workers gracefully
140. feat(async): implement Channel<T> unbounded variant
141. feat(async): Channel::send / Channel::recv (blocking)
142. feat(async): Channel::try_send / Channel::try_recv (non-blocking)
143. feat(async): Channel::close + recv returns nullopt when closed+empty
144. feat(async): Channel bounded capacity variant with backpressure
145. test(async): single producer single consumer sends 10000 ints
146. test(async): closing channel unblocks all blocked recv calls
147. feat(async): implement EventBus with type-indexed subscriber map
148. feat(async): EventBus::subscribe<Event> returns subscription id
149. feat(async): EventBus::unsubscribe<Event>(id) removes handler
150. feat(async): EventBus::publish<Event> synchronous delivery
151. feat(async): EventBus::publish_async<Event> via ThreadPool
152. feat(async): EventBus capture mode for testing
153. test(async): EventBus delivers to all subscribers of matching type
154. test(async): EventBus does not deliver to wrong type subscribers
155. feat(async): Logger publishes LogEvent to EventBus on each log call
```

### Phase 6 — Utilities & Config (156–175)
```
156. feat(util): implement ArgParser with flag/option/positional
157. feat(util): ArgParser::parse returns Result<void> with error detail
158. feat(util): ArgParser::help_text generates formatted help
159. feat(util): ArgParser subcommand support
160. test(util): ArgParser parses --flag -f correctly
161. test(util): ArgParser error on missing required positional
162. feat(util): implement Config INI parser from string
163. feat(util): Config::from_file with Result error on IO failure
164. feat(util): Config section support [section] key=value
165. feat(util): Config::get_int, get_double, get_bool, get_list
166. feat(util): Config::merge (later wins)
167. feat(util): Config::save writes back to INI format
168. test(util): Config roundtrip parse+save+parse is idempotent
169. feat(util): ArgParser --config drives Config::from_file integration
170. refactor: migrate Date/DateRange/DateUtils into libcpp::util namespace
171. refactor: unify csv.hpp and CsvParser into libcpp::util::CsvParser
172. feat(util): CsvParser returns Result<Document> instead of throwing
173. test(util): CsvParser returns Err on missing file
174. feat(util): CsvParser type inference promoted to public API
175. docs: add util README with ArgParser + Config example
```

### Phase 7 — Memory & Coverage (176–195)
```
176. feat(mem): add LeakGuard::Mode::SCOPED_ONLY enum
177. feat(mem): LeakGuard::set_mode switches between global and scoped
178. feat(mem): LeakGuard::alloc<T>(args...) for scoped tracking
179. feat(mem): LeakGuard::free_ptr in scoped mode
180. test(mem): scoped mode does not intercept std::string internals
181. feat(mem): implement Pool<T, Capacity> fixed-size allocator
182. feat(mem): Pool::alloc constructs in-place in storage
183. feat(mem): Pool::free destructs and reclaims slot
184. feat(mem): Pool::reset destroys all and reclaims everything
185. test(mem): Pool alloc + free roundtrip 1000 times without leak
186. test(mem): Pool full() correctly blocks overallocation
187. feat(test): implement Coverage::hit line tracking
188. feat(test): Coverage::branch true/false counting
189. feat(test): Coverage::line_coverage_pct / branch_coverage_pct
190. feat(test): Coverage::print_report terminal output
191. feat(test): Coverage::save_lcov exports .lcov format
192. feat(test): COV_HIT / COV_BRANCH macros compile to no-op without flag
193. test(test): Coverage reports 0% on empty run
194. test(test): Coverage reports 100% when all branches hit
195. feat(test): TestSuite integrates coverage report after run_and_print
```

### Phase 8 — Integration & Polish (196–200)
```
196. refactor: regenerate libcpp.hpp master include for new layout
197. refactor: migrate postman.cpp to use libcpp::term::TableCore for rendering
198. feat: end-to-end demo: ArgParser → Config → Logger → TestSuite → Snapshot
199. docs: write comprehensive README with architecture diagram
200. chore: all tests green, Makefile and CMake both build cleanly
```

---

## 7. Design Principles to Follow Strictly

1. **Result everywhere**: any function that can fail returns `Result<T, E>`.
   No silent returns, no throw-or-ignore, no `bool ok = ...` out-params.

2. **Zero-cost abstractions**: macros (`LOG_*`, `PROFILE_*`, `COV_*`) compile
   to exactly nothing when the relevant feature flag is off.

3. **No global state except opt-in singletons**: `EventBus::global()`,
   `Profiler::global()`, `Coverage::global()` are opt-in. `TestSuite` is
   never global.

4. **Header-only where sensible, compiled where necessary**: template-heavy
   code (`Result`, `Signal`, `Property`, `Spy`, `Generator`, `Pool`) goes
   header-only. `Profiler`, `ThreadPool`, `EventBus`, `ArgParser`, `Config`
   are compiled.

5. **Consistent naming**: `snake_case` for all methods and variables.
   `PascalCase` for types. `SCREAMING_SNAKE` for macros only.

6. **No raw `new`/`delete` in new code**: use `std::unique_ptr`, `std::shared_ptr`,
   `Pool`, or `Arena`. The only `new`/`delete` overrides live in `LeakGuard.cpp`.

7. **C++17 throughout**: use `std::optional`, `std::variant`, `std::string_view`,
   structured bindings, `if constexpr`, fold expressions freely.

8. **One commit per logical unit**: each commit compiles, tests pass.
   Use conventional commits: `feat(module):`, `fix(module):`, `refactor(module):`,
   `test(module):`, `docs:`, `chore:`.

---

## 8. Working Instructions for Claude Opus 4.6

When implementing this rearchitecture, follow these rules:

1. **One commit at a time.** Show the full file contents for each modified or
   new file, then show the commit message. Wait for confirmation before the next.

2. **Always specify the full path** at the top of each code block:
   `// include/libcpp/core/result.hpp`

3. **Test alongside implementation**: every `feat:` commit is followed immediately
   by the corresponding `test:` commit. No feature ships without tests.

4. **Never break the build**: each commit must leave `make` and all existing tests
   green. If a refactor temporarily breaks something, fix it in the same commit.

5. **Preserve backward compatibility during migration**: old headers (`Srgb.hpp`,
   `TermStyle.hpp`, etc.) get thin forwarding wrappers until all callers are migrated.
   Only delete them in the final cleanup phase.

6. **EventBus is the integration spine**: whenever two new features interact
   (logger + test, profiler + benchmark, coverage + test suite), route through
   EventBus rather than direct coupling.

7. **When implementing `Snapshot::myers_diff`**: use the standard O(ND) algorithm.
   Output must use `+` (green) for additions and `-` (red) for removals,
   with 3 lines of context above and below each hunk.

8. **When implementing `Property::computed`**: changes to the source automatically
   propagate to the computed property via Signal. Computed properties are read-only.

9. **When implementing `Fuzzer::shrink`**: use binary search — halve the input,
   check if it still fails, keep the smaller failing input. For strings, reduce
   character by character. For integers, halve toward 0.

10. **The first thing to output** when you receive this prompt is:
    ```
    Commit 1/200
    chore: set up CMakeLists.txt with libcpp as INTERFACE/static target
    ```
    Then show the full `CMakeLists.txt` and the updated `Makefile`.