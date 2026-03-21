#pragma once
// random_2d_coordinate_generator.hpp — Seeded 2D coordinate generator.
//
// Produces unique random IVector2 coordinates within a bounding box.

#include "libcpp/math/ivector2.hpp"
#include <cstdint>
#include <random>
#include <set>

namespace libcpp {
namespace math {

class Random2DCoordinateGenerator {
public:
    // Seed the generator and define the bounding rectangle
    Random2DCoordinateGenerator(uint64_t seed, const IVector2& min,
                                const IVector2& max);

    // Generate a random, non-repeating coordinate
    // Throws if all coordinates are exhausted.
    IVector2 operator()();

    // Reset the generator (allows previously generated coordinates again)
    void reset();

    // Number of remaining unique coordinates
    size_t remaining() const;

private:
    uint64_t                  _seed;
    IVector2                  _min;
    IVector2                  _max;
    std::mt19937_64           _rng;
    std::set<std::pair<int,int>> _used;
    size_t                    _total;
};

} // namespace math
} // namespace libcpp
