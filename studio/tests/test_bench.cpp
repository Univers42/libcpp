/* ************************************************************************** */
/*                                                                            */
/*   test_bench.cpp — integration tests for bench module                      */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/test/suite.hpp"
#include "libcpp/bench/timer.hpp"
#include "libcpp/bench/benchmark.hpp"
#include "libcpp/bench/profiler.hpp"

static void test_stopwatch(libcpp::test::TestSuite& s)
{
	libcpp::bench::StopWatch sw;
	sw.start();
	/* minimal work */
	volatile int x = 0;
	for (int i = 0; i < 100; ++i) x += i;
	(void)x;
	sw.stop();
	ASSERT_TRUE(s, sw.elapsed_ms() >= 0.0);
}

static void bench_fn(void)
{
	volatile int sum = 0;
	for (int i = 0; i < 1000; ++i) sum += i;
	(void)sum;
}

static void test_benchmark_run(libcpp::test::TestSuite& s)
{
	libcpp::bench::Benchmark bm("test_bench");
	bm.add("noop", &bench_fn);
	bm.iterations(10).warmup(2);
	bm.run();
	ASSERT_EQ(s, bm.result_count(), 1);
	ASSERT_TRUE(s, bm.result(0).mean_ms >= 0.0);
	ASSERT_EQ(s, bm.result(0).iterations, 10);
}

static void test_profiler_scope(libcpp::test::TestSuite& s)
{
	libcpp::bench::Profiler::instance().reset();
	{
		PROFILE_SCOPE("test_section");
		volatile int x = 0;
		for (int i = 0; i < 100; ++i) x += i;
		(void)x;
	}
	ASSERT_EQ(s, libcpp::bench::Profiler::instance().entry_count(), 1);
}

void run_bench_tests(void)
{
	libcpp::test::TestSuite s("bench");
	s.test("StopWatch",       &test_stopwatch);
	s.test("Benchmark::run",  &test_benchmark_run);
	s.test("Profiler::scope", &test_profiler_scope);
	s.run();
}
