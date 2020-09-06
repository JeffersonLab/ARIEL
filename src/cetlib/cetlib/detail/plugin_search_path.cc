#include "cetlib/detail/plugin_search_path.h"

#include "cetlib/os_libpath.h"
#include "cetlib/plugin_libpath.h"

#include <cstdlib>
#include <set>
#include <string>

namespace {
  // Prepend specified path to os_libpath(), dropping duplicates.
  void
  prepend_path_to_os_libpath(cet::search_path const& sp)
  {
    std::string new_os_libpath;
    std::set<std::string> paths;
    auto const np_sz = sp.size();
    for (auto i = 0ull; i != np_sz; ++i) {
      if (!new_os_libpath.empty()) {
        new_os_libpath.append(":");
      }
      new_os_libpath.append(sp[i]);
      paths.insert(sp[i]);
    }
    cet::search_path oslp{cet::os_libpath(), std::nothrow};
    auto const oslp_sz = oslp.size();
    for (auto i = 0ull; i != oslp_sz; ++i) {
      if (paths.find(oslp[i]) == paths.end()) {
        if (!new_os_libpath.empty()) {
          new_os_libpath.append(":");
        }
        new_os_libpath.append(oslp[i]);
        paths.insert(oslp[i]);
      }
    }
    setenv(cet::os_libpath(), new_os_libpath.c_str(), 1);
  }
}

cet::search_path
cet::detail::plugin_search_path()
{
  return plugin_search_path(search_path{plugin_libpath(), std::nothrow});
}

cet::search_path
cet::detail::plugin_search_path(search_path sp)
{
  if (sp.showenv() == plugin_libpath() &&
      std::getenv(plugin_libpath()) == nullptr) {
    sp = search_path{os_libpath()};
  } else {
    prepend_path_to_os_libpath(sp);
  }
  return sp;
}
