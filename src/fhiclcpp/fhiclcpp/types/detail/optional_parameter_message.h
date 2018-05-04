#ifndef fhiclcpp_types_detail_optional_parameter_message_h
#define fhiclcpp_types_detail_optional_parameter_message_h

namespace fhicl {
  namespace detail {
    inline std::string
    optional_parameter_message(bool const with_comment = true)
    {
      std::string result;
      if (with_comment) {
        result += "## ";
      }
      result += "Any parameters prefaced with '#' are optional.";
      return result;
    }
  }
}

#endif /* fhiclcpp_types_detail_optional_parameter_message_h */

// Local variables:
// mode: c++
// End:
