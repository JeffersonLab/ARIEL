#ifndef canvas_test_Persistency_Common_MockRun_h
#define canvas_test_Persistency_Common_MockRun_h

#include "canvas/Persistency/Common/Wrapper.h"

#include <memory>
#include <utility>
#include <vector>

namespace arttest {

  class MockRun {
  public:
    template <typename T, typename... ARGS>
    void
    put(ARGS&&... args)
    {
      auto prod = std::make_unique<T>(std::forward<ARGS>(args)...);
      products_.push_back(new art::Wrapper<T>(std::move(prod)));
    }

    template <typename T>
    T
    get() const
    {
      // Retrieve and aggregate the products
      auto result = static_cast<art::Wrapper<T>&>(*products_[0]);
      for (auto it = products_.cbegin() + 1, e = products_.cend(); it != e;
           ++it) {
        result.combine(*it);
      }
      return *result.product();
    }

  private:
    std::vector<art::EDProduct*> products_;
  };
}

#endif /* canvas_test_Persistency_Common_MockRun_h */

// Local variables:
// mode: c++
// End:
