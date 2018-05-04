#ifndef canvas_Utilities_WrappedTypeID_h
#define canvas_Utilities_WrappedTypeID_h
// vim: set sw=2:
//
// WrappedTypeID: A unique identifier for a C++ type.
//
// The identifier is unique within an entire program, but cannot be
// persisted across invocations of the program.

#include "canvas/Utilities/TypeID.h"

namespace art {
  template <typename T>
  class Wrapper;

  struct WrappedTypeID;
}

struct art::WrappedTypeID {
  template <typename T>
  static WrappedTypeID make();
  TypeID product_type;
  TypeID wrapped_product_type;

private:
  WrappedTypeID(std::type_info const& product_type,
                std::type_info const& wrapped_product_type);
};

inline art::WrappedTypeID::WrappedTypeID(
  std::type_info const& prod_type,
  std::type_info const& wrapped_prod_type)
  : product_type{prod_type}, wrapped_product_type{wrapped_prod_type}
{}

template <typename T>
art::WrappedTypeID
art::WrappedTypeID::make()
{
  return WrappedTypeID{typeid(T), typeid(Wrapper<T>)};
}

// Local Variables:
// mode: c++
// End:
#endif /* canvas_Utilities_WrappedTypeID_h */
