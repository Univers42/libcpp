#pragma once
// perlin_noise_2d.hpp — 2D Perlin noise generator.
//
// Classic gradient noise seeded via mt19937. Provides a sample(x, y)
// function that returns a value in [-1, 1].

#include <cstdint>
#include <vector>

namespace libcpp {
namespace math {

class PerlinNoise2D {
public:
    explicit PerlinNoise2D(uint64_t seed);

    // Sample noise at fractional coordinates, returns [-1, 1]
    double sample(double x, double y) const;

    // Multi-octave sampling
    double octave(double x, double y, int octaves,
                  double persistence = 0.5) const;

private:
    static double _fade(double t);
    static double _lerp(double a, double b, double t);
    static double _grad(int hash, double x, double y);

    std::vector<int> _perm; // permutation table (size 512)
};

} // namespace math
} // namespace libcpp
