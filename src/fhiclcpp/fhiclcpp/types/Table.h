#ifndef fhiclcpp_types_Table_h
#define fhiclcpp_types_Table_h

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/type_traits.h"
#include "fhiclcpp/types/Comment.h"
#include "fhiclcpp/types/ConfigPredicate.h"
#include "fhiclcpp/types/MaybeUseFunction.h"
#include "fhiclcpp/types/Name.h"
#include "fhiclcpp/types/detail/NameStackRegistry.h"
#include "fhiclcpp/types/detail/ParameterArgumentTypes.h"
#include "fhiclcpp/types/detail/ParameterMetadata.h"
#include "fhiclcpp/types/detail/TableBase.h"
#include "fhiclcpp/types/detail/TableMemberRegistry.h"
#include "fhiclcpp/types/detail/type_traits_error_msgs.h"

#include <memory>
#include <ostream>
#include <set>
#include <string>

namespace fhicl {

  //========================================================
  template <typename T, typename KeysToIgnore = void>
  class Table final : public detail::TableBase,
                      private detail::RegisterIfTableMember {
  public:
    static_assert(!tt::is_sequence_type_v<T>, NO_STD_CONTAINERS);
    static_assert(!tt::is_fhicl_type_v<T>, NO_NESTED_FHICL_TYPES_IN_TABLE);
    static_assert(!tt::is_table_fragment_v<T>, NO_NESTED_TABLE_FRAGMENTS);
    static_assert(!tt::is_delegated_parameter_v<T>, NO_DELEGATED_PARAMETERS);

    //=====================================================
    // User-friendly
    // ... c'tors
    template <typename... TCARGS>
    explicit Table(Name&& name, TCARGS&&... tcargs);
    template <typename... TCARGS>
    explicit Table(Name&& name, Comment&& comment, TCARGS&&... tcargs);
    template <typename... TCARGS>
    explicit Table(Name&& name,
                   Comment&& comment,
                   MaybeUseFunction maybeUse,
                   TCARGS&&... tcargs);

    // Choose this c'tor if user specifies the second 'KeysToIgnore' template
    // argument.
    template <typename K = KeysToIgnore,
              typename = std::enable_if_t<tt::is_callable<K>::value>>
    Table(ParameterSet const& pset);

    // If user does not specify the second 'KeysToIgnore' template argument,
    // then choose this c'tor.
    template <typename K = KeysToIgnore,
              typename = std::enable_if_t<!tt::is_callable<K>::value>>
    Table(ParameterSet const& pset,
          std::set<std::string> const& keysToIgnore = {});

    // ... Accessors
    auto const&
    operator()() const
    {
      return *value_;
    }

    ParameterSet const&
    get_PSet() const
    {
      return pset_;
    }

    void validate_ParameterSet(ParameterSet const& pset,
                               std::set<std::string> const& keysToIgnore = {});

    void print_allowed_configuration(
      std::ostream& os,
      std::string const& tab = std::string(3, ' ')) const;

    //=====================================================
    // Expert-only
    using default_type = T;
    using value_type = T;

  private:
    using members_t = std::vector<cet::exempt_ptr<ParameterBase>>;

    std::shared_ptr<T> value_{std::make_shared<T>()};
    ParameterSet pset_{};
    members_t members_{
      detail::TableMemberRegistry::instance().release_members()};

    struct Impl {};
    Table(ParameterSet const&, std::set<std::string> const&, Impl);
    void maybe_implicitly_default();

    members_t const&
    get_members() const override
    {
      return members_;
    }
    void do_set_value(fhicl::ParameterSet const& pset,
                      bool const trimParents) override;
  };

  template <typename T>
  inline std::ostream&
  operator<<(std::ostream& os, Table<T> const& t)
  {
    std::ostringstream config;
    t.print_allowed_configuration(config);
    return os << config.str();
  }
}

// Implementation

#include "cetlib/container_algorithms.h"
#include "fhiclcpp/detail/printing_helpers.h"
#include "fhiclcpp/types/detail/PrintAllowedConfiguration.h"
#include "fhiclcpp/types/detail/ValidateThenSet.h"
#include "fhiclcpp/types/detail/optional_parameter_message.h"
#include "fhiclcpp/types/detail/strip_containing_names.h"
#include "fhiclcpp/types/detail/validationException.h"

namespace fhicl {

  template <typename T, typename KeysToIgnore>
  template <typename... TCARGS>
  Table<T, KeysToIgnore>::Table(Name&& name, TCARGS&&... tcargs)
    : Table{std::move(name), Comment(""), std::forward<TCARGS>(tcargs)...}
  {}

