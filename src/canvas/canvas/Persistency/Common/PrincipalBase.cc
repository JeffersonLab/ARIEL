#include "canvas/Persistency/Common/PrincipalBase.h"
// vim: set sw=2 expandtab :

namespace art {

  PrincipalBase::~PrincipalBase() {}
  PrincipalBase::PrincipalBase() {}

  // Note: Used only by canvas RefCoreStreamer.cc through
  // PrincipalBase::getEDProductGetter!
  EDProductGetter const*
  PrincipalBase::getEDProductGetter(ProductID const& pid) const
  {
    return getEDProductGetter_(pid);
  }

} // namespace art
