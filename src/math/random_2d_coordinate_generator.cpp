// random_2d_coordinate_generator.cpp — Random2DCoordinateGenerator impl.

#include "libcpp/math/random_2d_coordinate_generator.hpp"
#include <stdexcept>

namespace libcpp {
namespace math {

Random2DCoordinateGenerator::Random2DCoordinateGenerator(uint64_t seed,
                                                         const IVector2& min,
                                                         const IVector2& max)
    : _seed(seed), _min(min), _max(max), _rng(seed) {
  if (_max.x < _min.x || _max.y < _min.y)
    throw std::runtime_error("Random2DCoordinateGenerator: max < min");
  _total = static_cast<size_t>(_max.x - _min.x + 1) *
           static_cast<size_t>(_max.y - _min.y + 1);
}

IVector2 Random2DCoordinateGenerator::operator()() {
  if (_used.size() >= _total)
    throw std::runtime_error(
        "Random2DCoordinateGenerator: all coordinates exhausted");

  std::uniform_int_distribution<int> distX(_min.x, _max.x);
  std::uniform_int_distribution<int> distY(_min.y, _max.y);

  while (true) {
    int x = distX(_rng);
    int y = distY(_rng);
    if (_used.emplace(x, y).second) return IVector2(x, y);
  }
}

void Random2DCoordinateGenerator::reset() {
  _rng.seed(_seed);
  _used.clear();
}

size_t Random2DCoordinateGenerator::remaining() const {
  return _total - _used.size();
}

}  // namespace math
}  // namespace libcpp
