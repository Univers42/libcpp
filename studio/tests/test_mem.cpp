/* ************************************************************************** */
/*                                                                            */
/*   test_mem.cpp — integration tests for mem module                          */
/*                                                                            */
/* ************************************************************************** */

#include "libcpp/mem/pool.hpp"
#include "libcpp/test/suite.hpp"

static void test_pool_alloc_dealloc(libcpp::test::TestSuite& s) {
  libcpp::mem::Pool<int, 16> pool;
  int* a = pool.alloc(42);
  ASSERT_TRUE(s, a != 0);
  ASSERT_EQ(s, *a, 42);
  pool.dealloc(a);
  ASSERT_EQ(s, static_cast<int>(pool.available()), 16);
}

static void test_pool_full(libcpp::test::TestSuite& s) {
  libcpp::mem::Pool<int, 4> pool;
  int* ptrs[4];
  for (int i = 0; i < 4; ++i) ptrs[i] = pool.alloc(i);
  ASSERT_EQ(s, static_cast<int>(pool.available()), 0);
  /* next alloc should return NULL */
  int* extra = pool.alloc(99);
  ASSERT_TRUE(s, extra == 0);
  /* free one and re-alloc */
  pool.dealloc(ptrs[0]);
  extra = pool.alloc(100);
  ASSERT_TRUE(s, extra != 0);
  ASSERT_EQ(s, *extra, 100);
}

void run_mem_tests(void) {
  libcpp::test::TestSuite s("mem");
  s.test("Pool::alloc_dealloc", &test_pool_alloc_dealloc);
  s.test("Pool::full", &test_pool_full);
  s.run();
}
