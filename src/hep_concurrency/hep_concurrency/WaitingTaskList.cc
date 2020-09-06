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
      ANNOTATE_THREAD_IGNORE_BEGIN;
      delete taskQueue_;
      ;
      taskQueue_ = nullptr;
      delete exceptionPtr_;
      exceptionPtr_ = nullptr;
      ANNOTATE_THREAD_IGNORE_END;
    }

    WaitingTaskList::WaitingTaskList()
    {
      taskQueue_ = new std::queue<tbb::task*>;
      waiting_ = true;
      exceptionPtr_ = new exception_ptr;
    }

    void
    WaitingTaskList::reset()
    {
      RecursiveMutexSentry sentry{mutex_, __func__};
      // We should not be reset if there are
      // any tasks waiting to be run.
      assert(taskQueue_->empty());
      waiting_ = true;
      delete exceptionPtr_;
      exceptionPtr_ = new exception_ptr;
    }

    void
    WaitingTaskList::add(tbb::task* tsk)
    {
      RecursiveMutexSentry sentry{mutex_, __func__};
      if (tsk == nullptr) {
        assert(tsk != nullptr);
        return;
      }
      tsk->increment_ref_count();
      if (!waiting_) {
        // We are not in waiting mode, we should
        // run the task immediately.
        if (*exceptionPtr_) {
          // The doneWaiting call that set us running
          // propagated an exception to us.
          auto wt = dynamic_cast<WaitingTaskExHolder*>(tsk);
          if (wt == nullptr) {
            abort();
          }
          wt->dependentTaskFailed(*exceptionPtr_);
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
      taskQueue_->push(tsk);
    }

    void
    WaitingTaskList::doneWaiting(exception_ptr exc)
    {
      RecursiveMutexSentry sentry{mutex_, __func__};
      // Run all the tasks and propagate an
      // exception to them.
      waiting_ = false;
      delete exceptionPtr_;
      exceptionPtr_ = new exception_ptr{exc};
      runAllTasks();
    }

    void
    WaitingTaskList::runAllTasks()
    {
      RecursiveMutexSentry sentry{mutex_, __func__};
      auto isEmpty = taskQueue_->empty();
      while (!isEmpty) {
        auto tsk = taskQueue_->front();
        taskQueue_->pop();
        if (*exceptionPtr_) {
          auto wt = dynamic_cast<WaitingTaskExHolder*>(tsk);
          if (wt == nullptr) {
            abort();
          }
          wt->dependentTaskFailed(*exceptionPtr_);
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
        isEmpty = taskQueue_->empty();
      }
    }

  } // namespace concurrency
} // namespace hep
