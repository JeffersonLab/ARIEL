#ifndef hep_concurrency_WaitingTask_h
#define hep_concurrency_WaitingTask_h
// vim: set sw=2 expandtab :

#include "tbb/task.h"

#include <atomic>
#include <exception>
#include <memory>

namespace hep {
  namespace concurrency {

    using WaitingTask = tbb::task;

    // Used for hiding the FunctorWaitingTask ctor vptr adjustment.
    extern std::atomic<bool> functorWaitingTaskExternalSync_;

    // Used for hiding the FunctorWaitingTask ctor vptr adjustment.
    inline void
    functorWaitingTaskStartSync()
    {
      // functorWaitingTaskExternalSync_.store(true, std::memory_order_release);
    }

    // Used for hiding the FunctorWaitingTask ctor vptr adjustment.
    inline void
    functorWaitingTaskFinishSync()
    {
      // functorWaitingTaskExternalSync_.load(std::memory_order_acquire);
    }

    class WaitingTaskExHolder {

      // Data Members
    private:
      std::atomic<std::exception_ptr*> m_ptr;

      // Special Member Functions
    public:
      ~WaitingTaskExHolder();

      WaitingTaskExHolder();

      std::exception_ptr const* exceptionPtr() const;

      void dependentTaskFailed(std::exception_ptr);
    };

    template <typename F>
    class FunctorWaitingTask : public tbb::task, public WaitingTaskExHolder {

      // Data Members
    private:
      std::atomic<F*> func_;

      // Special Member Functions
    public:
      ~FunctorWaitingTask() override;

      explicit FunctorWaitingTask();

      explicit FunctorWaitingTask(F);

      task* execute() override;
    };

    // template <typename F>
    // class FunctorWaitingTask : public WaitingTask {
    //
    // private:
    //
    //  std::atomic<F*>
    //  func_;
    //
    // public:
    //
    //  ~FunctorWaitingTask();
    //
    //  explicit
    //  FunctorWaitingTask(F);
    //
    //  task*
    //  execute() override;
    //
    //};

    template <typename F>
    FunctorWaitingTask<F>::~FunctorWaitingTask()
    {
      delete func_.load();
      func_.store(nullptr);
    }

    template <typename F>
    FunctorWaitingTask<F>::FunctorWaitingTask()
    {
      func_ = nullptr;
    }

    template <typename F>
    FunctorWaitingTask<F>::FunctorWaitingTask(F f)
    {
      func_ = new F(f);
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
      auto ret = new (iAlloc) FunctorWaitingTask<F>(f);
      return ret;
    }

  } // concurrency
} // hep

#endif /* hep_concurrency_WaitingTask_h */
