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
    static_assert(!tt::is_sequence_type<T>::value, NO_STD_CONTAINERS);
    static_assert(!tt::is_fhicl_type<T>::value, NO_NESTED_FHICL_TYPES_IN_TABLE);
    static_assert(!tt::is_table_fragment<T>::value, NO_NESTED_TABLE_FRAGMENTS);
    static_assert(!tt::is_delegated_parameter<T>::value,
                  NO_DELEGATED_PARAMETERS);

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

    struct Impl {
    };
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

#include "fhiclcpp/types/detail/Table.icc"

#endif /* fhiclcpp_types_Table_h */

// Local variables:
// mode: c++
// End:
