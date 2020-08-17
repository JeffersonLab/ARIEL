#ifndef fhiclcpp_types_OptionalTableAs_h
#define fhiclcpp_types_OptionalTableAs_h

#include "fhiclcpp/type_traits.h"
#include "fhiclcpp/types/ConfigPredicate.h"
#include "fhiclcpp/types/OptionalTable.h"
#include "fhiclcpp/types/detail/ParameterBase.h"
#include "fhiclcpp/types/detail/type_traits_error_msgs.h"

#include <memory>
#include <string>
#include <utility>

namespace fhicl {

  //==================================================================
  // e.g. OptionalTableAs<T, Config> ====> T as created by convert(Config)
  //
  template <typename T, typename Config>
  class OptionalTableAs {
  public:
    explicit OptionalTableAs(Name&& name);
    explicit OptionalTableAs(Name&& name, Comment&& comment);
    explicit OptionalTableAs(Name&& name,
                             Comment&& comment,
                             std::function<bool()> maybeUse);

    bool
    operator()(T& result) const
    {
      typename OptionalTable<Config>::value_type via;
      if (tableObj_(via)) {
        result = convert(via);
        return true;
      }
      return false;
    }

    bool
    hasValue() const
    {
      return tableObj_.hasValue();
    }

  private:
    OptionalTable<Config> tableObj_;

    Comment
    conversion_comment(Comment&& comment) const
    {
      std::string const preface =
        "N.B. The following table is converted to type:";
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

  template <typename T, typename Config>
  OptionalTableAs<T, Config>::OptionalTableAs(Name&& name)
    : OptionalTableAs{std::move(name), Comment("")}
  {}

  template <typename T, typename Config>
  OptionalTableAs<T, Config>::OptionalTableAs(Name&& name, Comment&& comment)
    : tableObj_{std::move(name), conversion_comment(std::move(comment))}
  {}

  template <typename T, typename Config>
  OptionalTableAs<T, Config>::OptionalTableAs(Name&& name,
                                              Comment&& comment,
                                              std::function<bool()> maybeUse)
    : tableObj_{std::move(name),
                conversion_comment(std::move(comment)),
                maybeUse}
  {}
}

#endif /* fhiclcpp_types_OptionalTableAs_h */

// Local variables:
// mode: c++
// End:
