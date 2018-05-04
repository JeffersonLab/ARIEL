#ifndef hep_concurrency_WaitingTaskList_h
#define hep_concurrency_WaitingTaskList_h
// vim: set sw=2 expandtab :

#include "hep_concurrency/RecursiveMutex.h"
#include "tbb/task.h"

#include <atomic>
#include <exception>
#include <queue>

namespace hep {
  namespace concurrency {

    class WaitingTaskList {
      // Data Members
    private:
      // Protects taskQueue_;
      hep::concurrency::RecursiveMutex mutex_{"WaitingTaskList::mutex_"};
      std::atomic<std::queue<tbb::task*>*> taskQueue_;
      std::atomic<bool> m_waiting;
      std::atomic<std::exception_ptr*> m_exceptionPtr;
      // Implementation details
    private:
      void runAllTasks();
      // Special Member Functions
    public:
      ~WaitingTaskList();
      WaitingTaskList();
      WaitingTaskList(WaitingTaskList const&) = delete;
      const WaitingTaskList& operator=(WaitingTaskList const&) = delete;
      // API
    public:
      void add(tbb::task*);
      void doneWaiting(std::exception_ptr);
      void reset();
    };

  } // namespace concurrency
} // namespace hep

#endif /* hep_concurrency_WaitingTaskList_h */
