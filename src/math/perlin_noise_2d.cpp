// perlin_noise_2d.cpp — PerlinNoise2D implementation.

#include "libcpp/math/perlin_noise_2d.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace libcpp {
namespace math {

PerlinNoise2D::PerlinNoise2D(uint64_t seed)
{
    // Build permutation table [0..255], shuffle, duplicate
    _perm.resize(512);
    std::vector<int> base(256);
    std::iota(base.begin(), base.end(), 0);

    std::mt19937_64 rng(seed);
    std::shuffle(base.begin(), base.end(), rng);

    for (int i = 0; i < 256; ++i) {
        _perm[i]       = base[static_cast<size_t>(i)];
        _perm[i + 256] = base[static_cast<size_t>(i)];
    }
}

double PerlinNoise2D::sample(double x, double y) const
{
    // Unit square containing the point
    int xi = static_cast<int>(std::floor(x)) & 255;
    int yi = static_cast<int>(std::floor(y)) & 255;

    // Relative position in the cell
    double xf = x - std::floor(x);
    double yf = y - std::floor(y);

    // Fade curves
    double u = _fade(xf);
    double v = _fade(yf);

    // Hash coordinates of the 4 corners
    int aa = _perm[static_cast<size_t>(_perm[static_cast<size_t>(xi)] + yi)];
    int ab = _perm[static_cast<size_t>(_perm[static_cast<size_t>(xi)] + yi + 1)];
    int ba = _perm[static_cast<size_t>(_perm[static_cast<size_t>(xi + 1)] + yi)];
    int bb = _perm[static_cast<size_t>(_perm[static_cast<size_t>(xi + 1)] + yi + 1)];

    // Bilinear interpolation of gradient contributions
    double x1 = _lerp(_grad(aa, xf, yf), _grad(ba, xf - 1.0, yf), u);
    double x2 = _lerp(_grad(ab, xf, yf - 1.0), _grad(bb, xf - 1.0, yf - 1.0), u);

    return _lerp(x1, x2, v);
}

double PerlinNoise2D::octave(double x, double y, int octaves,
                              double persistence) const
{
    double total     = 0.0;
    double frequency = 1.0;
    double amplitude = 1.0;
    double maxVal    = 0.0;

    for (int i = 0; i < octaves; ++i) {
        total += sample(x * frequency, y * frequency) * amplitude;
        maxVal    += amplitude;
        amplitude *= persistence;
        frequency *= 2.0;
    }
    return total / maxVal;
}

double PerlinNoise2D::_fade(double t)
{
    // 6t^5 - 15t^4 + 10t^3
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

double PerlinNoise2D::_lerp(double a, double b, double t)
{
    return a + t * (b - a);
}

double PerlinNoise2D::_grad(int hash, double x, double y)
{
    // Use lower 2 bits to pick gradient direction
    int h = hash & 3;
    double u = (h < 2) ? x : y;
    double v = (h < 2) ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

} // namespace math
} // namespace libcpp
