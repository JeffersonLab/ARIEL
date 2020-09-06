#ifndef fhiclcpp_types_OptionalTable_h
#define fhiclcpp_types_OptionalTable_h

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/type_traits.h"
#include "fhiclcpp/types/Comment.h"
#include "fhiclcpp/types/ConfigPredicate.h"
#include "fhiclcpp/types/Name.h"
#include "fhiclcpp/types/detail/NameStackRegistry.h"
#include "fhiclcpp/types/detail/ParameterArgumentTypes.h"
#include "fhiclcpp/types/detail/ParameterMetadata.h"
#include "fhiclcpp/types/detail/TableBase.h"
#include "fhiclcpp/types/detail/TableMemberRegistry.h"
#include "fhiclcpp/types/detail/type_traits_error_msgs.h"

#include <memory>
#include <set>
#include <string>

namespace fhicl {

  //========================================================
  template <typename T>
  class OptionalTable final : public detail::TableBase,
                              private detail::RegisterIfTableMember {
  public:
    static_assert(!tt::is_sequence_type_v<T>, NO_STD_CONTAINERS);
    static_assert(!tt::is_fhicl_type_v<T>, NO_NESTED_FHICL_TYPES_IN_TABLE);
    static_assert(!tt::is_table_fragment_v<T>, NO_NESTED_TABLE_FRAGMENTS);
    static_assert(!tt::is_delegated_parameter_v<T>, NO_DELEGATED_PARAMETERS);

    //=====================================================
    // User-friendly
    // ... c'tors
    explicit OptionalTable(Name&& name);
    explicit OptionalTable(Name&& name, Comment&& comment);
    explicit OptionalTable(Name&& name,
                           Comment&& comment,
                           std::function<bool()> maybeUse);
    OptionalTable(ParameterSet const& pset,
                  std::set<std::string> const& keysToIgnore);

    // ... Accessors
    bool
    operator()(T& value) const
    {
      if (has_value_) {
        value = *value_;
        return true;
      }
      return false;
    }

    bool
    hasValue() const
    {
      return has_value_;
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
    using value_type = T;

    OptionalTable();

  private:
    using members_t = std::vector<cet::exempt_ptr<ParameterBase>>;

    std::shared_ptr<T> value_{std::make_shared<T>()};
    bool has_value_{false};
    ParameterSet pset_{};
    members_t members_{
      detail::TableMemberRegistry::instance().release_members()};

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
  operator<<(std::ostream& os, OptionalTable<T> const& t)
  {
    std::ostringstream config;
    t.print_allowed_configuration(config);
    return os << config.str();
  }
}

#include "cetlib/container_algorithms.h"
#include "fhiclcpp/detail/printing_helpers.h"
#include "fhiclcpp/types/detail/PrintAllowedConfiguration.h"
#include "fhiclcpp/types/detail/ValidateThenSet.h"
#include "fhiclcpp/types/detail/optional_parameter_message.h"
#include "fhiclcpp/types/detail/strip_containing_names.h"

namespace fhicl {

  template <typename T>
  OptionalTable<T>::OptionalTable(Name&& name)
    : OptionalTable{std::move(name), Comment("")}
  {}

  template <typename T>
  OptionalTable<T>::OptionalTable(Name&& name, Comment&& comment)
    : TableBase{std::move(name),
                std::move(comment),
                par_style::OPTIONAL,
                detail::AlwaysUse()}
    , RegisterIfTableMember{this}
  {
    NameStackRegistry::end_of_ctor();
  }

  template <typename T>
  OptionalTable<T>::OptionalTable(Name&& name,
                                  Comment&& comment,
                                  std::function<bool()> maybeUse)
    : TableBase{std::move(name),
                std::move(comment),
                par_style::OPTIONAL_CONDITIONAL,
                maybeUse}
    , RegisterIfTableMember{this}
  {
    NameStackRegistry::end_of_ctor();
  }

  template <typename T>
  OptionalTable<T>::OptionalTable(ParameterSet const& pset,
                                  std::set<std::string> const& keysToIgnore)
    : TableBase{Name("<top_level>"),
                Comment(""),
                par_style::OPTIONAL,
                detail::AlwaysUse()}
    , RegisterIfTableMember{this}
  {
    validate_ParameterSet(pset, keysToIgnore);
    NameStackRegistry::end_of_ctor();
  }

  template <typename T>
  void
  OptionalTable<T>::validate_ParameterSet(
    ParameterSet const& pset,
    std::set<std::string> const& keysToIgnore)
  {
    pset_ = pset;
    detail::ValidateThenSet vs{pset_, keysToIgnore};
    cet::for_all(members(), [&vs](auto m) { vs(m.get()); });
    vs.check_keys();
  }

  template <typename T>
  void
  OptionalTable<T>::print_allowed_configuration(std::ostream& os,
                                                std::string const& tab) const
  {
    os << '\n' << tab << detail::optional_parameter_message() << '\n';
    detail::PrintAllowedConfiguration pc{os};
    pc.walk_over(*this);
  }

  template <typename T>
  void
  OptionalTable<T>::do_set_value(fhicl::ParameterSet const& pset,
                                 bool const /*trimParent*/)
  {
    // Kind of tricky: we do not have the name of the current
    // parameter set.  A placeholder is often used
    // (e.g. "<top_level>").  Fortunately, since the pset is passed
    // in, we can just assign to it for a top-level ParameterSet.
    // However, for nested parameter sets, we need to trim off the
    // placeholder, and then the key we send
    // pset.get<fhicl::ParameterSet>(key) is the key relative to the
    // top-level pset.
    std::string const& rkey = key();
    std::string const& nkey = detail::strip_first_containing_name(rkey);
    if (nkey == rkey) {
      pset_ = pset;
      has_value_ = true;
    } else {
      has_value_ = pset.get_if_present<fhicl::ParameterSet>(nkey, pset_);
    }
  }
}

#endif /* fhiclcpp_types_OptionalTable_h */

// Local variables:
// mode: c++
// End:
