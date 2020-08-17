#ifndef art_root_io_test_fastclonefail_v10_ClonedProd_h
#define art_root_io_test_fastclonefail_v10_ClonedProd_h
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Common/Wrapper.h"

namespace arttest {

  class ClonedProd {
  public:
    ~ClonedProd() noexcept;
    ClonedProd() noexcept;

  public:
    int dp1_{3};
  };

} // namespace arttest

#endif /* art_root_io_test_fastclonefail_v10_ClonedProd_h */

// Local Variables:
// mode: c++
// End:
