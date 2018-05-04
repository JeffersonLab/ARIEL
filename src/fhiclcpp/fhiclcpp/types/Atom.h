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
    static_assert(!tt::is_sequence_type<T>::value, NO_STD_CONTAINERS);
    static_assert(!tt::is_fhicl_type<T>::value, NO_NESTED_FHICL_TYPES_IN_ATOM);
    static_assert(!tt::is_table_fragment<T>::value, NO_NESTED_TABLE_FRAGMENTS);
    static_assert(!tt::is_delegated_parameter<T>::value,
                  NO_DELEGATED_PARAMETERS);

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

#include "fhiclcpp/types/detail/Atom.icc"

#endif /* fhiclcpp_types_Atom_h */

// Local variables:
// mode: c++
// End:
