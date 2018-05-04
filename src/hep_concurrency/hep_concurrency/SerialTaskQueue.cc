// vim: set sw=2 expandtab :
#include "hep_concurrency/SerialTaskQueue.h"

#include "hep_concurrency/RecursiveMutex.h"
#include "hep_concurrency/tsan.h"
#include "tbb/task.h"

#include <atomic>
#include <queue>

using namespace std;

namespace hep {
  namespace concurrency {

    SerialTaskQueue::~SerialTaskQueue()
    {
      ANNOTATE_THREAD_IGNORE_WRITES_BEGIN;
      delete taskQueue_.load();
      taskQueue_ = nullptr;
      ANNOTATE_THREAD_IGNORE_WRITES_END;
    }

    SerialTaskQueue::SerialTaskQueue()
    {
      taskQueue_ = new queue<tbb::task*>;
      pauseCount_ = 0;
      taskRunning_ = false;
    }

    bool
    SerialTaskQueue::pause()
    {
      auto ret = false;
      {
        RecursiveMutexSentry sentry{mutex_, __func__};
        ret = (++pauseCount_ == 1);
      }
      return ret;
    }

    bool
    SerialTaskQueue::resume()
    {
      bool ret = false;
      {
        RecursiveMutexSentry sentry{mutex_, __func__};
        if (--pauseCount_ == 0) {
          tbb::task* nt = pickNextTask();
          if (nt != nullptr) {
            tbb::task::spawn(*nt);
          }
          ret = true;
        }
      }
      return ret;
    }

    void
    SerialTaskQueue::pushTask(tbb::task* tsk)
    {
      if (tsk != nullptr) {
        taskQueue_.load()->push(tsk);
        tbb::task* nt = pickNextTask();
        if (nt != nullptr) {
          tbb::task::spawn(*nt);
        }
      }
    }

    tbb::task*
    SerialTaskQueue::notify()
    {
      tbb::task* ret = nullptr;
      {
        RecursiveMutexSentry sentry{mutex_, __func__};
        taskRunning_ = false;
        ret = pickNextTask();
      }
      return ret;
    }

    tbb::task*
    SerialTaskQueue::pickNextTask()
    {
      tbb::task* ret = nullptr;
      if ((pauseCount_.load() == 0) && !taskRunning_.load() &&
          !taskQueue_.load()->empty()) {
        ret = taskQueue_.load()->front();
        taskQueue_.load()->pop();
        taskRunning_ = true;
      }
      return ret;
    }

  } // namespace concurrency
} // namespace hep
