/* ************************************************************************** */
/*                                                                            */
/*   test_data.cpp — integration tests for data module                        */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/test/suite.hpp"
#include "libcpp/data/date.hpp"
#include "libcpp/data/csv.hpp"
#include "libcpp/data/database.hpp"
#include <sstream>

/* ── Date ───────────────────────────────────────────────────────────────── */

static void test_date_construct(libcpp::test::TestSuite& s)
{
	libcpp::data::Date d(2024, 3, 15);
	ASSERT_EQ(s, d.year(), 2024);
	ASSERT_EQ(s, d.month(), 3);
	ASSERT_EQ(s, d.day(), 15);
}

static void test_date_iso(libcpp::test::TestSuite& s)
{
	libcpp::data::Date d(2024, 1, 5);
	ASSERT_EQ_STR(s, d.to_iso(), "2024-01-05");
}

static void test_date_parse(libcpp::test::TestSuite& s)
{
	libcpp::data::Date d = libcpp::data::Date::parse("2024-06-15");
	ASSERT_EQ(s, d.year(), 2024);
	ASSERT_EQ(s, d.month(), 6);
	ASSERT_EQ(s, d.day(), 15);
}

static void test_date_add_days(libcpp::test::TestSuite& s)
{
	libcpp::data::Date d(2024, 1, 31);
	libcpp::data::Date d2 = d.add_days(1);
	ASSERT_EQ(s, d2.month(), 2);
	ASSERT_EQ(s, d2.day(), 1);
}

static void test_date_leap_year(libcpp::test::TestSuite& s)
{
	ASSERT_TRUE(s, libcpp::data::Date::is_leap_year(2024));
	ASSERT_FALSE(s, libcpp::data::Date::is_leap_year(2023));
}

static void test_date_comparison(libcpp::test::TestSuite& s)
{
	libcpp::data::Date a(2024, 1, 1);
	libcpp::data::Date b(2024, 1, 2);
	ASSERT_TRUE(s, a < b);
	ASSERT_TRUE(s, b > a);
	ASSERT_TRUE(s, a != b);
}

static void test_daterange(libcpp::test::TestSuite& s)
{
	libcpp::data::Date a(2024, 1, 1);
	libcpp::data::Date b(2024, 1, 10);
	libcpp::data::DateRange r(a, b);
	ASSERT_EQ(s, r.days(), 9);
	ASSERT_TRUE(s, r.contains(libcpp::data::Date(2024, 1, 5)));
	ASSERT_FALSE(s, r.contains(libcpp::data::Date(2024, 2, 1)));
}

/* ── CSV ────────────────────────────────────────────────────────────────── */

static void test_csv_parse_stream(libcpp::test::TestSuite& s)
{
	std::istringstream iss("Name,Age\nAlice,30\nBob,25\n");
	libcpp::data::CsvDocument doc;
	ASSERT_TRUE(s, doc.load_stream(iss));
	ASSERT_EQ(s, static_cast<int>(doc.row_count()), 2);
	ASSERT_EQ(s, static_cast<int>(doc.column_count()), 2);
	ASSERT_EQ_STR(s, doc.headers()[0], "Name");
}

static void test_csv_aggregate(libcpp::test::TestSuite& s)
{
	std::istringstream iss("Val\n10\n20\n30\n");
	libcpp::data::CsvDocument doc;
	doc.load_stream(iss);
	ASSERT_EQ_DBL(s, libcpp::data::csv_sum(doc, "Val"), 60.0, 0.01);
	ASSERT_EQ_DBL(s, libcpp::data::csv_average(doc, "Val"), 20.0, 0.01);
	ASSERT_EQ_DBL(s, libcpp::data::csv_min(doc, "Val"), 10.0, 0.01);
	ASSERT_EQ_DBL(s, libcpp::data::csv_max(doc, "Val"), 30.0, 0.01);
}

/* ── Database ───────────────────────────────────────────────────────────── */

static void test_database_add_query(libcpp::test::TestSuite& s)
{
	libcpp::data::Database db;
	db.add_column("Name");
	db.add_column("Dept");

	std::map<std::string, std::string> r1;
	r1["Name"] = "Alice";
	r1["Dept"] = "Eng";
	db.add_row(r1);

	std::map<std::string, std::string> r2;
	r2["Name"] = "Bob";
	r2["Dept"] = "HR";
	db.add_row(r2);

	ASSERT_EQ(s, static_cast<int>(db.count()), 2);
	std::vector<libcpp::data::DbRow> res = db.where("Dept", "Eng");
	ASSERT_EQ(s, static_cast<int>(res.size()), 1);
	ASSERT_EQ_STR(s, res[0].get_value("Name"), "Alice");
}

/* ── Run ────────────────────────────────────────────────────────────────── */

void run_data_tests(void)
{
	libcpp::test::TestSuite s("data");
	s.test("Date::construct",    &test_date_construct);
	s.test("Date::iso",          &test_date_iso);
	s.test("Date::parse",        &test_date_parse);
	s.test("Date::add_days",     &test_date_add_days);
	s.test("Date::leap_year",    &test_date_leap_year);
	s.test("Date::comparison",   &test_date_comparison);
	s.test("DateRange",          &test_daterange);
	s.test("CSV::parse_stream",  &test_csv_parse_stream);
	s.test("CSV::aggregate",     &test_csv_aggregate);
	s.test("Database::add_query", &test_database_add_query);
	s.run();
}
