#ifndef hep_concurrency_WaitingTask_h
#define hep_concurrency_WaitingTask_h
// vim: set sw=2 expandtab :

#include "hep_concurrency/tsan.h"
#include "tbb/task.h"

#include <atomic>
#include <exception>
#include <memory>

namespace hep::concurrency {

  using WaitingTask = tbb::task;

  class WaitingTaskExHolder {
  private: // Data Members
    std::atomic<std::exception_ptr*> ptr_;

  public: // Special Member Functions
    ~WaitingTaskExHolder();
    WaitingTaskExHolder();

  public: // API
    std::exception_ptr const* exceptionPtr() const;
    void dependentTaskFailed(std::exception_ptr);
  };

  template <typename F>
  class FunctorWaitingTask : public tbb::task, public WaitingTaskExHolder {
  private: // Data Members
    std::atomic<F*> func_;

  public: // Special Member Functions
    ~FunctorWaitingTask() override;
    explicit FunctorWaitingTask();
    explicit FunctorWaitingTask(F);

  public: // API required by tbb::task
    task* execute() override;
  };

  template <typename F>
  FunctorWaitingTask<F>::~FunctorWaitingTask()
  {
    ANNOTATE_BENIGN_RACE_SIZED(reinterpret_cast<char*>(&tbb::task::self()) -
                               sizeof(tbb::internal::task_prefix),
                               sizeof(tbb::task) +
                               sizeof(tbb::internal::task_prefix),
                               "tbb::task");
    ANNOTATE_THREAD_IGNORE_BEGIN;
    delete func_.load();
    func_ = nullptr;
    ANNOTATE_THREAD_IGNORE_END;
  }

  template <typename F>
  FunctorWaitingTask<F>::FunctorWaitingTask()
  {
    ANNOTATE_THREAD_IGNORE_BEGIN;
    func_ = nullptr;
    ANNOTATE_THREAD_IGNORE_END;
  }

  template <typename F>
  FunctorWaitingTask<F>::FunctorWaitingTask(F f)
  {
    ANNOTATE_THREAD_IGNORE_BEGIN;
    func_ = new F(f);
    ANNOTATE_THREAD_IGNORE_END;
  }

  template <typename F>
  tbb::task*
  FunctorWaitingTask<F>::execute()
  {
    auto p = exceptionPtr();
    auto theFunc = func_.load();
    theFunc->operator()(p);
    return nullptr;
  }

  template <typename ALLOC, typename F>
  tbb::task*
  make_waiting_task(ALLOC&& iAlloc, F f)
  {
    ANNOTATE_THREAD_IGNORE_BEGIN;
    auto ret = new (iAlloc) FunctorWaitingTask<F>(f);
    ANNOTATE_THREAD_IGNORE_END;
    return ret;
  }

} // hep::concurrency

#endif /* hep_concurrency_WaitingTask_h */

// Local Variables:
// mode: c++
// End:
