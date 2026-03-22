#pragma once
// singleton.hpp — Singleton<TType> design pattern.
//
// Ensures a class has only one instance. The managed class must declare
// Singleton<MyClass> as a friend to allow instantiation.

#include <memory>
#include <stdexcept>

namespace libcpp {
namespace core {

/// @brief Singleton pattern — ensures exactly one instance of TType.
/// Managed class should declare Singleton<T> as friend.
template <typename TType>
class Singleton {
 public:
  [[nodiscard]] static TType* instance() { return _instance.get(); }

  [[nodiscard]] static bool isInstantiated() { return _instance != nullptr; }

  template <typename... TArgs>
  static void instantiate(TArgs&&... p_args) {
    if (_instance) throw std::runtime_error("Singleton: already instantiated");
    _instance =
        std::unique_ptr<TType>(new TType(std::forward<TArgs>(p_args)...));
  }

  static void destroy() { _instance.reset(); }

 private:
  static std::unique_ptr<TType> _instance;
};

template <typename TType>
std::unique_ptr<TType> Singleton<TType>::_instance = nullptr;

}  // namespace core
}  // namespace libcpp
