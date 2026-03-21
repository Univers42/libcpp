# libcpp / libftpp

A modular C++17 utility library implementing the **42 libftpp subject** and more. Builds a static library (`libcpp.a` / `libftpp.a`) with zero external dependencies.

## Architecture

```
libcpp/
├── include/libcpp/          # public headers
│   ├── core/                # Result, Option, Signal, Property, Arena
│   │                        # + Memento, Observer, Singleton, StateMachine
│   │                        #   ObservableValue (design patterns)
│   ├── term/                # colors, styles, table, tree, progress, writer
│   │                        # + ThreadSafeIOStream
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
├── tests/                   # 91 integration tests
├── demo/                    # demo programs
├── libftpp.hpp              # umbrella header (required by subject)
└── Makefile                 # 42-compatible build
```

## Quick Start

```bash
make                 # builds libcpp.a + libftpp.a
make test            # runs 91 integration tests
make demo            # builds and runs libftpp demo
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

## Legacy Modules (from libcpp)

| Module | Key classes |
|--------|-------------|
| **core** | `Result<T,E>`, `Option<T>`, `Signal<A>`, `Property<T>`, `Arena<T>` |
| **term** | `Srgb`, `TermStyle`, `TableCore`, `TreePrinter`, `ProgressBar`, `TermWriter` |
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

### v2.0.0 — libftpp integration (C++17)
- Added 12 new libftpp classes: Pool, DataBuffer, Memento, Observer, Singleton, StateMachine, ObservableValue, ThreadSafeIOStream, ThreadSafeQueue, Thread, WorkerPool, PersistentWorker
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
