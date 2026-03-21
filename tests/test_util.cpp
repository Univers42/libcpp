/* ************************************************************************** */
/*                                                                            */
/*   test_util.cpp — integration tests for util module                        */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/test/suite.hpp"
#include "libcpp/util/argparser.hpp"
#include "libcpp/util/config.hpp"

/* ── ArgParser ──────────────────────────────────────────────────────────── */

static void test_argparser_flag(libcpp::test::TestSuite& s)
{
	libcpp::util::ArgParser ap("test", "test program");
	ap.flag("-v", "--verbose", "verbose mode");

	const char* argv[] = { "prog", "-v" };
	ASSERT_TRUE(s, ap.parse(2, const_cast<char**>(argv)));
	ASSERT_TRUE(s, ap.has("--verbose"));
}

static void test_argparser_option(libcpp::test::TestSuite& s)
{
	libcpp::util::ArgParser ap("test", "test program");
	ap.option("-o", "--output", "output file", "out.txt");

	const char* argv[] = { "prog", "--output=foo.txt" };
	ASSERT_TRUE(s, ap.parse(2, const_cast<char**>(argv)));
	ASSERT_EQ_STR(s, ap.get("--output"), "foo.txt");
}

static void test_argparser_positional(libcpp::test::TestSuite& s)
{
	libcpp::util::ArgParser ap("test", "test program");

	const char* argv[] = { "prog", "file1.txt", "file2.txt" };
	ASSERT_TRUE(s, ap.parse(3, const_cast<char**>(argv)));
	ASSERT_EQ(s, ap.positional_count(), 2);
}

/* ── Config ─────────────────────────────────────────────────────────────── */

static void test_config_load_string(libcpp::test::TestSuite& s)
{
	libcpp::util::Config cfg;
	cfg.load_string(
		"[server]\n"
		"host = localhost\n"
		"port = 8080\n"
		"debug = true\n"
	);
	ASSERT_TRUE(s, cfg.has("server", "host"));
	ASSERT_EQ_STR(s, cfg.get("server", "host"), "localhost");
	ASSERT_EQ(s, cfg.get_int("server", "port", 0), 8080);
	ASSERT_TRUE(s, cfg.get_bool("server", "debug", false));
}

static void test_config_default_values(libcpp::test::TestSuite& s)
{
	libcpp::util::Config cfg;
	ASSERT_EQ_STR(s, cfg.get("x", "y", "default"), "default");
	ASSERT_EQ(s, cfg.get_int("x", "y", 42), 42);
}

static void test_config_set(libcpp::test::TestSuite& s)
{
	libcpp::util::Config cfg;
	cfg.set("db", "name", "mydb");
	ASSERT_EQ_STR(s, cfg.get("db", "name"), "mydb");
	cfg.set("db", "name", "newdb");
	ASSERT_EQ_STR(s, cfg.get("db", "name"), "newdb");
}

/* ── Run ────────────────────────────────────────────────────────────────── */

void run_util_tests(void)
{
	libcpp::test::TestSuite s("util");
	s.test("ArgParser::flag",        &test_argparser_flag);
	s.test("ArgParser::option",      &test_argparser_option);
	s.test("ArgParser::positional",  &test_argparser_positional);
	s.test("Config::load_string",    &test_config_load_string);
	s.test("Config::default_values", &test_config_default_values);
	s.test("Config::set",            &test_config_set);
	s.run();
}
