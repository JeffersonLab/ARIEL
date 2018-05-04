#ifndef hep_concurrency_WaitingTaskHolder_h
#define hep_concurrency_WaitingTaskHolder_h
// vim: set sw=2 expandtab :

#include "hep_concurrency/WaitingTask.h"

#include <atomic>

namespace hep {
  namespace concurrency {

    class WaitingTaskHolder {

    private:
      std::atomic<tbb::task*> m_task;

    public:
      ~WaitingTaskHolder();

      WaitingTaskHolder();

      explicit WaitingTaskHolder(tbb::task*);

      WaitingTaskHolder(const WaitingTaskHolder&);

      WaitingTaskHolder(WaitingTaskHolder&&);

      WaitingTaskHolder& operator=(const WaitingTaskHolder&);

      void doneWaiting(std::exception_ptr);
    };

  } // namespace concurrency
} // namespace hep

#endif /* hep_concurrency_WaitingTaskHolder_h */
