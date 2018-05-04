#ifndef hep_concurrency_SerialTaskQueue_h
#define hep_concurrency_SerialTaskQueue_h
// vim: set sw=2 expandtab :

#include "hep_concurrency/RecursiveMutex.h"
#include "hep_concurrency/tsan.h"
#include "tbb/task.h"

#include <atomic>
#include <queue>

namespace hep {
  namespace concurrency {

    class SerialTaskQueue;

    template <typename T>
    class QueuedTask final : public tbb::task {
    private:
      // Used to call notify() when the functor returns.
      std::atomic<SerialTaskQueue*> queue_;
      // The functor we are going to run later.
      std::atomic<T*> func_;

    private:
      tbb::task* execute() override;

    public:
      virtual ~QueuedTask();
      QueuedTask(SerialTaskQueue*, T const& func);
    };

    class SerialTaskQueue final {
      // Data Members
    private:
      // Protects all data members
      hep::concurrency::RecursiveMutex mutex_{"SerialTaskQueue::mutex_"};
      std::atomic<std::queue<tbb::task*>*> taskQueue_;
      std::atomic<unsigned long> pauseCount_;
      std::atomic<bool> taskRunning_;
      // Implementation Details
    private:
      void pushTask(tbb::task*);
      tbb::task* pickNextTask();
      // Special Member Functions
    public:
      ~SerialTaskQueue();
      SerialTaskQueue();
      SerialTaskQueue(SerialTaskQueue const&) = delete;
      SerialTaskQueue& operator=(SerialTaskQueue const&) = delete;
      // API
    public:
      bool pause();
      bool resume();
      template <typename T>
      void push(const T& func);
      // Used only by QueuedTask<T>::execute().
      tbb::task* notify();
    };

    template <typename T>
    QueuedTask<T>::~QueuedTask()
    {
      ANNOTATE_THREAD_IGNORE_WRITES_BEGIN;
      queue_ = nullptr;
      delete func_.load();
      func_ = nullptr;
      ANNOTATE_THREAD_IGNORE_WRITES_END;
    }

    template <typename T>
    QueuedTask<T>::QueuedTask(SerialTaskQueue* queue, T const& func)
    {
      queue_ = queue;
      func_ = new T(func);
    }

    template <typename T>
    tbb::task*
    QueuedTask<T>::execute()
    {
      try {
        auto p = func_.load();
        (*p)();
      }
      catch (...) {
      }
      auto ret = queue_.load()->notify();
      if (ret != nullptr) {
        tbb::task::spawn(*ret);
      }
      return nullptr;
    }

    template <typename T>
    void
    SerialTaskQueue::push(const T& func)
    {
      {
        hep::concurrency::RecursiveMutexSentry sentry{mutex_, __func__};
        tbb::task* p =
          new (tbb::task::allocate_root()) QueuedTask<T>{this, func};
        pushTask(p);
      }
    }

  } // namespace concurrency
} // namespace hep

#endif /* hep_concurrency_SerialTaskQueue_h */
