#ifndef cetlib_os_libpath_h
#define cetlib_os_libpath_h

namespace cet {
  // Return the name of the environment variable that defines the
  // search path for the dynamic library loader on this operating
  // system.
  constexpr char const* os_libpath();
}

inline constexpr char const*
cet::os_libpath()
{
#if __linux__
  return "LD_LIBRARY_PATH";
#elif __APPLE__ && __MACH__
  return "DYLD_LIBRARY_PATH";
#else
#error Unknown operating system
#endif
}

#endif /* cetlib_os_libpath_h */

// Local Variables:
// mode: c++
// End:
