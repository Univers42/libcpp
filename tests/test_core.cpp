/* ************************************************************************** */
/*                                                                            */
/*   test_core.cpp — integration tests for core module                        */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/test/suite.hpp"
#include "libcpp/core/types.hpp"
#include "libcpp/core/result.hpp"
#include "libcpp/core/option.hpp"
#include "libcpp/core/signal.hpp"
#include "libcpp/core/property.hpp"
#include "libcpp/core/arena.hpp"

/* ── Result tests (use POD error type to stay C++98 union-safe) ─────────── */

static void test_result_ok(libcpp::test::TestSuite& s)
{
	libcpp::Result<int, int> r = libcpp::Result<int, int>::ok(42);
	ASSERT_TRUE(s, r.is_ok());
	ASSERT_FALSE(s, r.is_err());
	ASSERT_EQ(s, r.unwrap(), 42);
}

static void test_result_err(libcpp::test::TestSuite& s)
{
	libcpp::Result<int, int> r = libcpp::Result<int, int>::err(-1);
	ASSERT_TRUE(s, r.is_err());
	ASSERT_EQ(s, r.error(), -1);
}

static void test_result_unwrap_or(libcpp::test::TestSuite& s)
{
	libcpp::Result<int, int> r = libcpp::Result<int, int>::err(-1);
	ASSERT_EQ(s, r.unwrap_or(99), 99);
}

static int double_it(const int& v) { return v * 2; }

static void test_result_map(libcpp::test::TestSuite& s)
{
	libcpp::Result<int, int> r = libcpp::Result<int, int>::ok(5);
	libcpp::Result<int, int> mapped = r.map(&double_it);
	ASSERT_EQ(s, mapped.unwrap(), 10);
}

/* ── Option tests ───────────────────────────────────────────────────────── */

static void test_option_some(libcpp::test::TestSuite& s)
{
	libcpp::Option<int> o = libcpp::Option<int>::some(7);
	ASSERT_TRUE(s, o.is_some());
	ASSERT_EQ(s, o.unwrap(), 7);
}

static void test_option_none(libcpp::test::TestSuite& s)
{
	libcpp::Option<int> o = libcpp::Option<int>::none();
	ASSERT_TRUE(s, o.is_none());
	ASSERT_EQ(s, o.unwrap_or(0), 0);
}

/* ── Signal tests ───────────────────────────────────────────────────────── */

static int g_sig_val = 0;
static void on_signal(const int& v) { g_sig_val = v; }

static void test_signal_emit(libcpp::test::TestSuite& s)
{
	libcpp::Signal<int> sig;
	sig.connect(&on_signal);
	sig.emit(42);
	ASSERT_EQ(s, g_sig_val, 42);
}

/* ── Property tests ─────────────────────────────────────────────────────── */

static int g_prop_new = 0;
static void on_change(const libcpp::ChangePair<int>& p) { g_prop_new = p.new_val; }

static void test_property_change(libcpp::test::TestSuite& s)
{
	libcpp::Property<int> prop(0);
	prop.on_change.connect(&on_change);
	prop.set(55);
	ASSERT_EQ(s, prop.get(), 55);
	ASSERT_EQ(s, g_prop_new, 55);
}

/* ── Arena tests ────────────────────────────────────────────────────────── */

static void test_arena_alloc_free(libcpp::test::TestSuite& s)
{
	libcpp::Arena<double> arena(8);
	libcpp::Arena<double>::Index a = arena.alloc(3.14);
	libcpp::Arena<double>::Index b = arena.alloc(2.72);
	ASSERT_TRUE(s, a != b);
	ASSERT_TRUE(s, arena.is_alive(a));
	arena.free(a);
	ASSERT_FALSE(s, arena.is_alive(a));
}

/* ── Run ────────────────────────────────────────────────────────────────── */

void run_core_tests(void)
{
	libcpp::test::TestSuite s("core");
	s.test("Result::ok",         &test_result_ok);
	s.test("Result::err",        &test_result_err);
	s.test("Result::unwrap_or",  &test_result_unwrap_or);
	s.test("Result::map",        &test_result_map);
	s.test("Option::some",       &test_option_some);
	s.test("Option::none",       &test_option_none);
	s.test("Signal::emit",       &test_signal_emit);
	s.test("Property::change",   &test_property_change);
	s.test("Arena::alloc_free",  &test_arena_alloc_free);
	s.run();
}
