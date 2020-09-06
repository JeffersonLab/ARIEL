#ifndef cetlib_BasicPluginFactory_h
#define cetlib_BasicPluginFactory_h
////////////////////////////////////////////////////////////////////////
// BasicPluginFactory
//
// Refinement of PluginFactory for a common class of plugins featuring a
// "pluginType" function and a "makePlugin" function. The pluginType
// function must be compatible with the following signature:
//
//   std::string ()
//
// For a useful class template / macro combination to formalize the
// pluginType, see cetlib/PluginTypeDeducer.h
//
// The make function is arbitrary in signature: the result type must be
// specified but the arguments (if any) may be deduced.
//
////////////////////////////////////////////////////////////////////////

#include "cetlib/PluginFactory.h"

namespace cet {
  class BasicPluginFactory;
}

class cet::BasicPluginFactory : public PluginFactory {
public:
  explicit BasicPluginFactory(
    cet::search_path const& search_path,
    std::string const& suffix = "plugin",
    std::string const& makerName = "makePlugin",
    std::string const& pluginTypeFuncName = "pluginType");

  explicit BasicPluginFactory(
    std::string const& suffix = "plugin",
    std::string const& makerName = "makePlugin",
    std::string const& pluginTypeFuncName = "pluginType");

  // Find and call the makePlugin() function in the plugin library.
  template <typename RESULT_TYPE, typename... ARGS>
  std::enable_if_t<!std::is_function_v<RESULT_TYPE>, RESULT_TYPE> makePlugin(
    std::string const& libspec,
    ARGS&&... args) const;

  template <typename FUNCTION_TYPE>
  std::enable_if_t<std::is_function_v<FUNCTION_TYPE>,
                   std::function<FUNCTION_TYPE>>
  makePlugin(std::string const& libspec) const;

  // Find and call the pluginType() function in the plugin library.
  std::string pluginType(std::string const& libspec) const;

private:
  std::string const makerName_;
  std::string const pluginTypeFuncName_;
};

inline std::string
cet::BasicPluginFactory::pluginType(std::string const& libspec) const
{
  return call<std::string>(libspec, pluginTypeFuncName_);
}

template <typename RESULT_TYPE, typename... ARGS>
inline std::enable_if_t<!std::is_function_v<RESULT_TYPE>, RESULT_TYPE>
cet::BasicPluginFactory::makePlugin(std::string const& libspec,
                                    ARGS&&... args) const
{
  return call<RESULT_TYPE>(libspec, makerName_, std::forward<ARGS>(args)...);
}

template <typename FUNCTION_TYPE>
inline std::enable_if_t<std::is_function_v<FUNCTION_TYPE>,
                        std::function<FUNCTION_TYPE>>
cet::BasicPluginFactory::makePlugin(std::string const& libspec) const
{
  return find<FUNCTION_TYPE>(libspec, makerName_);
}

#endif /* cetlib_BasicPluginFactory_h */

// Local Variables:
// mode: c++
// End:
