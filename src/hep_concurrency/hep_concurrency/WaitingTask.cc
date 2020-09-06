#include "hep_concurrency/WaitingTask.h"
// vim: set sw=2 expandtab :

#include "hep_concurrency/tsan.h"
#include "tbb/task.h"

#include <atomic>
#include <exception>
#include <memory>

using namespace std;

namespace hep {
  namespace concurrency {

    WaitingTaskExHolder::~WaitingTaskExHolder()
    {
      ANNOTATE_THREAD_IGNORE_BEGIN;
      delete ptr_.load();
      ptr_ = nullptr;
      ANNOTATE_THREAD_IGNORE_END;
    }

    WaitingTaskExHolder::WaitingTaskExHolder()
    {
      ANNOTATE_THREAD_IGNORE_BEGIN;
      ptr_ = nullptr;
      ANNOTATE_THREAD_IGNORE_END;
    }

    std::exception_ptr const*
    WaitingTaskExHolder::exceptionPtr() const
    {
      auto ret = ptr_.load();
      return ret;
    }

    void
    WaitingTaskExHolder::dependentTaskFailed(std::exception_ptr iPtr)
    {
      if (iPtr && (ptr_.load() == nullptr)) {
        ANNOTATE_THREAD_IGNORE_BEGIN;
        auto temp = new std::exception_ptr(iPtr);
        ANNOTATE_THREAD_IGNORE_END;
        std::exception_ptr* expected = nullptr;
        ANNOTATE_THREAD_IGNORE_BEGIN;
        if (ptr_.compare_exchange_strong(expected, temp)) {
          ANNOTATE_THREAD_IGNORE_END;
          return;
        }
        ANNOTATE_THREAD_IGNORE_END;
        delete temp;
        temp = nullptr;
      }
    }

  } // concurrency
} // hep
