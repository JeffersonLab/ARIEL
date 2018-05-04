#ifndef cetlib_ProvideFilePathMacro_h
#define cetlib_ProvideFilePathMacro_h

// =====================================================================
//
// HelperMacros
//
// =====================================================================

#include "boost/filesystem.hpp"
#include "cetlib/compiler_macros.h"

#include <string>

namespace bfs = boost::filesystem;

#define CET_PROVIDE_FILE_PATH()                                                \
  EXTERN_C_FUNC_DECLARE_START                                                  \
  std::string get_source_location()                                            \
  {                                                                            \
    bfs::path const p{__FILE__};                                               \
    return bfs::complete(p).native();                                          \
  }                                                                            \
  EXTERN_C_FUNC_DECLARE_END

#endif /* cetlib_ProvideFilePathMacro_h */

// Local Variables:
// mode: c++
// End:
