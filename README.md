# libcpp / libftpp

A modular C++17 utility library implementing the **42 libftpp subject** and more.
Builds a static library (`libcpp.a` / `libftpp.a`) with zero external dependencies.

## Architecture

```
libc/
├── include/libcpp/          # public headers
│   ├── core/                # Result, Option, Signal, Property, Arena
│   │                        # + Memento, Observer, Singleton, StateMachine
│   │                        #   ObservableValue (design patterns)
│   ├── term/                # colors, styles, table, tree, progress, writer
│   │                        # + StyleSheet, ThreadSafeIOStream
│   ├── log/                 # leveled logging with macros
│   ├── test/                # test suite, snapshot, spy, fuzzer
│   ├── bench/               # stopwatch, benchmark, profiler
│   ├── mem/                 # memory pool, leak guard
│   ├── str/                 # UTF-8, formatting, case conversion
│   ├── util/                # CLI arg parser, INI config, Timer, Chronometer
│   ├── data/                # Date, CSV, Database, Pool<TType>, DataBuffer
│   ├── async/               # ThreadSafeQueue, Thread, WorkerPool, PersistentWorker
│   ├── net/                 # Message, Client, Server (TCP/POSIX)
│   ├── math/                # IVector2, IVector3, Random2D, PerlinNoise2D
│   └── libcpp.hpp           # master include
├── src/                     # implementation files (mirrors include/)
├── tests/                   # integration tests
├── studio/                  # demos + test runner
│   ├── demo/                # themed demo programs
│   └── tests/               # studio test suite
├── libftpp.hpp              # umbrella header (required by subject)
└── Makefile                 # 42-compatible build
```

## Quick Start

```bash
make                 # builds libcpp.a + libftpp.a
make test            # runs integration tests
make demo            # builds and runs libftpp demo
make compile_studio  # builds all studio demos + test runner
make run_demos       # builds + runs all demos (prints OK/FAIL per binary)
make run_tests       # builds + executes the studio test suite
make stats           # prints codebase metrics
make re              # clean rebuild
```

### Link in Your Project

```makefile
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -Iinclude -pthread
LDFLAGS  = -L/path/to/libcpp -lcpp
```

Or use the subject-required name:

```cpp
#include "libftpp.hpp"
// -I. -Iinclude -lftpp
```

## libftpp Subject Classes

### Data Structures

| Class | Header | Description |
|-------|--------|-------------|
| `Pool<TType>` | `data/pool.hpp` | Resource pool with RAII `Object` handles |
| `DataBuffer` | `data/data_buffer.hpp` | Polymorphic byte container with `<<`/`>>` |

```cpp
libcpp::data::Pool<int> pool;
pool.resize(10);
auto obj = pool.acquire(42);  // RAII handle — returns slot on destruction
```

### Design Patterns

| Class | Header | Description |
|-------|--------|-------------|
| `Memento` | `core/memento.hpp` | Undo/redo via DataBuffer snapshots |
| `Observer<TEvent>` | `core/observer.hpp` | Event subscription + dispatch |
| `Singleton<TType>` | `core/singleton.hpp` | Unique instance with `instantiate`/`destroy` |
| `StateMachine<TState>` | `core/state_machine.hpp` | States, transitions, actions |
| `ObservableValue<TType>` | `core/observable_value.hpp` | Value wrapper that notifies on change |

```cpp
libcpp::core::StateMachine<GameState> sm;
sm.addState(GameState::Menu);
sm.addState(GameState::Play);
sm.addTransition(GameState::Menu, GameState::Play, []{ std::cout << "Starting!\n"; });
sm.transitionTo(GameState::Menu);
sm.transitionTo(GameState::Play);  // prints "Starting!"
```

### IOStream

| Class | Header | Description |
|-------|--------|-------------|
| `ThreadSafeIOStream` | `term/thread_safe_iostream.hpp` | Thread-local buffered output with atomic flush |

```cpp
libcpp::threadSafeCout.setPrefix("Main");
libcpp::threadSafeCout << "Hello" << std::endl;  // "[Main] Hello"
```

### Threading

| Class | Header | Description |
|-------|--------|-------------|
| `ThreadSafeQueue<T>` | `async/thread_safe_queue.hpp` | Deque + mutex + condition variable |
| `Thread` | `async/thread.hpp` | Named thread wrapper with prefix |
| `WorkerPool` | `async/worker_pool.hpp` | Configurable thread pool |
| `PersistentWorker` | `async/persistent_worker.hpp` | Named task cycle loop |

