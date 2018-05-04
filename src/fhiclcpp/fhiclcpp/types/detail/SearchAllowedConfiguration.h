#ifndef fhiclcpp_types_detail_SearchAllowedConfiguration_h
#define fhiclcpp_types_detail_SearchAllowedConfiguration_h

// ============================================================================
//
// SearchAllowedConfiguration
//
// This class provides an interface for determining if a supplied
// ParameterBase object supports a specified key.
//
// Implemented behavior:
//
//   - All querying of allowed configurations happens through the
//     static member function 'supports_key' (the default c'tor of
//     SearchAllowedConfiguration is private).
//
//   - The supplied key is interpreted to be relative to the
//     ParameterBase object's key.  For example, if the key of the
//     ParameterBase object is 'table1.table2', and the user
//     specifies:
//
//       bool const result {SearchAllowedConfiguration::supports_key(pb,
//       "atom")};
//
//     then the searched-for key is "table.table2.atom".
//
//   - Specifying a ParameterBase object that is not of sequence type
//     or table type throws an exception.
//
//   - Specifying an empty searched-for key throws an exception.
//
// The searching happens by using the fhicl::detail::ParameterWalker
// interface.  With the current implementation, if a match is found,
// the walker continues checking for matches for the rest of the
// parameters of the given nesting level, without descending further.
// This is an inefficiency that should be optimized if the need
// arises.  Note, however, that the only way to completely stop a tree
// walk with the current implementation is to throw an exception,
// which we do not do.
//
// ============================================================================

#include "fhiclcpp/type_traits.h"
#include "fhiclcpp/types/detail/ParameterArgumentTypes.h"
#include "fhiclcpp/types/detail/ParameterWalker.h"

#include <string>

namespace fhicl {
  namespace detail {

    class SearchAllowedConfiguration
      : public ParameterWalker<tt::const_flavor::require_const> {
    public:
      static bool supports_key(ParameterBase const& pb,
                               std::string const& searched_for_key);

    private:
      std::string key_;
      bool supportsKey_{false};

      SearchAllowedConfiguration(ParameterBase const& pb,
                                 std::string const& searched_for_key);

      bool
      result() const
      {
        return supportsKey_;
      };

      bool before_action(ParameterBase const& pb) override;
      void
      enter_table(TableBase const&) override
      {}
      void
      enter_sequence(SequenceBase const&) override
      {}
      void
      atom(AtomBase const&) override
      {}
      void
      delegated_parameter(DelegateBase const&) override
      {}

      static std::string form_absolute(ParameterBase const& pb,
                                       std::string const& searched_for_key);
    };
  }
}

#endif /* fhiclcpp_types_detail_SearchAllowedConfiguration_h */

// Local variables:
// mode: c++
// End:
