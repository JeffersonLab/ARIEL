#ifndef fhiclcpp_types_OptionalTupleAs_h
#define fhiclcpp_types_OptionalTupleAs_h

#include "fhiclcpp/type_traits.h"
#include "fhiclcpp/types/ConfigPredicate.h"
#include "fhiclcpp/types/OptionalTuple.h"
#include "fhiclcpp/types/detail/NameStackRegistry.h"
#include "fhiclcpp/types/detail/ParameterBase.h"
#include "fhiclcpp/types/detail/type_traits_error_msgs.h"

#include <memory>
#include <string>
#include <tuple>
#include <utility>

namespace fhicl {

  //==================================================================
  // e.g. OptionalTupleAs<T,int,double,bool> ====> T(int,double,bool)
  //
  template <typename T, typename... ARGS>
  class OptionalTupleAs;

  template <typename T, typename... ARGS>
  class OptionalTupleAs<T(ARGS...)> {
  public:
    explicit OptionalTupleAs(Name&& name);
    explicit OptionalTupleAs(Name&& name, Comment&& comment);
    explicit OptionalTupleAs(Name&& name,
                             Comment&& comment,
                             std::function<bool()> maybeUse);

    bool
    operator()(T& result) const
    {
      typename OptionalTuple<ARGS...>::value_type via;
      if (tupleObj_(via)) {
        result = std::make_from_tuple(via);
        return true;
      }
      return false;
    }

    bool
    hasValue() const
    {
      return tupleObj_.hasValue();
    }

  private:
    OptionalTuple<ARGS...> tupleObj_;

    Comment
    conversion_comment(Comment&& comment) const
    {
      std::string const preface =
        "N.B. The following sequence is converted to type:";
      std::string const name =
        "        '" + cet::demangle_symbol(typeid(T).name()) + "'";
      std::string const user_comment =
        comment.value.empty() ? "" : "\n\n" + comment.value;

      std::ostringstream oss;
      oss << preface << '\n' << name << user_comment;

      return Comment{oss.str().c_str()};
    }
  };

  //==================================================================
  // IMPLEMENTATION

  template <typename T, typename... ARGS>
  OptionalTupleAs<T(ARGS...)>::OptionalTupleAs(Name&& name)
    : OptionalTupleAs{std::move(name), Comment("")}
  {}

  template <typename T, typename... ARGS>
  OptionalTupleAs<T(ARGS...)>::OptionalTupleAs(Name&& name, Comment&& comment)
    : tupleObj_{std::move(name), conversion_comment(std::move(comment))}
  {}

  template <typename T, typename... ARGS>
  OptionalTupleAs<T(ARGS...)>::OptionalTupleAs(Name&& name,
                                               Comment&& comment,
                                               std::function<bool()> maybeUse)
    : tupleObj_{std::move(name),
                conversion_comment(std::move(comment)),
                maybeUse}
  {}
}

#endif /* fhiclcpp_types_OptionalTupleAs_h */

// Local variables:
// mode: c++
// End:
