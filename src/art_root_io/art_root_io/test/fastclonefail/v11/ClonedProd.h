#ifndef art_root_io_test_fastclonefail_v11_ClonedProd_h
#define art_root_io_test_fastclonefail_v11_ClonedProd_h
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Common/Wrapper.h"

namespace arttest {

  class ClonedProd {
  public:
    ~ClonedProd() noexcept;
    ClonedProd() noexcept;

  public:
    double dp1_{5.0};
  };

} // namespace arttest

#endif /* art_root_io_test_fastclonefail_v11_ClonedProd_h */

// Local Variables:
// mode: c++
// End:
