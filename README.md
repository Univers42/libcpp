# libcpp

A modular C++98 utility library built with Orthodox Canonical Form. Designed as a comprehensive toolkit for 42-school projects and beyond.

## Architecture

```
libcpp/
├── include/libcpp/          # public headers
│   ├── core/                # types, Result, Option, Signal, Property, Arena
│   ├── term/                # terminal colors, styles, table, tree, progress
│   ├── log/                 # leveled logging with macros
│   ├── test/                # test suite, snapshot, spy, fuzzer
│   ├── bench/               # stopwatch, benchmark, profiler
│   ├── mem/                 # memory pool, leak guard
│   ├── str/                 # UTF-8, formatting, case conversion
│   ├── util/                # CLI arg parser, INI config
│   ├── data/                # Date, CSV, in-memory Database
│   └── libcpp.hpp           # master include
├── src/                     # implementation files (mirrors include/)
├── tests/                   # integration test suite
├── demo/                    # end-to-end demo program
├── Makefile                 # 42-compatible build
└── CMakeLists.txt           # CMake build
```

## Quick Start

### Build

```bash
# Using Make (42 style)
make            # builds libcpp.a
make re         # clean rebuild

# Using CMake
mkdir build && cd build
cmake .. && make
```

### Run Tests

```bash
make
c++ -std=c++98 -Wall -Wextra -Werror -Iinclude tests/*.cpp -L. -lcpp -o test_runner
./test_runner
```

### Run Demo

```bash
c++ -std=c++98 -Wall -Wextra -Werror -Iinclude demo/main.cpp -L. -lcpp -o demo_runner
./demo_runner --name=Student --verbose
```

### Link in Your Project

```makefile
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -I/path/to/libcpp/include
LDFLAGS  = -L/path/to/libcpp -lcpp
```

## Modules

### core — Foundational types

| Component | Description |
|-----------|-------------|
| `types.hpp` | Fixed-width typedefs (`t_u8`, `t_i32`, ...) |
| `Result<T,E>` | Rust-inspired error handling with `ok()`, `err()`, `map()` |
| `Option<T>` | Nullable value wrapper with `some()`, `none()`, `unwrap_or()` |
| `Signal<A>` | Synchronous observer pattern (function + method slots) |
| `Property<T>` | Observable value that fires `on_change` signal |
| `Arena<T>` | Index-based arena allocator |

```cpp
#include "libcpp/core/result.hpp"

libcpp::Result<int, int> r = libcpp::Result<int, int>::ok(42);
if (r.is_ok())
    std::cout << r.unwrap() << std::endl;
```

### term — Terminal output

| Component | Description |
|-----------|-------------|
| `Srgb` | SRGB color with RGB/HSL conversion |
| `TermStyle` | Composable text styling (bold, italic, fg/bg colors) |
| `TableCore` | ASCII/Unicode table rendering |
| `TreePrinter` | Tree structure visualization |
| `ProgressBar` | Animated terminal progress bar |
| `TermWriter` | Buffered terminal output |

### log — Structured logging

| Component | Description |
|-----------|-------------|
| `Logger` | Leveled logger (TRACE → FATAL) with sinks |
| Macros | `LOG_DEBUG(logger, msg)`, `LOG_INFO(...)`, etc. |

### test — Testing framework

| Component | Description |
|-----------|-------------|
| `TestSuite` | Test runner with pass/fail/skip, hooks, timing |
| `Snapshot` | Golden-file snapshot comparison |
| `Spy` | Call-tracking test double |
| `Fuzzer` | Random input generation for int, string, etc. |

```cpp
#include "libcpp/test/suite.hpp"

void my_test(libcpp::test::TestSuite& s) {
    ASSERT_EQ(s, 1 + 1, 2);
    ASSERT_EQ_STR(s, libcpp::str::trim("  hi  "), "hi");
}

libcpp::test::TestSuite suite("example");
suite.test("arithmetic", &my_test);
suite.run();
```

### bench — Benchmarking

| Component | Description |
|-----------|-------------|
| `StopWatch` | High-resolution timer |
| `Benchmark` | Benchmark runner with warmup and statistics |
| `Profiler` | Scoped profiling with `PROFILE_SCOPE("name")` |

### mem — Memory management

| Component | Description |
|-----------|-------------|
| `Pool<T, N>` | Fixed-size object pool (stack-allocated) |
| `LeakGuard` | Global new/delete tracking and leak reporting |

### str — String utilities

| Component | Description |
|-----------|-------------|
| `utf8_len`, `utf8_valid` | UTF-8 string operations |
| `fmt()` | Positional format: `fmt("{0} has {1}", name, count)` |
| `trim`, `pad_left`, `pad_right` | Whitespace manipulation |
| `join`, `split` | Array-based join and split |
| `to_upper`, `to_lower`, `to_snake_case`, ... | Case conversion |

### util — Utilities

| Component | Description |
|-----------|-------------|
| `ArgParser` | CLI argument parsing with flags, options, positionals |
| `Config` | INI-style configuration file parser |

### data — Data structures

| Component | Description |
|-----------|-------------|
| `Date`, `DateRange` | Date arithmetic, parsing, ISO formatting |
| `CsvDocument` | CSV parser with aggregation functions |
| `Database` | In-memory row store with column schema and queries |

## Design Principles

- **C++98 compliant** — no C++11 features, builds with `-std=c++98`
- **Orthodox Canonical Form** — every class has default ctor, copy ctor, `operator=`, destructor
- **No external dependencies** — STL only
- **42 Makefile compatible** — `make`, `make clean`, `make fclean`, `make re`
- **Namespace isolation** — all code under `libcpp::` with module sub-namespaces
- **Static library** — outputs `libcpp.a`

## Compiler Flags

```
-std=c++98 -Wall -Wextra -Werror -Iinclude
```

## License

Educational project. Use freely.
