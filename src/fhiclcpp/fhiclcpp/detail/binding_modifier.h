#ifndef fhiclcpp_detail_binding_modifier_h
#define fhiclcpp_detail_binding_modifier_h
namespace fhicl {
  namespace detail {
    enum class binding_modifier : unsigned char {
      NONE,
      PROTECT_IGNORE,
      PROTECT_ERROR,
#if 0 /* Preparation for issue #7231. */
        INITIAL,
        REPLACE,
        REPLACE_COMPAT,
        ADD_OR_REPLACE_COMPAT
#endif
    };
  }
}

#endif /* fhiclcpp_detail_binding_modifier_h */

// Local Variables:
// mode: c++
// End:
