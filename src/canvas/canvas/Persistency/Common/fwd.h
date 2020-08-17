#ifndef canvas_Persistency_Common_fwd_h
#define canvas_Persistency_Common_fwd_h

// ======================================================================
//
// Forward declarations of types in Persistency/Common
//
// ======================================================================

#include "canvas/Utilities/TypeID.h"
#include "canvas/Utilities/product_metatype.h"

namespace art {
  namespace detail {
    class AssnsBase;
  }

  template <typename L, typename R, typename D>
  class Assns;
  template <typename L, typename R>
  class Assns<L, R, void>;
  class EDProduct;
  class EDProductGetter;
  class GroupQueryResult;
  class HLTGlobalStatus;
  class HLTPathStatus;
  class OutputHandle;
  template <typename T>
  class Ptr;
  template <typename T>
  class PtrVector;
  class PtrVectorBase;
  class ProductID;
  class RefCore;
  template <typename T>
  class Sampled;
  template <typename T>
  class Wrapper;
  using product_typeids_t = std::map<product_metatype, TypeID>;
} // art

// ======================================================================

#endif /* canvas_Persistency_Common_fwd_h */

// Local Variables:
// mode: c++
// End:
