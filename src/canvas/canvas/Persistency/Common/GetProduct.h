#ifndef canvas_Persistency_Common_GetProduct_h
#define canvas_Persistency_Common_GetProduct_h

namespace art::detail {
  template <typename COLLECTION>
  struct GetProduct {
    using const_iterator = typename COLLECTION::const_iterator;
    static auto const*
    address(const_iterator const& i)
    {
      return &*i;
    }
    static COLLECTION const*
    product(COLLECTION const& coll)
    {
      return &coll;
    }
  };
}

#endif /* canvas_Persistency_Common_GetProduct_h */

// Local Variables:
// mode: c++
// End:
