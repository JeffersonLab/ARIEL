#include "canvas/Utilities/DebugMacros.h"
// vim: set sw=2 expandtab :

#include <cstdlib>

namespace art {

  DebugValue::DebugValue()
  {
    cvalue_ = getenv("PROC_DEBUG");
    value_ = (cvalue_.load() == nullptr) ? 0 : atoi(cvalue_.load());
  }

  DebugTasksValue::DebugTasksValue()
  {
    cvalue_ = getenv("ART_DEBUG_TASKS");
    value_ = (cvalue_.load() == nullptr) ? 0 : atoi(cvalue_.load());
  }

  DebugValue debugit;
  DebugTasksValue debugTasks;

} // namespace art