```cpp
libcpp::async::WorkerPool pool(4);
pool.addJob([]{ /* work */ });
```

### Network

| Class | Header | Description |
|-------|--------|-------------|
| `Message` | `net/message.hpp` | Type ID + DataBuffer payload with serialize/deserialize |
| `MessageConsumer` | `net/message.hpp` | Type-to-callback dispatcher |
| `Client` | `net/client.hpp` | TCP client (POSIX sockets) |
| `Server` | `net/server.hpp` | TCP server with per-client threads |

```cpp
libcpp::net::Message msg(MSG_CHAT);
msg.buffer() << std::string("hello");
auto bytes = msg.serialize();
```

### Mathematics

| Class | Header | Description |
|-------|--------|-------------|
| `IVector2` | `math/ivector2.hpp` | 2D integer vector with arithmetic |
| `IVector3` | `math/ivector3.hpp` | 3D integer vector with cross product |
| `Random2DCoordinateGenerator` | `math/random_2d_coordinate_generator.hpp` | Seeded unique 2D coordinates |
| `PerlinNoise2D` | `math/perlin_noise_2d.hpp` | Classic Perlin noise with octaves |

```cpp
libcpp::math::PerlinNoise2D noise(42);
double v = noise.octave(x * 4.0, y * 4.0, 4, 0.5);
```

### Bonus

| Class | Header | Description |
|-------|--------|-------------|
| `Timer` | `util/timer.hpp` | setTimeout / setInterval callbacks |
| `Chronometer` | `util/chronometer.hpp` | High-resolution pause/resume timer |

## StyleSheet Theming System

`libcpp` ships a CSS-like theming layer for `TermStyle` and `TermWriter`, living in
`include/libcpp/term/stylesheet.hpp` and `src/term/stylesheet.cpp`.

### Quick usage

```cpp
#include "libcpp/libcpp.hpp"

libcpp::term::TermStyle  ts;
libcpp::term::TermWriter w;
libcpp::term::StyleSheet ss;

ss.preset("dracula");   // or "nord" / "monokai" / "solarized"
ss.apply(ts, w);        // applies rules to ts, registers callouts in w

w.render(ts, R"(
# Heading
## Sub-heading
> [!NOTE] A styled callout
- bullet one
1. ordered item
`inline code` and **bold** and __underline__ and ~~strike~~
)");
```

### Rule macros (header-only themes)

```cpp
#include "libcpp/term/stylesheet.hpp"

LIBCPP_THEME_BEGIN(my_theme, ss)            // opens an inline theme block

  // plain element rules
  RULE(h1)    .glyph("◆ ").fg(0xBD93F9);
  RULE(h2)    .glyph("◇ ").fg(0xFF79C6);
  RULE(bullet).glyph("▸ ").fg(0x8BE9FD);

  // ordered-list: body_glyph = prefix before number, glyph = suffix after
  RULE(ol)    .body_glyph("").glyph(")");   // → "1) item"

  // callout boxes
  CALLOUT_RULE(note)    .fg(0x8BE9FD).glyph("ℹ ").label("NOTE");
  CALLOUT_RULE(warning) .fg(0xFFB86C).glyph("⚠ ").label("WARNING");
  CALLOUT_RULE(tip)     .fg(0x50FA7B).glyph("✔ ").label("TIP");
  CALLOUT_RULE(error)   .fg(0xFF5555).glyph("✖ ").label("ERROR");

  // table overrides
  TABLE_RULE(my_table)  .border(libcpp::term::BorderPreset::ROUNDED)
                        .zebra(0x282A36).zebra_alt(0x21222C);

LIBCPP_THEME_END

my_theme(ss);   // apply the theme to a StyleSheet instance
```

### Built-in presets

| Preset | Palette | OL format | Bullet | Notes |
|--------|---------|-----------|--------|-------|
| `dracula` | Dracula purple/pink/cyan | `N)` | `◆` purple | italic h3 |
| `nord` | Nord frost/aurora | `N.` | `▸` frost | heavy h1 border |
| `monokai` | Monokai neon orange/green | `N-` | `★` orange | arrow h2 `➜` |
| `solarized` | Solarized warm base | `(N)` | `○` teal | muted/italic h3 |