  template <typename T, typename KeysToIgnore>
  template <typename... TCARGS>
  Table<T, KeysToIgnore>::Table(Name&& name,
                                Comment&& comment,
                                TCARGS&&... tcargs)
    : TableBase{std::move(name),
                std::move(comment),
                par_style::REQUIRED,
                detail::AlwaysUse()}
    , RegisterIfTableMember{this}
    , value_{std::make_shared<T>(std::forward<TCARGS>(tcargs)...)}
  {
    maybe_implicitly_default();
    NameStackRegistry::end_of_ctor();
  }

  template <typename T, typename KeysToIgnore>
  template <typename... TCARGS>
  Table<T, KeysToIgnore>::Table(Name&& name,
                                Comment&& comment,
                                MaybeUseFunction maybeUse,
                                TCARGS&&... tcargs)
    : TableBase{std::move(name),
                std::move(comment),
                par_style::REQUIRED_CONDITIONAL,
                maybeUse}
    , RegisterIfTableMember{this}
    , value_{std::make_shared<T>(std::forward<TCARGS>(tcargs)...)}
  {
    maybe_implicitly_default();
    NameStackRegistry::end_of_ctor();
  }

  template <typename T, typename KeysToIgnore>
  template <typename, typename>
  Table<T, KeysToIgnore>::Table(ParameterSet const& pset)
    : Table{pset, KeysToIgnore{}(), Impl{}}
  {}

  template <typename T, typename KeysToIgnore>
  template <typename, typename>
  Table<T, KeysToIgnore>::Table(ParameterSet const& pset,
                                std::set<std::string> const& keysToIgnore)
    : Table{pset, keysToIgnore, Impl{}}
  {}

  template <typename T, typename KeysToIgnore>
  Table<T, KeysToIgnore>::Table(ParameterSet const& pset,
                                std::set<std::string> const& keysToIgnore,
                                Impl)
    : TableBase{Name("<top_level>"),
                Comment(""),
                par_style::REQUIRED,
                detail::AlwaysUse()}
    , RegisterIfTableMember{this}
  {
    maybe_implicitly_default();
    validate_ParameterSet(pset, keysToIgnore);
    NameStackRegistry::end_of_ctor();
  }

  template <typename T, typename KeysToIgnore>
  void
  Table<T, KeysToIgnore>::validate_ParameterSet(
    ParameterSet const& pset,
    std::set<std::string> const& keysToIgnore)
  {
    pset_ = pset;
    detail::ValidateThenSet vs{pset_, keysToIgnore};
    cet::for_all(members(), [&vs](auto m) { vs.walk_over(*m); });

    try {
      vs.check_keys();
    }
    catch (fhicl::detail::validationException const&) {
      NameStackRegistry::instance().clear();
      throw;
    }
  }

  template <typename T, typename KeysToIgnore>
  void
  Table<T, KeysToIgnore>::print_allowed_configuration(
    std::ostream& os,
    std::string const& tab) const
  {
    os << '\n' << tab << detail::optional_parameter_message() << '\n';
    detail::PrintAllowedConfiguration pc{os, false, tab};
    pc.walk_over(*this);
  }

  template <typename T, typename KeysToIgnore>
  void
  Table<T, KeysToIgnore>::do_set_value(fhicl::ParameterSet const& pset,
                                       bool const /*trimParent*/)
  {
    // Kind of tricky: we do not have the name of the current
    // parameter set.  A placeholder is often used (e.g. "<top_level>").
    // Fortunately, since the pset is passed in, we can just assign to
    // it for a top-level ParameterSet.  However, for nested parameter
    // sets, we need to trim off the placeholder, and then the key we
    // send pset.get<fhicl::ParameterSet>(key) is the key relative to
    // the top-level pset.
    std::string const& rkey = key();
    std::string const& nkey = detail::strip_first_containing_name(rkey);
    pset_ = (nkey == rkey) ? pset : pset.get<fhicl::ParameterSet>(nkey);
  }

  template <typename T, typename KeysToIgnore>
  void
  Table<T, KeysToIgnore>::maybe_implicitly_default()
  {
    bool const implicitly_default =
      std::all_of(members_.begin(), members_.end(), [](auto p) {
        return p->has_default() || p->is_optional();
      });
    if (implicitly_default)
      set_par_style(par_style::DEFAULT);
  }
}

#endif /* fhiclcpp_types_Table_h */

// Local variables:
// mode: c++
// End:
