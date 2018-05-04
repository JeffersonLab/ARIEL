// ======================================================================
//
// RefCore: The component of art::Ptr containing
//            - the product ID and
//            - the product getter.
//
// ======================================================================

#include "canvas/Persistency/Common/RefCore.h"

#include "canvas/Utilities/Exception.h"
#include <cassert>

bool
art::RefCore::isAvailable() const
{
  return productPtr() != nullptr ||
         (id_.isValid() && productGetter() != 0 && productGetter()->isReady() &&
          productGetter()->getIt() != nullptr);
}