All built-in presets define callout rules for: `note`, `tip`, `warning`, `error`,
`info`, `success`, `danger`, `abstract`.

### Themed demos

```bash
make compile_studio                          # build all demos
make run_demos                               # run all demos (OK/FAIL per binary)
make run_tests                               # run studio test suite

build/bin/studio/demo/demo_dracula
build/bin/studio/demo/demo_nord
build/bin/studio/demo/demo_monokai
build/bin/studio/demo/demo_solarized
build/bin/studio/demo/demo_customization
build/bin/studio/demo/demo_tables
build/bin/studio/demo/stylesheet_demo
```

## Legacy Modules (from libcpp)

| Module | Key classes |
|--------|-------------|
| **core** | `Result<T,E>`, `Option<T>`, `Signal<A>`, `Property<T>`, `Arena<T>` |
| **term** | `Srgb`, `TermStyle`, `TableCore`, `TreePrinter`, `ProgressBar`, `TermWriter`, `StyleSheet` |
| **log** | `Logger` with `LOG_DEBUG`, `LOG_INFO`, `LOG_WARN`, `LOG_ERROR`, `LOG_FATAL` |
| **test** | `TestSuite`, `Snapshot`, `Spy`, `Fuzzer` |
| **bench** | `ScopeTimer`, `StopWatch`, `Benchmark`, `Profiler` |
| **mem** | `Pool<T, N>` (stack-allocated), `LeakGuard` |
| **str** | `utf8_len`, `fmt()`, `trim`, `join`, `split`, case conversion |
| **util** | `ArgParser`, `Config` |
| **data** | `Date`, `DateRange`, `CsvDocument`, `Database` |

## Section Headers (libftpp subject)

The subject requires these umbrella headers:

```cpp
#include "libcpp/data/data_structures.hpp"    // Pool, DataBuffer, CSV, Database, Date
#include "libcpp/core/design_patterns.hpp"    // Memento, Observer, Singleton, StateMachine, ...
#include "libcpp/async/threading.hpp"         // ThreadSafeQueue, Thread, WorkerPool, PersistentWorker
#include "libcpp/net/network.hpp"             // Message, Client, Server
#include "libcpp/math/mathematics.hpp"        // IVector2, IVector3, Random2D, PerlinNoise2D
```

## Design Principles

- **C++17** — `-std=c++17 -Wall -Wextra -Werror -pthread`
- **No external dependencies** — STL + POSIX sockets only
- **No printf, alloc, free** — pure C++ idioms
- **Thread-safe** — mutexes, atomics, condition variables where needed
- **RAII everywhere** — Pool::Object, Thread, WorkerPool, Timer, etc.
- **Namespace isolation** — `libcpp::` with module sub-namespaces
- **Static library** — `libcpp.a` + `libftpp.a` (alias)
- **42 Makefile** — `make`, `make clean`, `make fclean`, `make re`, `make test`
- **`[[nodiscard]]`** — annotated on all query/acquire methods

## Changelog

### v2.1.0 — StyleSheet theming system
- **`StyleSheet`** CSS-like fluent configuration for `TermStyle` + `TermWriter`
- **`RULE` / `CALLOUT_RULE` / `TABLE_RULE` macros** for header-only theme authoring
- **4 built-in themes**: `dracula`, `nord`, `monokai`, `solarized`
- Customisable headings, bullets, ordered-list format, inline styles and callout boxes per theme
- `make run_demos` — batch-execute all studio demos; `make run_tests` — run test suite
- `make compile_studio` — builds all demos + test runner

### v2.0.0 — libftpp integration (C++17)
- Added 12 new libftpp classes: Pool, DataBuffer, Memento, Observer, Singleton, StateMachine,
  ObservableValue, ThreadSafeIOStream, ThreadSafeQueue, Thread, WorkerPool, PersistentWorker
- Added TCP networking: Message, Client, Server
- Added math: IVector2, IVector3, Random2DCoordinateGenerator, PerlinNoise2D
- Added utils: Timer, Chronometer
- 91 integration tests (36 libftpp-specific)
- Full `[[nodiscard]]` annotations on query methods
- Doxygen `@brief` comments on all class headers
- `make stats`, `make demo` targets

### v1.0.0 — Initial C++98 library
- 55 tests, 12 modules, Orthodox Canonical Form

## License

Educational project. Use freely.
