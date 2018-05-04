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
    static_assert(!tt::is_sequence_type<T>::value, NO_STD_CONTAINERS);
    static_assert(!tt::is_fhicl_type<T>::value, NO_NESTED_FHICL_TYPES_IN_TABLE);
    static_assert(!tt::is_table_fragment<T>::value, NO_NESTED_TABLE_FRAGMENTS);
    static_assert(!tt::is_delegated_parameter<T>::value,
                  NO_DELEGATED_PARAMETERS);

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

#include "fhiclcpp/types/detail/OptionalTable.icc"

#endif /* fhiclcpp_types_OptionalTable_h */

// Local variables:
// mode: c++
// End:
