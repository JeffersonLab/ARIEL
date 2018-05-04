#include "cetlib_except/demangle.h"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {
  void
  ensure_eq(std::string const& test, std::string const& ref)
  {
    if (test != ref) {
      std::string msg("ERROR: Expected \"" + ref + "\", got \"" + test +
                      "\"\n");
      throw std::runtime_error(msg);
    }
  }
}

int
main()
{
  ensure_eq(cet::demangle_symbol("t"), "unsigned short");
  ensure_eq(cet::demangle_symbol("_ZTVN3cet9exceptionE"),
            "vtable for cet::exception");
  ensure_eq(
    cet::demangle_message("Unable to resolve symbol: _ZTVN3cet9exceptionE"),
    "Unable to resolve symbol: vtable for cet::exception");
  ensure_eq(cet::demangle_symbol("_ZN3cet8demangleERKSs"),
            "cet::demangle(std::string const&)");
  ensure_eq(cet::demangle_symbol("_ZN13Xrawfileparser13RawFileParserD1Ev"),
            "_ZN13Xrawfileparser13RawFileParserD1Ev");
}
