#ifndef cetlib_ProvideMakePluginMacros_h
#define cetlib_ProvideMakePluginMacros_h

#include "cetlib/compiler_macros.h"

#define MAKE_PLUGIN_START(result_type, ...)                                    \
  EXTERN_C_FUNC_DECLARE_START result_type makePlugin(__VA_ARGS__)
#define MAKE_PLUGIN_END EXTERN_C_FUNC_DECLARE_END

#endif /* cetlib_ProvideMakePluginMacros_h */
