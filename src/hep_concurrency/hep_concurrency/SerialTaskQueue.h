#ifndef hep_concurrency_SerialTaskQueue_h
#define hep_concurrency_SerialTaskQueue_h
// vim: set sw=2 expandtab :

#include "hep_concurrency/RecursiveMutex.h"
#include "hep_concurrency/tsan.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-W#pragma-messages"
#include "tbb/task.h"
#pragma GCC diagnostic pop

#include <atomic>
#include <queue>

namespace hep::concurrency {

  class SerialTaskQueue;

  template <typename T>
  class QueuedTask final : public tbb::task {
  private: // DATA MEMBERS
    // Used to call notify() when the functor returns.
    std::atomic<SerialTaskQueue*> queue_;
    // The functor we are going to run later.
    std::atomic<T*> func_;

  public: // Special Member Functions
    virtual ~QueuedTask();
    QueuedTask(SerialTaskQueue*, T const& func);

  private: // API -- required by tbb::task
    tbb::task* execute() override;
  };

  class SerialTaskQueue final {
  private: // Data Members
    hep::concurrency::RecursiveMutex mutex_{"SerialTaskQueue::mutex_"};
    std::queue<tbb::task*>* taskQueue_;
    unsigned long pauseCount_;
    bool taskRunning_;

  private: // Implementation Details
    void pushTask(tbb::task*);
    tbb::task* pickNextTask();

  public: // Special Member Functions
    ~SerialTaskQueue();
    SerialTaskQueue();
    SerialTaskQueue(SerialTaskQueue const&) = delete;
    SerialTaskQueue& operator=(SerialTaskQueue const&) = delete;

  public: // API
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
    ANNOTATE_BENIGN_RACE_SIZED(reinterpret_cast<char*>(&tbb::task::self()) -
                               sizeof(tbb::internal::task_prefix),
                               sizeof(tbb::task) +
                               sizeof(tbb::internal::task_prefix),
                               "tbb::task");
    ANNOTATE_THREAD_IGNORE_BEGIN;
    queue_ = nullptr;
    delete func_.load();
    func_ = nullptr;
    ANNOTATE_THREAD_IGNORE_END;
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
    hep::concurrency::RecursiveMutexSentry sentry{mutex_, __func__};
    ANNOTATE_THREAD_IGNORE_BEGIN;
    tbb::task* p = new (tbb::task::allocate_root()) QueuedTask<T>{this, func};
    ANNOTATE_THREAD_IGNORE_END;
    pushTask(p);
  }

} // namespace hep::concurrency

#endif /* hep_concurrency_SerialTaskQueue_h */

// Local Variables:
// mode: c++
// End:
