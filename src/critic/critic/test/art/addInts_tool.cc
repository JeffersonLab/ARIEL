#include "art/Utilities/ToolMacros.h"

namespace {
  void
  addInts(int& i, int const j)
  {
    i += j;
  }
} // namespace

DEFINE_ART_FUNCTION_TOOL(addInts, "addInts")
