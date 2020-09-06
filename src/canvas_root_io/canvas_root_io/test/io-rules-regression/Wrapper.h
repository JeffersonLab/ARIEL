#ifndef art_root_io_test_io_rules_regression_Wrapper
#define art_root_io_test_io_rules_regression_Wrapper

#include "EDProduct.h"

#include <memory>
#include <stdexcept>

namespace arttest {
  template <typename T>
  class Wrapper;
}

template <typename T>
class arttest::Wrapper : public arttest::EDProduct {
public:
  Wrapper() = default;
  explicit Wrapper(std::unique_ptr<T> ptr);
  virtual ~Wrapper() noexcept = default;

  T const* product() const;
  T const* operator->() const;

  static short
  Class_Version()
  {
    return 10;
  }

private:
  bool
  isPresent_() const override
  {
    return present;
  }
  T&& refOrThrow(T* ptr);

  bool present{false};
  unsigned rangeSetID{-1u};
  T obj{};
};

template <typename T>
arttest::Wrapper<T>::Wrapper(std::unique_ptr<T> ptr)
  : present{ptr.get() != nullptr}, rangeSetID{-1u}, obj(refOrThrow(ptr.get()))
{}

template <typename T>
T const*
arttest::Wrapper<T>::product() const
{
  return present ? &obj : nullptr;
}

template <typename T>
T const* arttest::Wrapper<T>::operator->() const
{
  return product();
}

template <typename T>
inline T&&
arttest::Wrapper<T>::refOrThrow(T* ptr)
{
  if (ptr) {
    return std::move(*ptr);
  } else {
    throw std::runtime_error("Attempt to construct from nullptr\n");
  }
}

#endif /* art_root_io_test_io_rules_regression_Wrapper */

// Local Variables:
// mode: c++
// End:
