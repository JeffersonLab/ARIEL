#ifndef hep_concurrency_WaitingTaskList_h
#define hep_concurrency_WaitingTaskList_h
// vim: set sw=2 expandtab :

#include "hep_concurrency/RecursiveMutex.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-W#pragma-messages"
#include "tbb/task.h"
#pragma GCC diagnostic pop

#include <atomic>
#include <exception>
#include <queue>

namespace hep {
  namespace concurrency {

    class WaitingTaskList {
    private: // Data Members
      hep::concurrency::RecursiveMutex mutex_{"WaitingTaskList::mutex_"};
      std::queue<tbb::task*>* taskQueue_;
      bool waiting_;
      std::exception_ptr* exceptionPtr_;

    private: // Implementation details
      void runAllTasks();

    public: // Special Member Functions
      ~WaitingTaskList();
      WaitingTaskList();
      WaitingTaskList(WaitingTaskList const&) = delete;
      const WaitingTaskList& operator=(WaitingTaskList const&) = delete;

    public: // API
      void add(tbb::task*);
      void doneWaiting(std::exception_ptr);
      void reset();
    };

  } // namespace concurrency
} // namespace hep

#endif /* hep_concurrency_WaitingTaskList_h */

// Local Variables:
// mode: c++
// End:
