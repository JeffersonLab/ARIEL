#ifndef cetlib_PluginTypeDeducer_h
#define cetlib_PluginTypeDeducer_h

////////////////////////////////////////////////////////////////////////
// PluginTypeDeducer
//
// An aid to managing multiple plugin types. See
// cetlib/BasicPluginFactory.h for background.
//
// Specialize this class for your own plugin type and make sure it is
// seen before being required. A reasonable place would be the in the
// header file of the plugin type's base class (assuming there is one),
// immediately below the forward declaration of the base class name.
//
// Define a macro DEFINE_BASIC_PLUGINTYPE_FUNC(base) to produce a
// pluginType() function for a plugin library which should be findable
// by BasicPluginFactory::pluginType(const std::string & libspec).
//
// PluginTypeDeducer<> is used rather than a sinple string literal to
// allow for the ability for users of the plugin factory to compare the
// answer with a known value to verify the type of the plugin found.
////////////////////////////////////////////////////////////////////////

#include "cetlib/compiler_macros.h"

#include <string>

namespace cet {
  // PluginTypeDeducer.
  template <typename T>
  struct PluginTypeDeducer;
}

#define DEFINE_BASIC_PLUGINTYPE_FUNC(base)                                     \
  EXTERN_C_FUNC_DECLARE_START                                                  \
  std::string pluginType() { return cet::PluginTypeDeducer<base>::value; }     \
  EXTERN_C_FUNC_DECLARE_END

template <typename T>
struct cet::PluginTypeDeducer {
  static std::string const value;
};

template <typename T>
std::string const cet::PluginTypeDeducer<T>::value = "Unknown";

#endif /* cetlib_PluginTypeDeducer_h */

// Local Variables:
// mode: c++
// End:
