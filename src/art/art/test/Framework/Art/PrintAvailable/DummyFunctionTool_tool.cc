#include "art/Utilities/ToolMacros.h"

namespace art {
  namespace test {
    int
    num(int)
    {
      return 1;
    }
  } // namespace test
} // namespace art

DEFINE_ART_FUNCTION_TOOL(art::test::num, "num")
