// vim: set sw=2 expandtab :
#include "hep_concurrency/WaitingTaskList.h"

#include "hep_concurrency/RecursiveMutex.h"
#include "hep_concurrency/WaitingTask.h"
#include "hep_concurrency/hardware_pause.h"
#include "hep_concurrency/tsan.h"
#include "tbb/task.h"

#include <atomic>
#include <cassert>
#include <exception>
#include <queue>

using namespace std;

namespace hep {
  namespace concurrency {

    WaitingTaskList::~WaitingTaskList()
    {
      ANNOTATE_THREAD_IGNORE_WRITES_BEGIN;
      delete taskQueue_;
      taskQueue_ = nullptr;
      delete m_exceptionPtr.load();
      ANNOTATE_THREAD_IGNORE_WRITES_END;
    }

    WaitingTaskList::WaitingTaskList()
    {
      taskQueue_ = new std::queue<tbb::task*>;
      m_waiting = true;
      m_exceptionPtr = new exception_ptr;
    }

    void
    WaitingTaskList::reset()
    {
      RecursiveMutexSentry sentry{mutex_, __func__};
      // We should not be reset if there are
      // any tasks waiting to be run.
      assert(taskQueue_.load()->empty());
      m_waiting = true;
      delete m_exceptionPtr.load();
      m_exceptionPtr = new exception_ptr;
    }

    void
    WaitingTaskList::add(tbb::task* tsk)
    {
      if (tsk == nullptr) {
        assert(tsk != nullptr);
        return;
      }
      RecursiveMutexSentry sentry{mutex_, __func__};
      tsk->increment_ref_count();
      if (!m_waiting.load()) {
        // We are not in waiting mode, we should
        // run the task immediately.
        if (*m_exceptionPtr.load()) {
          // The doneWaiting call that set us running
          // propagated an exception to us.
          auto wt = dynamic_cast<WaitingTaskExHolder*>(tsk);
          if (wt == nullptr) {
            abort();
          }
          wt->dependentTaskFailed(*m_exceptionPtr.load());
        }
        if (tsk->decrement_ref_count() == 0) {
          tbb::task::spawn(*tsk);
        }
        // Note: If we did not spawn the task above, then we assume
        // that the same task is on multiple waiting task lists,
        // and whichever list decrements the refcount to zero will
        // be the one that actually spawns it. We actually do this
        // in the case of the pathsDoneTask, which is in the waiting
        // list of each path, but is spawned only once, by the the
        // last path to finish.
        return;
      }
      taskQueue_.load()->push(tsk);
    }

    void
    WaitingTaskList::doneWaiting(exception_ptr exc)
    {
      // Run all the tasks and propagate an
      // exception to them.
      RecursiveMutexSentry sentry{mutex_, __func__};
      m_waiting = false;
      delete m_exceptionPtr;
      m_exceptionPtr = new exception_ptr{exc};
      runAllTasks();
    }

    void
    WaitingTaskList::runAllTasks()
    {
      while (!taskQueue_.load()->empty()) {
        auto tsk = taskQueue_.load()->front();
        taskQueue_.load()->pop();
        if (*m_exceptionPtr.load()) {
          auto wt = dynamic_cast<WaitingTaskExHolder*>(tsk);
          if (wt == nullptr) {
            abort();
          }
          wt->dependentTaskFailed(*m_exceptionPtr.load());
        }
        if (tsk->decrement_ref_count() == 0) {
          tbb::task::spawn(*tsk);
        }
        // Note: If we did not spawn the task above, then we assume
        // that the same task is on multiple waiting task lists,
        // and whichever list decrements the refcount to zero will
        // be the one that actually spawns it. We actually do this
        // in the case of the pathsDoneTask, which is in the waiting
        // list of each path, but is spawned only once, by the the
        // last path to finish.
      }
    }

  } // namespace concurrency
} // namespace hep
