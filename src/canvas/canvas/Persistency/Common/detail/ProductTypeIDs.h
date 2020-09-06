#ifndef canvas_Persistency_Common_detail_ProductTypeIDs_h
#define canvas_Persistency_Common_detail_ProductTypeIDs_h

#include "canvas/Persistency/Common/fwd.h"

namespace art {
  namespace detail {
    template <typename T>
    inline auto
    wrapperTypeID()
    {
      return TypeID{typeid(art::Wrapper<T>)};
    }

    template <typename T>
    struct ProductTypeIDs {
      static product_typeids_t
      get()
      {
        return {{product_metatype::Normal, wrapperTypeID<T>()}};
      }
    };

    template <typename L, typename R>
    struct ProductTypeIDs<Assns<L, R, void>> {
      using LR_t = Assns<L, R, void>;
      using RL_t = typename LR_t::partner_t;
      static product_typeids_t
      get()
      {
        return {{product_metatype::LeftRight, wrapperTypeID<LR_t>()},
                {product_metatype::RightLeft, wrapperTypeID<RL_t>()}};
      }
    };

    template <typename L, typename R, typename D>
    struct ProductTypeIDs<Assns<L, R, D>> {
      using LR_t = Assns<L, R, void>;
      using RL_t = typename LR_t::partner_t;
      using LRD_t = Assns<L, R, D>;
      using RLD_t = typename LRD_t::partner_t;
      static product_typeids_t
      get()
      {
        return {{product_metatype::LeftRight, wrapperTypeID<LR_t>()},
                {product_metatype::RightLeft, wrapperTypeID<RL_t>()},
                {product_metatype::LeftRightData, wrapperTypeID<LRD_t>()},
                {product_metatype::RightLeftData, wrapperTypeID<RLD_t>()}};
      }
    };
  }
}
#endif /* canvas_Persistency_Common_detail_ProductTypeIDs_h */

// Local Variables:
// mode: c++
// End:
