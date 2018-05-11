// ======================================================================
//
// getenv_os_libpath: get os_libpath() environment variable plus
// additional path data from MODULE_LOAD_PATH.
//
// ======================================================================

#include "cetlib/os_libpath.h"
#include "cetlib/getenv.h"

#include "cetlib_except/exception.h"

// ======================================================================

using std::string;

namespace {
  string const os_libpath_fallback{"CET_MODULE_PATH"};
}

string
cet::getenv_os_libpath()
{
  string libpath, mlpath;
  bool libpath_found{true};
  try {
    libpath = cet::getenv(cet::os_libpath());
  }
  catch (cet::exception const& e) {
    // If undefined, try fallback
    libpath.clear();
    libpath_found = false;
  }
  // Append MODULE_LOAD_PATH, if defined, or use as fallback
  try {
    mlpath = cet::getenv(os_libpath_fallback);
  }
  catch (cet::exception const& e) {
    if (!libpath_found)
      // Fallback failed, bail out
      throw cet::exception("getenv")
	<< "Neither " << cet::os_libpath() << " nor "
	<< os_libpath_fallback << " defined in environment";
    mlpath.clear();
  }
  if (!mlpath.empty()) {
    if (!libpath.empty() && libpath.back() != ':')
      libpath += ':';
    libpath += mlpath;
  }
  return libpath;
}

// ======================================================================
