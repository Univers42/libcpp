/* ************************************************************************** */
/*                                                                            */
/*   demo/main.cpp — end-to-end showcase of libcpp modules                    */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/libcpp.hpp"
#include <iostream>
#include <sstream>

/* ── helpers ────────────────────────────────────────────────────────────── */

static void on_value_change(const libcpp::ChangePair<int>& p)
{
	std::cout << "  [Property] changed from "
			  << p.old_val << " to " << p.new_val << std::endl;
}

static void greet_test(libcpp::test::TestSuite& s)
{
	ASSERT_EQ_STR(s, libcpp::str::fmt("hello {0}", "world"), "hello world");
}

static void trim_test(libcpp::test::TestSuite& s)
{
	ASSERT_EQ_STR(s, libcpp::str::trim("  hi  "), "hi");
}

static void bench_noop(void)
{
	volatile int x = 0;
	for (int i = 0; i < 1000; ++i) x += i;
	(void)x;
}

/* ── main ───────────────────────────────────────────────────────────────── */

int main(int argc, char** argv)
{
	/* ── 1. ArgParser ──────────────────────────────────────────── */
	std::cout << "\n=== libcpp demo ===" << std::endl;

	libcpp::util::ArgParser ap("demo", "libcpp demo program");
	ap.flag("-v", "--verbose", "enable verbose output");
	ap.option("-n", "--name", "your name", "world");
	ap.parse(argc, argv);

	bool verbose = ap.has("--verbose");
	std::string name = ap.get("--name");

	std::cout << "\n[ArgParser] name=" << name
			  << " verbose=" << (verbose ? "true" : "false") << std::endl;

	/* ── 2. Config ─────────────────────────────────────────────── */
	libcpp::util::Config cfg;
	cfg.load_string(
		"[app]\n"
		"title = libcpp demo\n"
		"version = 1.0\n"
		"[logging]\n"
		"level = info\n"
	);
	std::cout << "\n[Config] title=" << cfg.get("app", "title")
			  << "  version=" << cfg.get("app", "version") << std::endl;

	/* ── 3. String utilities ───────────────────────────────────── */
	std::cout << "\n[str] to_upper(\"hello\") = "
			  << libcpp::str::to_upper("hello") << std::endl;
	std::cout << "[str] to_snake_case(\"helloWorld\") = "
			  << libcpp::str::to_snake_case("helloWorld") << std::endl;
	std::cout << "[str] fmt(\"{0} has {1} items\", \"box\", \"5\") = "
			  << libcpp::str::fmt("{0} has {1} items", "box", "5") << std::endl;

	/* ── 4. Signal & Property ──────────────────────────────────── */
	std::cout << "\n[Property]" << std::endl;
	libcpp::Property<int> prop(10);
	prop.on_change.connect(&on_value_change);
	prop.set(42);

	/* ── 5. Result & Option ────────────────────────────────────── */
	libcpp::Result<int, int> ok = libcpp::Result<int, int>::ok(42);
	std::cout << "\n[Result] ok.unwrap() = " << ok.unwrap() << std::endl;

	libcpp::Option<int> opt = libcpp::Option<int>::some(7);
	std::cout << "[Option] some.unwrap() = " << opt.unwrap() << std::endl;

	/* ── 6. Date ───────────────────────────────────────────────── */
	libcpp::data::Date today(2024, 6, 15);
	std::cout << "\n[Date] " << today.to_iso()
			  << "  leap=" << (libcpp::data::Date::is_leap_year(2024) ? "yes" : "no")
			  << std::endl;

	/* ── 7. CSV ────────────────────────────────────────────────── */
	std::istringstream csv_data("Name,Score\nAlice,90\nBob,85\n");
	libcpp::data::CsvDocument doc;
	doc.load_stream(csv_data);
	std::cout << "\n[CSV] rows=" << doc.row_count()
			  << "  sum(Score)=" << libcpp::data::csv_sum(doc, "Score")
			  << std::endl;

	/* ── 8. Database ───────────────────────────────────────────── */
	libcpp::data::Database db;
	db.add_column("Name");
	db.add_column("Dept");

	std::map<std::string, std::string> row;
	row["Name"] = "Alice"; row["Dept"] = "Eng";
	db.add_row(row);
	row["Name"] = "Bob";   row["Dept"] = "HR";
	db.add_row(row);

	std::cout << "\n[Database] count=" << db.count()
			  << "  where(Dept=Eng)=" << db.where("Dept", "Eng").size()
			  << std::endl;

	/* ── 9. TestSuite mini run ─────────────────────────────────── */
	std::cout << std::endl;
	libcpp::test::TestSuite ts("demo");
	ts.test("greet", &greet_test);
	ts.test("trim",  &trim_test);
	ts.run();

	/* ── 10. Benchmark mini run ────────────────────────────────── */
	std::cout << std::endl;
	libcpp::bench::Benchmark bm("demo_bench");
	bm.add("noop_loop", &bench_noop);
	bm.iterations(100).warmup(5);
	bm.run();

	/* ── 11. Arena ─────────────────────────────────────────────── */
	libcpp::Arena<int> arena(8);
	libcpp::Arena<int>::Index a = arena.alloc(100);
	libcpp::Arena<int>::Index b = arena.alloc(200);
	std::cout << "\n[Arena] get(a)=" << arena.get(a)
			  << "  get(b)=" << arena.get(b)
			  << "  count=" << arena.count() << std::endl;
	arena.free(a);
	std::cout << "[Arena] after free(a): count=" << arena.count()
			  << "  a alive=" << (arena.is_alive(a) ? "yes" : "no")
			  << std::endl;

	/* ── 12. Pool ──────────────────────────────────────────────── */
	libcpp::mem::Pool<int, 4> pool;
	int* p1 = pool.alloc(10);
	int* p2 = pool.alloc(20);
	std::cout << "\n[Pool] *p1=" << *p1 << " *p2=" << *p2
			  << "  available=" << pool.available() << std::endl;
	pool.dealloc(p1);
	std::cout << "[Pool] after dealloc: available=" << pool.available()
			  << std::endl;

	std::cout << "\n=== demo complete ===\n" << std::endl;
	return 0;
}
