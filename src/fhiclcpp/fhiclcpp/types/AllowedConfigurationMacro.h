#ifndef fhiclcpp_types_AllowedConfigurationMacro_h
#define fhiclcpp_types_AllowedConfigurationMacro_h

// =====================================================================
//
// Extern allowed-configuration macro
//
// Use: Fill in
//
// =====================================================================

#include "cetlib/compiler_macros.h"
#include "cetlib/metaprogramming.h"
#include "fhiclcpp/types/ConfigurationTable.h"
#include "fhiclcpp/types/Name.h"

#include <memory>
#include <string>

namespace fhicl {
  namespace detail {

    template <class T, class Enable = void>
    struct AllowedConfiguration {
      static std::unique_ptr<fhicl::ConfigurationTable>
      get(std::string const& /*name*/)
      {
        return std::unique_ptr<fhicl::ConfigurationTable>{nullptr};
      }
    };

    template <class T>
    struct AllowedConfiguration<
      T,
      cet::enable_if_type_exists_t<typename T::Parameters>> {
      static std::unique_ptr<fhicl::ConfigurationTable>
      get(std::string const& name)
      {
        return std::make_unique<typename T::Parameters>(fhicl::Name{name});
      }
    };
  }
}

#define FHICL_PROVIDE_ALLOWED_CONFIGURATION(klass)                  \
  EXTERN_C_FUNC_DECLARE_START                                       \
  std::unique_ptr<fhicl::ConfigurationTable> allowed_configuration( \
    std::string const& name)                                        \
  {                                                                 \
    return fhicl::detail::AllowedConfiguration<klass>::get(name);   \
  }                                                                 \
  EXTERN_C_FUNC_DECLARE_END

#endif /* fhiclcpp_types_AllowedConfigurationMacro_h */

// Local Variables:
// mode: c++
// End:
