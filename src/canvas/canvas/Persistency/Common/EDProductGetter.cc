#include "canvas/Persistency/Common/EDProductGetter.h"
// vim: set sw=2 expandtab :

namespace art {

  EDProductGetter::~EDProductGetter() {}

  EDProductGetter::EDProductGetter() {}

  EDProduct const*
  EDProductGetter::getIt() const
  {
    return getIt_();
  }

  EDProduct const*
  EDProductGetter::getIt_() const
  {
    return nullptr;
  }

} // namespace art
