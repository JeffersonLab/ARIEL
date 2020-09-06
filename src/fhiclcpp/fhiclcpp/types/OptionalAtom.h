#ifndef fhiclcpp_types_OptionalAtom_h
#define fhiclcpp_types_OptionalAtom_h

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/type_traits.h"
#include "fhiclcpp/types/Comment.h"
#include "fhiclcpp/types/ConfigPredicate.h"
#include "fhiclcpp/types/Name.h"
#include "fhiclcpp/types/detail/AtomBase.h"
#include "fhiclcpp/types/detail/NameStackRegistry.h"
#include "fhiclcpp/types/detail/ParameterMetadata.h"
#include "fhiclcpp/types/detail/TableMemberRegistry.h"
#include "fhiclcpp/types/detail/ostream_helpers.h"
#include "fhiclcpp/types/detail/type_traits_error_msgs.h"

#include <optional>
#include <sstream>
#include <string>

namespace fhicl {

  //========================================================
  template <typename T>
  class OptionalAtom final : public detail::AtomBase,
                             private detail::RegisterIfTableMember {
  public:
    static_assert(!tt::is_sequence_type_v<T>, NO_STD_CONTAINERS);
    static_assert(!tt::is_fhicl_type_v<T>, NO_NESTED_FHICL_TYPES_IN_ATOM);
    static_assert(!tt::is_table_fragment_v<T>, NO_NESTED_TABLE_FRAGMENTS);
    static_assert(!tt::is_delegated_parameter_v<T>, NO_DELEGATED_PARAMETERS);

    //=====================================================
    // User-friendly
    // ... c'tors
    explicit OptionalAtom(Name&& name);
    explicit OptionalAtom(Name&& name, Comment&& comment);
    explicit OptionalAtom(Name&& name,
                          Comment&& comment,
                          std::function<bool()> maybeUse);

    // ... Accessors
    bool
    operator()(T& value) const
    {
      if (hasValue()) {
        value = *value_;
        return true;
      }
      return false;
    }

    bool
    hasValue() const
    {
      return value_.has_value();
    }

    // Expert-only
    using value_type = T;

    OptionalAtom();

  private:
    std::optional<T> value_{};

    std::string get_stringified_value() const override;
    void do_set_value(fhicl::ParameterSet const&, bool const) override;
  };
}

#include "fhiclcpp/detail/printing_helpers.h"
#include "fhiclcpp/types/detail/strip_containing_names.h"

namespace fhicl {

  template <typename T>
  OptionalAtom<T>::OptionalAtom(Name&& name)
    : OptionalAtom{std::move(name), Comment("")}
  {}

  template <typename T>
  OptionalAtom<T>::OptionalAtom(Name&& name, Comment&& comment)
    : AtomBase{std::move(name),
               std::move(comment),
               par_style::OPTIONAL,
               detail::AlwaysUse()}
    , RegisterIfTableMember{this}
  {
    NameStackRegistry::end_of_ctor();
  }

  template <typename T>
  OptionalAtom<T>::OptionalAtom(Name&& name,
                                Comment&& comment,
                                std::function<bool()> maybeUse)
    : AtomBase{std::move(name),
               std::move(comment),
               par_style::OPTIONAL_CONDITIONAL,
               maybeUse}
    , RegisterIfTableMember{this}
  {
    NameStackRegistry::end_of_ctor();
  }

  template <typename T>
  std::string
  OptionalAtom<T>::get_stringified_value() const
  {
    std::stringstream oss;
    if (value_) {
      using namespace detail::yes_defaults;
      oss << maybe_quotes<T>(*value_);
    } else {
      using namespace detail::no_defaults;
      oss << expected_types<T>();
    }
    return oss.str();
  }

  template <typename T>
  void
  OptionalAtom<T>::do_set_value(fhicl::ParameterSet const& pset,
                                bool const trimParent)
  {
    std::string const& rkey = key();
    std::string const& key =
      trimParent ? detail::strip_first_containing_name(rkey) : rkey;
    T tmp{};
    if (pset.get_if_present<T>(key, tmp)) {
      value_ = std::move(tmp);
    }
  }
}

#endif /* fhiclcpp_types_OptionalAtom_h */

// Local variables:
// mode: c++
// End:
