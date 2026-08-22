#pragma once
// data_structures.hpp — section header required by the libftpp subject.
//
// Mixed section: csv/database/date are C++98-clean, data_buffer/pool need
// C++11. Under -std=c++98 the modern half is left out. See libcpp/config.hpp.
#include "libcpp/config.hpp"

#include "libcpp/data/csv.hpp"
#include "libcpp/data/database.hpp"
#include "libcpp/data/date.hpp"

#if LIBCPP_HAS_CXX11
# include "libcpp/data/data_buffer.hpp"
# include "libcpp/data/pool.hpp"
#endif /* LIBCPP_HAS_CXX11 */
