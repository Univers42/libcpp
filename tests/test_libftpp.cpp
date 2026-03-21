/* ************************************************************************** */
/*                                                                            */
/*   test_libftpp.cpp — integration tests for libftpp subject classes         */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/test/suite.hpp"
#include "libcpp/data/pool.hpp"
#include "libcpp/data/data_buffer.hpp"
#include "libcpp/core/memento.hpp"
#include "libcpp/core/observer.hpp"
#include "libcpp/core/singleton.hpp"
#include "libcpp/core/state_machine.hpp"
#include "libcpp/core/observable_value.hpp"
#include "libcpp/async/thread_safe_queue.hpp"
#include "libcpp/async/thread.hpp"
#include "libcpp/async/worker_pool.hpp"
#include "libcpp/async/persistent_worker.hpp"
#include "libcpp/net/message.hpp"
#include "libcpp/math/ivector2.hpp"
#include "libcpp/math/ivector3.hpp"
#include "libcpp/math/random_2d_coordinate_generator.hpp"
#include "libcpp/math/perlin_noise_2d.hpp"
#include "libcpp/util/chronometer.hpp"
#include "libcpp/util/timer.hpp"
#include <atomic>
#include <string>
#include <thread>
#include <chrono>

/* ── Pool ───────────────────────────────────────────────────────────────── */

static void test_pool_acquire_release(libcpp::test::TestSuite& s)
{
    libcpp::data::Pool<int> pool;
    pool.resize(4);
    ASSERT_EQ(s, pool.capacity(), static_cast<size_t>(4));
    ASSERT_EQ(s, pool.available(), static_cast<size_t>(4));
    {
        auto obj = pool.acquire(42);
        ASSERT_EQ(s, *obj, 42);
        ASSERT_EQ(s, pool.available(), static_cast<size_t>(3));
    }
    ASSERT_EQ(s, pool.available(), static_cast<size_t>(4));
}

static void test_pool_full(libcpp::test::TestSuite& s)
{
    libcpp::data::Pool<int> pool;
    pool.resize(1);
    auto obj = pool.acquire(1);
    ASSERT_TRUE(s, pool.full());
    bool threw = false;
    try { auto obj2 = pool.acquire(2); (void)obj2; }
    catch (const std::runtime_error&) { threw = true; }
    ASSERT_TRUE(s, threw);
}

/* ── DataBuffer ─────────────────────────────────────────────────────────── */

static void test_databuffer_int(libcpp::test::TestSuite& s)
{
    libcpp::data::DataBuffer buf;
    int a = 42;
    buf << a;
    buf.resetCursor();
    int b = 0;
    buf >> b;
    ASSERT_EQ(s, a, b);
}

static void test_databuffer_string(libcpp::test::TestSuite& s)
{
    libcpp::data::DataBuffer buf;
    std::string original = "hello world";
    buf << original;
    buf.resetCursor();
    std::string result;
    buf >> result;
    ASSERT_EQ_STR(s, original, result);
}

/* ── Memento ────────────────────────────────────────────────────────────── */

class TestMemento : public libcpp::core::Memento {
public:
    int value = 0;
protected:
    void _saveToSnapshot(Snapshot& snap) const override { snap << value; }
    void _loadFromSnapshot(Snapshot& snap) override { snap >> value; }
};

static void test_memento_undo_redo(libcpp::test::TestSuite& s)
{
    TestMemento m;
    m.value = 10;
    m.pushHistory();
    m.value = 20;
    m.pushHistory();
    m.undo();
    ASSERT_EQ(s, m.value, 10);
    m.redo();
    ASSERT_EQ(s, m.value, 20);
}

/* ── Observer ───────────────────────────────────────────────────────────── */

static void test_observer_notify(libcpp::test::TestSuite& s)
{
    libcpp::core::Observer<int> obs;
    int received = 0;
    obs.subscribe(42, [&]() { received = 42; });
    obs.notify(42);
    ASSERT_EQ(s, received, 42);
}

/* ── Singleton ──────────────────────────────────────────────────────────── */

struct SingletonTest { int x; SingletonTest(int v) : x(v) {} };

