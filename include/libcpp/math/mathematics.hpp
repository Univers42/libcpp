#pragma once
// mathematics.hpp — section header required by the libftpp subject.
//
// Every module in this section requires C++11 or later (= delete,
// = default, default member initializers, <thread>/<chrono>). Under
// -std=c++98 this header is empty rather than a parse error, so a C++98
// consumer can include it unconditionally. See libcpp/config.hpp.
#include "libcpp/config.hpp"

#if LIBCPP_HAS_CXX11
# include "libcpp/math/ivector2.hpp"
# include "libcpp/math/ivector3.hpp"
# include "libcpp/math/perlin_noise_2d.hpp"
# include "libcpp/math/random_2d_coordinate_generator.hpp"
#endif /* LIBCPP_HAS_CXX11 */
