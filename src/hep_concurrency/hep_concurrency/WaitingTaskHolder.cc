// vim: set sw=2 expandtab :
#include "hep_concurrency/WaitingTaskHolder.h"

#include "hep_concurrency/WaitingTask.h"

#include <atomic>
#include <cstdlib>

using namespace std;

namespace hep {
  namespace concurrency {

    WaitingTaskHolder::~WaitingTaskHolder()
    {
      if (m_task.load() != nullptr) {
        doneWaiting(std::exception_ptr{});
      }
    }

    WaitingTaskHolder::WaitingTaskHolder() { m_task = nullptr; }

    WaitingTaskHolder::WaitingTaskHolder(tbb::task* iTask)
    {
      auto wt = dynamic_cast<WaitingTaskExHolder*>(iTask);
      if (wt == nullptr) {
        abort();
      }
      iTask->increment_ref_count();
      m_task = iTask;
    }

    WaitingTaskHolder::WaitingTaskHolder(const WaitingTaskHolder& iHolder)
    {
      m_task = iHolder.m_task.load();
      m_task.load()->increment_ref_count();
    }

    WaitingTaskHolder::WaitingTaskHolder(WaitingTaskHolder&& iOther)
    {
      m_task = iOther.m_task.load();
      iOther.m_task = nullptr;
    }

    WaitingTaskHolder&
    WaitingTaskHolder::operator=(const WaitingTaskHolder& iRHS)
    {
      WaitingTaskHolder tmp(iRHS);

      // std::swap(m_task, tmp.m_task);
      WaitingTaskHolder meCopy(*this);
      m_task = tmp.m_task.load();
      m_task.load()->increment_ref_count();
      tmp.m_task = meCopy.m_task.load();
      tmp.m_task.load()->increment_ref_count();

      return *this;
    }

    void
    WaitingTaskHolder::doneWaiting(std::exception_ptr iExcept)
    {
      if (iExcept) {
        auto wt = dynamic_cast<WaitingTaskExHolder*>(m_task.load());
        if (wt == nullptr) {
          abort();
        }
        wt->dependentTaskFailed(iExcept);
      }
      if (m_task.load()->decrement_ref_count() == 0) {
        tbb::task::spawn(*m_task.load());
      }
      m_task = nullptr;
    }

  } // namespace concurrency
} // namespace hep