static void test_singleton_lifecycle(libcpp::test::TestSuite& s)
{
    using S = libcpp::core::Singleton<SingletonTest>;
    ASSERT_TRUE(s, !S::isInstantiated());
    S::instantiate(99);
    ASSERT_TRUE(s, S::isInstantiated());
    ASSERT_EQ(s, S::instance()->x, 99);
    S::destroy();
    ASSERT_TRUE(s, !S::isInstantiated());
}

/* ── StateMachine ───────────────────────────────────────────────────────── */

enum class GameState { Menu, Play, Pause };

static void test_statemachine_transitions(libcpp::test::TestSuite& s)
{
    libcpp::core::StateMachine<GameState> sm;
    sm.addState(GameState::Menu);
    sm.addState(GameState::Play);
    sm.addState(GameState::Pause);
    sm.addTransition(GameState::Menu, GameState::Play, [](){});
    sm.addTransition(GameState::Play, GameState::Pause, [](){});
    sm.addTransition(GameState::Pause, GameState::Play, [](){});

    sm.transitionTo(GameState::Menu);
    ASSERT_TRUE(s, sm.currentState() == GameState::Menu);
    sm.transitionTo(GameState::Play);
    ASSERT_TRUE(s, sm.currentState() == GameState::Play);
    ASSERT_TRUE(s, sm.canTransitionTo(GameState::Pause));
    ASSERT_TRUE(s, !sm.canTransitionTo(GameState::Menu));
}

/* ── ObservableValue ────────────────────────────────────────────────────── */

static void test_observable_value(libcpp::test::TestSuite& s)
{
    libcpp::core::ObservableValue<int> val(0);
    int lastSeen = -1;
    val.subscribe([&](const int& v) { lastSeen = v; });
    val = 42;
    ASSERT_EQ(s, lastSeen, 42);
    ASSERT_EQ(s, val.get(), 42);
}

/* ── ThreadSafeQueue ────────────────────────────────────────────────────── */

static void test_thread_safe_queue(libcpp::test::TestSuite& s)
{
    libcpp::async::ThreadSafeQueue<int> q;
    q.push_back(1);
    q.push_back(2);
    q.push_front(0);
    ASSERT_EQ(s, q.size(), static_cast<size_t>(3));
    ASSERT_EQ(s, q.pop_front(), 0);
    ASSERT_EQ(s, q.pop_back(), 2);
    auto opt = q.try_pop_front();
    ASSERT_TRUE(s, opt.has_value());
    ASSERT_EQ(s, *opt, 1);
    ASSERT_TRUE(s, q.empty());
}

/* ── Thread ─────────────────────────────────────────────────────────────── */

static void test_thread_start_stop(libcpp::test::TestSuite& s)
{
    std::atomic<bool> ran{false};
    libcpp::async::Thread t;
    t.setName("TestThread");
    t.setCallback([&]() { ran = true; });
    t.start();
    t.stop();
    ASSERT_TRUE(s, ran.load());
}

/* ── WorkerPool ─────────────────────────────────────────────────────────── */

static void test_worker_pool(libcpp::test::TestSuite& s)
{
    std::atomic<int> counter{0};
    {
        libcpp::async::WorkerPool pool(2);
        for (int i = 0; i < 10; ++i)
            pool.addJob([&]() { counter++; });
        // Give workers time
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    ASSERT_EQ(s, counter.load(), 10);
}

/* ── PersistentWorker ───────────────────────────────────────────────────── */

static void test_persistent_worker(libcpp::test::TestSuite& s)
{
    std::atomic<int> ticks{0};
    libcpp::async::PersistentWorker pw("TestPW");
    pw.addTask("tick", [&]() {
        ticks++;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    });
    pw.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    pw.stop();
    ASSERT_TRUE(s, ticks.load() > 0);
}

/* ── Message ────────────────────────────────────────────────────────────── */

static void test_message_serialize(libcpp::test::TestSuite& s)
{
    libcpp::net::Message msg(42);
    int payload = 123;
    msg.buffer() << payload;
    auto bytes = msg.serialize();
    auto msg2 = libcpp::net::Message::deserialize(bytes);
    ASSERT_EQ(s, msg2.type(), static_cast<libcpp::net::MessageType>(42));
    int out = 0;
    msg2.buffer() >> out;
    ASSERT_EQ(s, out, 123);
}

static void test_message_consumer(libcpp::test::TestSuite& s)
{
    libcpp::net::MessageConsumer consumer;
    int received = 0;
    consumer.registerHandler(1, [&](libcpp::net::Message& m) {
        m.buffer() >> received;
    });
    libcpp::net::Message msg(1);
    msg.buffer() << 99;
    msg.buffer().resetCursor();
    consumer.consume(msg);
    ASSERT_EQ(s, received, 99);
}

/* ── IVector2 ───────────────────────────────────────────────────────────── */

static void test_ivector2_ops(libcpp::test::TestSuite& s)
{
    libcpp::math::IVector2 a(3, 4);
    libcpp::math::IVector2 b(1, 2);
    auto c = a + b;
    ASSERT_EQ(s, c.x, 4);
    ASSERT_EQ(s, c.y, 6);
    ASSERT_EQ(s, a.dot(b), 11);
    ASSERT_EQ(s, a.cross(b), 2);
    ASSERT_EQ(s, a.lengthSquared(), 25);
}

/* ── IVector3 ───────────────────────────────────────────────────────────── */

static void test_ivector3_ops(libcpp::test::TestSuite& s)
{
    libcpp::math::IVector3 a(1, 0, 0);
    libcpp::math::IVector3 b(0, 1, 0);
    auto c = a.cross(b);
    ASSERT_EQ(s, c.x, 0);
    ASSERT_EQ(s, c.y, 0);
    ASSERT_EQ(s, c.z, 1);
    ASSERT_EQ(s, a.dot(b), 0);
}

/* ── Random2DCoordinateGenerator ────────────────────────────────────────── */

static void test_random2d(libcpp::test::TestSuite& s)
{
    libcpp::math::IVector2 min(0, 0);
    libcpp::math::IVector2 max(1, 1);
    libcpp::math::Random2DCoordinateGenerator gen(42, min, max);
    ASSERT_EQ(s, gen.remaining(), static_cast<size_t>(4)); // 2x2 grid
    auto p1 = gen();
    ASSERT_EQ(s, gen.remaining(), static_cast<size_t>(3));
    ASSERT_TRUE(s, p1.x >= 0 && p1.x <= 1);
    ASSERT_TRUE(s, p1.y >= 0 && p1.y <= 1);
}

/* ── PerlinNoise2D ──────────────────────────────────────────────────────── */

static void test_perlin_noise(libcpp::test::TestSuite& s)
{
    libcpp::math::PerlinNoise2D noise(42);
    double v = noise.sample(0.5, 0.5);
    ASSERT_TRUE(s, v >= -1.0 && v <= 1.0);
    // Same seed + same coords = deterministic
    double v2 = noise.sample(0.5, 0.5);
    ASSERT_TRUE(s, v == v2);
}

/* ── Chronometer ────────────────────────────────────────────────────────── */

static void test_chronometer(libcpp::test::TestSuite& s)
{
    libcpp::util::Chronometer chrono;
    chrono.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    chrono.stop();
    ASSERT_TRUE(s, chrono.elapsedMilliseconds() >= 5.0);
    ASSERT_TRUE(s, !chrono.isRunning());
}

/* ── Edge cases ─────────────────────────────────────────────────────────── */

static void test_databuffer_multi(libcpp::test::TestSuite& s)
{
    libcpp::data::DataBuffer buf;
    buf << 1 << 2 << 3 << std::string("abc");
    buf.resetCursor();
    int a, b, c; std::string str;
    buf >> a >> b >> c >> str;
    ASSERT_EQ(s, a, 1);
    ASSERT_EQ(s, b, 2);
    ASSERT_EQ(s, c, 3);
    ASSERT_EQ_STR(s, str, "abc");
}

static void test_queue_throw_empty(libcpp::test::TestSuite& s)
{
    libcpp::async::ThreadSafeQueue<int> q;
    bool threw = false;
    try { q.pop_front(); }
    catch (const std::runtime_error&) { threw = true; }
    ASSERT_TRUE(s, threw);
}

static void test_memento_double_undo(libcpp::test::TestSuite& s)
{
    TestMemento m;
    m.value = 10; m.pushHistory();
    m.value = 20; m.pushHistory();
    m.value = 30; m.pushHistory();
    ASSERT_TRUE(s, m.undo());  // -> 20
    ASSERT_EQ(s, m.value, 20);
    ASSERT_TRUE(s, m.undo());  // -> 10
    ASSERT_EQ(s, m.value, 10);
    ASSERT_TRUE(s, !m.undo()); // can't go further
}

static void test_ivector2_division(libcpp::test::TestSuite& s)
{
    libcpp::math::IVector2 v(10, 20);
    auto r = v / 2;
    ASSERT_EQ(s, r.x, 5);
    ASSERT_EQ(s, r.y, 10);
    bool threw = false;
    try { auto _ = v / 0; (void)_; }
    catch (const std::runtime_error&) { threw = true; }
    ASSERT_TRUE(s, threw);
}

static void test_perlin_octave(libcpp::test::TestSuite& s)
{
    libcpp::math::PerlinNoise2D noise(123);
    double v = noise.octave(1.5, 2.5, 4, 0.5);
    ASSERT_TRUE(s, v >= -1.0 && v <= 1.0);
}

static void test_random2d_exhaust(libcpp::test::TestSuite& s)
{
    libcpp::math::IVector2 min(0, 0);
    libcpp::math::IVector2 max(0, 1);
    libcpp::math::Random2DCoordinateGenerator gen(1, min, max);
    ASSERT_EQ(s, gen.remaining(), static_cast<size_t>(2));
    gen(); gen();  // exhaust
    ASSERT_EQ(s, gen.remaining(), static_cast<size_t>(0));
    bool threw = false;
    try { gen(); }
    catch (const std::runtime_error&) { threw = true; }
    ASSERT_TRUE(s, threw);
}

/* ── Pool move semantics ────────────────────────────────────────────────── */

static void test_pool_object_move(libcpp::test::TestSuite& s)
{
    libcpp::data::Pool<int> pool;
    pool.resize(2);
    auto a = pool.acquire(42);
    ASSERT_EQ(s, pool.available(), static_cast<size_t>(1));
    auto b = std::move(a);                // move-construct
    ASSERT_EQ(s, *b, 42);
    ASSERT_EQ(s, pool.available(), static_cast<size_t>(1)); // still 1 slot used
    {
        auto c = pool.acquire(99);
        b = std::move(c);                 // move-assign (releases old slot)
    }
    ASSERT_EQ(s, *b, 99);
    ASSERT_EQ(s, pool.available(), static_cast<size_t>(1)); // one released via move-assign
}

/* ── Observer unsubscribe ───────────────────────────────────────────────── */

static void test_observer_unsubscribe(libcpp::test::TestSuite& s)
{
    libcpp::core::Observer<int> obs;
    int count = 0;
    auto id = obs.subscribe(1, [&]() { count++; });
    obs.notify(1);
    ASSERT_EQ(s, count, 1);
    obs.unsubscribe(1, id);
    obs.notify(1);
    ASSERT_EQ(s, count, 1);  // should not fire again
    ASSERT_EQ(s, obs.listenerCount(1), static_cast<size_t>(0));
}

/* ── Singleton double-instantiate ───────────────────────────────────────── */

static void test_singleton_double(libcpp::test::TestSuite& s)
{
    using S = libcpp::core::Singleton<SingletonTest>;
    if (S::isInstantiated()) S::destroy();
    S::instantiate(1);
    bool threw = false;
    try { S::instantiate(2); }
    catch (const std::runtime_error&) { threw = true; }
    ASSERT_TRUE(s, threw);
    S::destroy();
}

/* ── StateMachine invalid transition ────────────────────────────────────── */

static void test_statemachine_invalid(libcpp::test::TestSuite& s)
{
    libcpp::core::StateMachine<GameState> sm;
    sm.addState(GameState::Menu);
    sm.addState(GameState::Play);
    sm.addTransition(GameState::Menu, GameState::Play);
    sm.transitionTo(GameState::Menu);
    // No transition from Menu → Pause
    sm.addState(GameState::Pause);
    bool threw = false;
    try { sm.transitionTo(GameState::Pause); }
    catch (const std::runtime_error&) { threw = true; }
    ASSERT_TRUE(s, threw);
}

/* ── Timer setTimeout ───────────────────────────────────────────────────── */

static void test_timer_timeout(libcpp::test::TestSuite& s)
{
    std::atomic<bool> fired{false};
    libcpp::util::Timer timer;
    timer.setTimeout([&]() { fired = true; },
                     std::chrono::milliseconds(10));
    ASSERT_TRUE(s, timer.isRunning());
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_TRUE(s, fired.load());
    ASSERT_TRUE(s, !timer.isRunning());
}

/* ── Timer setInterval ──────────────────────────────────────────────────── */

static void test_timer_interval(libcpp::test::TestSuite& s)
{
    std::atomic<int> ticks{0};
    libcpp::util::Timer timer;
    timer.setInterval([&]() { ticks++; },
                      std::chrono::milliseconds(10));
    std::this_thread::sleep_for(std::chrono::milliseconds(55));
    timer.stop();
    ASSERT_TRUE(s, ticks.load() >= 3);
    ASSERT_TRUE(s, !timer.isRunning());
}

/* ── DataBuffer setRaw ──────────────────────────────────────────────────── */

static void test_databuffer_setraw(libcpp::test::TestSuite& s)
{
    libcpp::data::DataBuffer src;
    src << 42 << std::string("raw");
    libcpp::data::DataBuffer dst;
    dst.setRaw(src.raw().data(), src.size());
    int val = 0; std::string str;
    dst >> val >> str;
    ASSERT_EQ(s, val, 42);
    ASSERT_EQ_STR(s, str, "raw");
}

/* ── Suite runner ───────────────────────────────────────────────────────── */

void run_libftpp_tests(void)
{
    libcpp::test::TestSuite s("libftpp");

    // Data Structures
    s.test("Pool::acquire_release", test_pool_acquire_release);
    s.test("Pool::full", test_pool_full);
    s.test("DataBuffer::int", test_databuffer_int);
    s.test("DataBuffer::string", test_databuffer_string);

    // Design Patterns
    s.test("Memento::undo_redo", test_memento_undo_redo);
    s.test("Observer::notify", test_observer_notify);
    s.test("Singleton::lifecycle", test_singleton_lifecycle);
    s.test("StateMachine::transitions", test_statemachine_transitions);
    s.test("ObservableValue::subscribe", test_observable_value);

    // Threading
    s.test("ThreadSafeQueue::ops", test_thread_safe_queue);
    s.test("Thread::start_stop", test_thread_start_stop);
    s.test("WorkerPool::jobs", test_worker_pool);
    s.test("PersistentWorker::run", test_persistent_worker);

    // Network
    s.test("Message::serialize", test_message_serialize);
    s.test("MessageConsumer::dispatch", test_message_consumer);

    // Mathematics
    s.test("IVector2::ops", test_ivector2_ops);
    s.test("IVector3::ops", test_ivector3_ops);
    s.test("Random2D::generate", test_random2d);
    s.test("PerlinNoise2D::sample", test_perlin_noise);

    // Bonus
    s.test("Chronometer::timing", test_chronometer);

    // Edge cases
    s.test("DataBuffer::multi", test_databuffer_multi);
    s.test("Queue::throw_empty", test_queue_throw_empty);
    s.test("Memento::double_undo", test_memento_double_undo);
    s.test("IVector2::div_zero", test_ivector2_division);
    s.test("PerlinNoise2D::octave", test_perlin_octave);
    s.test("Random2D::exhaust", test_random2d_exhaust);

    // Extended coverage
    s.test("Pool::Object_move", test_pool_object_move);
    s.test("Observer::unsubscribe", test_observer_unsubscribe);
    s.test("Singleton::double_instantiate", test_singleton_double);
    s.test("StateMachine::invalid_transition", test_statemachine_invalid);
    s.test("Timer::setTimeout", test_timer_timeout);
    s.test("Timer::setInterval", test_timer_interval);
    s.test("DataBuffer::setRaw", test_databuffer_setraw);

    s.run();
}
