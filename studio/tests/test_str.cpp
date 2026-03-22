/* ************************************************************************** */
/*                                                                            */
/*   test_str.cpp — integration tests for str module                          */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include "libcpp/str/case.hpp"
#include "libcpp/str/format.hpp"
#include "libcpp/str/utf8.hpp"
#include "libcpp/test/suite.hpp"

static void test_utf8_len(libcpp::test::TestSuite& s) {
  ASSERT_EQ(s, static_cast<int>(libcpp::str::utf8_len("hello")), 5);
  ASSERT_EQ(s, static_cast<int>(libcpp::str::utf8_len("")), 0);
}

static void test_utf8_valid(libcpp::test::TestSuite& s) {
  ASSERT_TRUE(s, libcpp::str::utf8_valid("abc"));
}

static void test_trim(libcpp::test::TestSuite& s) {
  ASSERT_EQ_STR(s, libcpp::str::trim("  hi  "), "hi");
}

static void test_pad(libcpp::test::TestSuite& s) {
  std::string r = libcpp::str::pad_right("ab", 5, ' ');
  ASSERT_EQ(s, static_cast<int>(r.size()), 5);
  ASSERT_EQ_STR(s, r, "ab   ");
}

static void test_repeat(libcpp::test::TestSuite& s) {
  ASSERT_EQ_STR(s, libcpp::str::repeat("ab", 3), "ababab");
}

static void test_join_split(libcpp::test::TestSuite& s) {
  std::string arr[3];
  arr[0] = "a";
  arr[1] = "b";
  arr[2] = "c";
  ASSERT_EQ_STR(s, libcpp::str::join(arr, 3, ","), "a,b,c");

  std::string parts[8];
  int n = libcpp::str::split("a,b,c", ',', parts, 8);
  ASSERT_EQ(s, n, 3);
  ASSERT_EQ_STR(s, parts[0], "a");
}

static void test_starts_ends(libcpp::test::TestSuite& s) {
  ASSERT_TRUE(s, libcpp::str::starts_with("hello world", "hello"));
  ASSERT_TRUE(s, libcpp::str::ends_with("hello world", "world"));
  ASSERT_FALSE(s, libcpp::str::starts_with("hello", "world"));
}

static void test_replace_all(libcpp::test::TestSuite& s) {
  ASSERT_EQ_STR(s, libcpp::str::replace_all("aabbcc", "bb", "XX"), "aaXXcc");
}

static void test_fmt(libcpp::test::TestSuite& s) {
  ASSERT_EQ_STR(s, libcpp::str::fmt("hello {0} {1}", "world", "42"),
                "hello world 42");
}

static void test_to_upper(libcpp::test::TestSuite& s) {
  ASSERT_EQ_STR(s, libcpp::str::to_upper("hello"), "HELLO");
}

static void test_to_lower(libcpp::test::TestSuite& s) {
  ASSERT_EQ_STR(s, libcpp::str::to_lower("HELLO"), "hello");
}

static void test_to_snake_case(libcpp::test::TestSuite& s) {
  ASSERT_EQ_STR(s, libcpp::str::to_snake_case("helloWorld"), "hello_world");
}

static void test_to_camel_case(libcpp::test::TestSuite& s) {
  ASSERT_EQ_STR(s, libcpp::str::to_camel_case("hello_world"), "helloWorld");
}

static void test_eq_nocase(libcpp::test::TestSuite& s) {
  ASSERT_TRUE(s, libcpp::str::eq_nocase("ABC", "abc"));
}

static void test_contains(libcpp::test::TestSuite& s) {
  ASSERT_TRUE(s, libcpp::str::contains("hello world", "world"));
  ASSERT_FALSE(s, libcpp::str::contains("hello world", "xyz"));
}

static void test_count(libcpp::test::TestSuite& s) {
  ASSERT_EQ(s, libcpp::str::count("abcabc", "abc"), 2);
  ASSERT_EQ(s, libcpp::str::count("aaa", "z"), 0);
}

static void test_reverse(libcpp::test::TestSuite& s) {
  ASSERT_EQ_STR(s, libcpp::str::reverse("abc"), "cba");
}

static void test_center(libcpp::test::TestSuite& s) {
  ASSERT_EQ_STR(s, libcpp::str::center("hi", 6, '-'), "--hi--");
}

static void test_truncate(libcpp::test::TestSuite& s) {
  ASSERT_EQ_STR(s, libcpp::str::truncate("hello world", 8, "..."), "hello...");
}

static void test_is_blank(libcpp::test::TestSuite& s) {
  ASSERT_TRUE(s, libcpp::str::is_blank("   \t\n"));
  ASSERT_FALSE(s, libcpp::str::is_blank("  x  "));
  ASSERT_TRUE(s, libcpp::str::is_empty(""));
  ASSERT_FALSE(s, libcpp::str::is_empty(" "));
}

static void test_capitalize(libcpp::test::TestSuite& s) {
  ASSERT_EQ_STR(s, libcpp::str::capitalize("hello"), "Hello");
  ASSERT_EQ_STR(s, libcpp::str::capitalize(""), "");
}

void run_str_tests(void) {
  libcpp::test::TestSuite s("str");
  s.test("utf8_len", &test_utf8_len);
  s.test("utf8_valid", &test_utf8_valid);
  s.test("trim", &test_trim);
  s.test("pad", &test_pad);
  s.test("repeat", &test_repeat);
  s.test("join/split", &test_join_split);
  s.test("starts/ends", &test_starts_ends);
  s.test("replace_all", &test_replace_all);
  s.test("fmt", &test_fmt);
  s.test("to_upper", &test_to_upper);
  s.test("to_lower", &test_to_lower);
  s.test("to_snake_case", &test_to_snake_case);
  s.test("to_camel_case", &test_to_camel_case);
  s.test("eq_nocase", &test_eq_nocase);
  s.test("contains", &test_contains);
  s.test("count", &test_count);
  s.test("reverse", &test_reverse);
  s.test("center", &test_center);
  s.test("truncate", &test_truncate);
  s.test("is_blank/empty", &test_is_blank);
  s.test("capitalize", &test_capitalize);
  s.run();
}
