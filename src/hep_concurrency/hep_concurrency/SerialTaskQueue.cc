// vim: set sw=2 expandtab :
#include "hep_concurrency/SerialTaskQueue.h"

#include "hep_concurrency/RecursiveMutex.h"
#include "hep_concurrency/tsan.h"
#include "tbb/task.h"

#include <atomic>
#include <queue>

using namespace std;

namespace hep::concurrency {

  SerialTaskQueue::~SerialTaskQueue()
  {
    ANNOTATE_THREAD_IGNORE_BEGIN;
    delete taskQueue_;
    taskQueue_ = nullptr;
    ANNOTATE_THREAD_IGNORE_END;
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
    RecursiveMutexSentry sentry{mutex_, __func__};
    auto ret = false;
    {
      ret = (++pauseCount_ == 1);
    }
    return ret;
  }

  bool
  SerialTaskQueue::resume()
  {
    RecursiveMutexSentry sentry{mutex_, __func__};
    bool ret = false;
    {
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
    RecursiveMutexSentry sentry{mutex_, __func__};
    if (tsk != nullptr) {
      taskQueue_->push(tsk);
      tbb::task* nt = pickNextTask();
      if (nt != nullptr) {
        tbb::task::spawn(*nt);
      }
    }
  }

  tbb::task*
  SerialTaskQueue::notify()
  {
    RecursiveMutexSentry sentry{mutex_, __func__};
    tbb::task* ret = nullptr;
    {
      taskRunning_ = false;
      ret = pickNextTask();
    }
    return ret;
  }

  tbb::task*
  SerialTaskQueue::pickNextTask()
  {
    RecursiveMutexSentry sentry{mutex_, __func__};
    tbb::task* ret = nullptr;
    if ((pauseCount_ == 0) && !taskRunning_) {
      if (!taskQueue_->empty()) {
        ret = taskQueue_->front();
        taskQueue_->pop();
        taskRunning_ = true;
      }
    }
    return ret;
  }

} // namespace hep::concurrency
