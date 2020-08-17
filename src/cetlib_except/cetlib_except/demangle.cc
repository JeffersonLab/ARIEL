// ======================================================================
//
// demangle: Call the cross-vendor API to demangle symbols
//
// ======================================================================

#include "cetlib_except/demangle.h"

#include <cstdlib>
#include <cstring>
#include <cxxabi.h>

std::string
cet::demangle_symbol(char const* symbol)
{
  std::string result;
  char* const unmangled =
    abi::__cxa_demangle(symbol, nullptr, nullptr, nullptr);
  if (unmangled != nullptr) {
    result = unmangled;
    free(unmangled);
  } else {
    result = symbol;
  }
  return result;
}

std::string
cet::demangle_message(std::string message)
{
  std::string result;
  result.reserve(message.size());
  char* tok_state = nullptr;
  char* tokptr = strtok_r(const_cast<char*>(message.c_str()), " ", &tok_state);
  bool first = true;
  for (; tokptr != nullptr; tokptr = strtok_r(nullptr, " ", &tok_state)) {
    if (first) {
      first = false;
    } else {
      result += ' ';
    }
    result += demangle_symbol(tokptr);
  }
  return result;
}
