#ifndef fhiclcpp_types_Comment_h
#define fhiclcpp_types_Comment_h

// There are two ways to initialize a fhicl::Comment:
//
//    fhicl::Comment{"Hello world."}     ==> Argument is char const*
//    fhicl::Comment{std::to_string(47)} ==> Argument is std::string
//
// Technical comments:
//
//   Specifying a Comment constructor that takes an std::string can
//   yield ambiguities for Sequence objects, which support
//   initialization with default values via an std::initializer_list.
//   To get around this issue, a layer of indirection is added to the
//   Comment c'tor--the comment_detail::ArgWrapper, which supports
//   implicit conversion of a 'char const*' or 'std::string' argument
//   to itself.  The extra layer introduces an additional conversion
//   required for a Sequence object, making it disfavored in overload
//   resolution.

#include <string>

namespace fhicl {

  namespace comment_detail {
    struct ArgWrapper {
      ArgWrapper(char const* arg) : value{arg} {}
      ArgWrapper(std::string const& arg) : value{arg} {}
      std::string value;
    };
  }

  struct Comment {
    explicit Comment(comment_detail::ArgWrapper const& wrapper)
      : value{wrapper.value}
    {}
    std::string value;
  };
}

#endif /* fhiclcpp_types_Comment_h */

// Local variables:
// mode: c++
// End:
