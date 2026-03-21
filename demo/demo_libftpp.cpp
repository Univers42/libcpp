/* ************************************************************************** */
/*  demo_libftpp.cpp — demonstrates all libftpp subject classes               */
/* ************************************************************************** */

#include "libftpp.hpp"
#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

// ── Memento demo helper ─────────────────────────────────────────────────
class Editor : public libcpp::core::Memento {
public:
    std::string text;
protected:
    void _saveToSnapshot(Snapshot& s) const override { s << text; }
    void _loadFromSnapshot(Snapshot& s) override { s >> text; }
};

int main()
{
    std::cout << "=== libftpp demo ===\n\n";

    // ── 1. Pool ─────────────────────────────────────────────────────────
    std::cout << "--- Pool<int> ---\n";
    libcpp::data::Pool<int> pool;
    pool.resize(3);
    {
        auto a = pool.acquire(10);
        auto b = pool.acquire(20);
        std::cout << "  acquired: " << *a << ", " << *b
                  << "  (avail=" << pool.available() << ")\n";
    }
    std::cout << "  after release: avail=" << pool.available() << "\n\n";

    // ── 2. DataBuffer ───────────────────────────────────────────────────
    std::cout << "--- DataBuffer ---\n";
    libcpp::data::DataBuffer buf;
    buf << 42 << std::string("hello");
    buf.resetCursor();
    int n; std::string s;
    buf >> n >> s;
    std::cout << "  int=" << n << "  str=\"" << s << "\"\n\n";

    // ── 3. Memento ──────────────────────────────────────────────────────
    std::cout << "--- Memento (undo/redo) ---\n";
    Editor ed;
    ed.text = "first";  ed.pushHistory();
    ed.text = "second"; ed.pushHistory();
    std::cout << "  current: " << ed.text << "\n";
    ed.undo();
    std::cout << "  undo:    " << ed.text << "\n";
    ed.redo();
    std::cout << "  redo:    " << ed.text << "\n\n";

    // ── 4. Observer ─────────────────────────────────────────────────────
    std::cout << "--- Observer ---\n";
    libcpp::core::Observer<std::string> obs;
    obs.subscribe("click", [](){ std::cout << "  click handler fired!\n"; });
    obs.notify("click");
    std::cout << "\n";

    // ── 5. Singleton ────────────────────────────────────────────────────
    std::cout << "--- Singleton ---\n";
    struct Config { int port; };
    using Cfg = libcpp::core::Singleton<Config>;
    Cfg::instantiate(Config{8080});
    std::cout << "  port=" << Cfg::instance()->port << "\n";
    Cfg::destroy();
    std::cout << "\n";

    // ── 6. StateMachine ─────────────────────────────────────────────────
    std::cout << "--- StateMachine ---\n";
    enum class St { Idle, Run, Done };
    libcpp::core::StateMachine<St> sm;
    sm.addState(St::Idle);
    sm.addState(St::Run);
    sm.addState(St::Done);
    sm.addTransition(St::Idle, St::Run, [](){ std::cout << "  Idle->Run\n"; });
    sm.addTransition(St::Run, St::Done, [](){ std::cout << "  Run->Done\n"; });
    sm.transitionTo(St::Idle);
    sm.transitionTo(St::Run);
    sm.transitionTo(St::Done);
    std::cout << "\n";

    // ── 7. ObservableValue ──────────────────────────────────────────────
    std::cout << "--- ObservableValue ---\n";
    libcpp::core::ObservableValue<int> score(0);
    score.subscribe([](const int& v){
        std::cout << "  score changed to " << v << "\n";
    });
    score = 100;
    score = 200;
    std::cout << "\n";

    // ── 8. ThreadSafeQueue ──────────────────────────────────────────────
    std::cout << "--- ThreadSafeQueue ---\n";
    libcpp::async::ThreadSafeQueue<int> q;
    q.push_back(1); q.push_back(2); q.push_back(3);
    std::cout << "  pop: " << q.pop_front() << ", " << q.pop_front()
              << ", " << q.pop_front() << "\n\n";

    // ── 9. Thread + WorkerPool ──────────────────────────────────────────
    std::cout << "--- WorkerPool (2 workers, 4 jobs) ---\n";
    {
        std::atomic<int> done{0};
        libcpp::async::WorkerPool wp(2);
        for (int i = 0; i < 4; ++i)
            wp.addJob([&done, i](){
                std::cout << "  job " << i << " done\n";
                done++;
            });
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::cout << "\n";

    // ── 10. Message ─────────────────────────────────────────────────────
    std::cout << "--- Message serialize/deserialize ---\n";
    libcpp::net::Message msg(1);
    msg.buffer() << std::string("payload data");
    auto bytes = msg.serialize();
    auto msg2 = libcpp::net::Message::deserialize(bytes);
    std::string payload;
    msg2.buffer() >> payload;
    std::cout << "  type=" << msg2.type() << "  payload=\"" << payload << "\"\n\n";

    // ── 11. IVector2 / IVector3 ─────────────────────────────────────────
    std::cout << "--- Vectors ---\n";
    libcpp::math::IVector2 v2(3, 4);
    std::cout << "  IVector2" << v2 << "  len=" << v2.length() << "\n";
    libcpp::math::IVector3 v3(1, 0, 0);
    libcpp::math::IVector3 v3b(0, 1, 0);
    std::cout << "  cross=" << v3.cross(v3b) << "\n\n";

    // ── 12. PerlinNoise2D ───────────────────────────────────────────────
    std::cout << "--- PerlinNoise2D ---\n";
    libcpp::math::PerlinNoise2D noise(42);
    for (double y = 0; y < 1.0; y += 0.25) {
        std::cout << "  ";
        for (double x = 0; x < 1.0; x += 0.25) {
            double v = noise.sample(x * 4.0, y * 4.0);
            std::cout << (v > 0 ? '#' : '.');
        }
        std::cout << "\n";
    }
    std::cout << "\n";

    // ── 13. Chronometer ─────────────────────────────────────────────────
    std::cout << "--- Chronometer ---\n";
    libcpp::util::Chronometer chrono;
    chrono.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    chrono.stop();
    std::cout << "  elapsed: " << chrono.elapsedMilliseconds() << " ms\n\n";

    std::cout << "=== demo complete ===\n";
    return 0;
}
