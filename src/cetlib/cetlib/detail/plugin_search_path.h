#ifndef cetlib_detail_plugin_search_path_h
#define cetlib_detail_plugin_search_path_h
////////////////////////////////////////////////////////////////////////
// plugin_search_path
//
// Return a validated search path, prepending entries to os_libpath() in
// the environment and removing duplicates. If the provided search path
// is based on the default plugin path (plugin_libpath()) and said
// environment variable is not present, we fall back to os_libpath(). In
// all other cases, we return an empty search path if warranted.
////////////////////////////////////////////////////////////////////////
#include "cetlib/search_path.h"

namespace cet {
  namespace detail {
    search_path plugin_search_path();
    search_path plugin_search_path(search_path sp);
  }
}
#endif /* cetlib_detail_plugin_search_path_h */

// Local Variables:
// mode: c++
// End:
