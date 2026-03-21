/* ************************************************************************** */
/*                                                                            */
/*   main.cpp — test runner entry point                                       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

/* Forward declarations of test suites */
void run_core_tests(void);
void run_str_tests(void);
void run_util_tests(void);
void run_data_tests(void);
void run_bench_tests(void);
void run_mem_tests(void);
void run_libftpp_tests(void);

int main(void)
{
	std::cout << "\n=== libcpp integration tests ===\n" << std::endl;

	run_core_tests();
	run_str_tests();
	run_util_tests();
	run_data_tests();
	run_bench_tests();
	run_mem_tests();
	run_libftpp_tests();

	std::cout << "\n=== all suites complete ===\n" << std::endl;
	return 0;
}
