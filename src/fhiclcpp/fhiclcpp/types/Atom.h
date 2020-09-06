#ifndef fhiclcpp_types_Atom_h
#define fhiclcpp_types_Atom_h

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

#include <memory>
#include <sstream>
#include <string>

namespace fhicl {

  //========================================================
  template <typename T>
  class Atom final : public detail::AtomBase,
                     private detail::RegisterIfTableMember {
  public:
    static_assert(!tt::is_sequence_type_v<T>, NO_STD_CONTAINERS);
    static_assert(!tt::is_fhicl_type_v<T>, NO_NESTED_FHICL_TYPES_IN_ATOM);
    static_assert(!tt::is_table_fragment_v<T>, NO_NESTED_TABLE_FRAGMENTS);
    static_assert(!tt::is_delegated_parameter_v<T>, NO_DELEGATED_PARAMETERS);

    //=====================================================
    // User-friendly
    // ... c'tors
    explicit Atom(Name&& name);
    explicit Atom(Name&& name, Comment&& comment);
    explicit Atom(Name&& name, Comment&& comment, std::function<bool()> useIf);

    // ... c'tors supporting defaults
    explicit Atom(Name&& name, T const& dflt_value);
    explicit Atom(Name&& name, Comment&& comment, T const& dflt_value);
    explicit Atom(Name&& name,
                  Comment&& comment,
                  std::function<bool()> useIf,
                  T const& dflt_value);

    // ... Accessors
    auto const&
    operator()() const
    {
      return *value_;
    }

    // Expert-only
    using default_type = T;
    using value_type = T;

  private:
    std::shared_ptr<T> value_;

    std::string get_stringified_value() const override;
    void do_set_value(fhicl::ParameterSet const&, bool const) override;
  };
}

// Implementation
#include "fhiclcpp/detail/printing_helpers.h"
#include "fhiclcpp/types/detail/strip_containing_names.h"

namespace fhicl {

  template <typename T>
  Atom<T>::Atom(Name&& name, Comment&& comment)
    : AtomBase{std::move(name),
               std::move(comment),
               par_style::REQUIRED,
               detail::AlwaysUse()}
    , RegisterIfTableMember{this}
    , value_{std::make_shared<T>()}
  {
    NameStackRegistry::end_of_ctor();
  }

  template <typename T>
  Atom<T>::Atom(Name&& name, Comment&& comment, std::function<bool()> maybeUse)
    : AtomBase{std::move(name),
               std::move(comment),
               par_style::REQUIRED_CONDITIONAL,
               maybeUse}
    , RegisterIfTableMember{this}
    , value_{std::make_shared<T>()}
  {
    NameStackRegistry::end_of_ctor();
  }

  template <typename T>
  Atom<T>::Atom(Name&& name, Comment&& comment, T const& dflt_value)
    : AtomBase{std::move(name),
               std::move(comment),
               par_style::DEFAULT,
               detail::AlwaysUse()}
    , RegisterIfTableMember{this}
    , value_{std::make_shared<T>(dflt_value)}
  {
    NameStackRegistry::end_of_ctor();
  }

  template <typename T>
  Atom<T>::Atom(Name&& name,
                Comment&& comment,
                std::function<bool()> maybeUse,
                T const& dflt_value)
    : AtomBase{std::move(name),
               std::move(comment),
               par_style::DEFAULT_CONDITIONAL,
               maybeUse}
    , RegisterIfTableMember{this}
    , value_{std::make_shared<T>(dflt_value)}
  {
    NameStackRegistry::end_of_ctor();
  }

  template <typename T>
  Atom<T>::Atom(Name&& name) : Atom{std::move(name), Comment("")}
  {}

  template <typename T>
  Atom<T>::Atom(Name&& name, T const& dflt_value)
    : Atom{std::move(name), Comment(""), dflt_value}
  {}

  template <typename T>
  std::string
  Atom<T>::get_stringified_value() const
  {
    std::stringstream oss;
    if (has_default()) {
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
  Atom<T>::do_set_value(fhicl::ParameterSet const& pset, bool const trimParent)
  {
    std::string const& rkey = key();
    std::string const& key =
      trimParent ? detail::strip_first_containing_name(rkey) : rkey;

    if (has_default())
      pset.get_if_present<T>(key, *value_);
    else
      value_ = std::make_shared<T>(pset.get<T>(key));
  }
}

#endif /* fhiclcpp_types_Atom_h */

// Local variables:
// mode: c++
// End:
