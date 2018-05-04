#include "hep_concurrency/WaitingTask.h"
// vim: set sw=2 expandtab :

#include "tbb/task.h"

#include <atomic>
#include <exception>
#include <memory>

namespace hep {
  namespace concurrency {

    // Used for hiding the FunctorWaitingTask ctor vptr adjustment.
    std::atomic<bool> functorWaitingTaskExternalSync_{true};

    // WaitingTask::
    //~WaitingTask()
    //{
    //  delete m_ptr.load();
    //  m_ptr.store(nullptr);
    //  atomic_thread_fence(std::memory_order_seq_cst);
    //}
    //
    // WaitingTask::
    // WaitingTask()
    //{
    //  m_ptr = nullptr;
    //}
    //
    // std::exception_ptr const*
    // WaitingTask::
    // exceptionPtr() const
    //{
    //  auto ret = m_ptr.load();
    //  return ret;
    //}
    //
    // void
    // WaitingTask::
    // dependentTaskFailed(std::exception_ptr iPtr)
    //{
    //  if (iPtr && (m_ptr.load() == nullptr)) {
    //    auto temp = new std::exception_ptr(iPtr);
    //    std::exception_ptr* expected = nullptr;
    //    if (m_ptr.compare_exchange_strong(expected, temp)) {
    //      return;
    //    }
    //    delete temp;
    //    temp = nullptr;
    //  }
    //}

    WaitingTaskExHolder::~WaitingTaskExHolder()
    {
      delete m_ptr.load();
      m_ptr.store(nullptr);
      atomic_thread_fence(std::memory_order_seq_cst);
    }

    WaitingTaskExHolder::WaitingTaskExHolder() { m_ptr = nullptr; }

    std::exception_ptr const*
    WaitingTaskExHolder::exceptionPtr() const
    {
      auto ret = m_ptr.load();
      return ret;
    }

    void
    WaitingTaskExHolder::dependentTaskFailed(std::exception_ptr iPtr)
    {
      if (iPtr && (m_ptr.load() == nullptr)) {
        auto temp = new std::exception_ptr(iPtr);
        std::exception_ptr* expected = nullptr;
        if (m_ptr.compare_exchange_strong(expected, temp)) {
          return;
        }
        delete temp;
        temp = nullptr;
      }
    }

  } // concurrency
} // hep
