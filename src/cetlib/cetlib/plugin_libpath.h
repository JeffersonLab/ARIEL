#ifndef cetlib_plugin_libpath_h
#define cetlib_plugin_libpath_h

namespace cet {
  // Return the name of the environment variable that defines the
  // search path for the dynamic library loader on this operating
  // system.
  constexpr char const* plugin_libpath();
}

inline constexpr char const*
cet::plugin_libpath()
{
  return "CET_PLUGIN_PATH";
}

#endif /* cetlib_plugin_libpath_h */

// Local Variables:
// mode: c++
// End:
