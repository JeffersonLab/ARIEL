#ifndef hep_concurrency_SerialTaskQueueChain_h
#define hep_concurrency_SerialTaskQueueChain_h
// vim: set sw=2 expandtab :

#include "hep_concurrency/RecursiveMutex.h"
#include "hep_concurrency/SerialTaskQueue.h"

#include <atomic>
#include <cassert>
#include <exception>
#include <memory>
#include <vector>

namespace hep {
  namespace concurrency {

    class SerialTaskQueueChain {
      // Data Members
    private:
      // Protects all data members
      hep::concurrency::RecursiveMutex mutex_{"SerialTaskQueueChain::mutex_"};
      // The queues
      std::vector<std::shared_ptr<SerialTaskQueue>> queues_;
      // Implementation Details
    private:
      template <typename T>
      void passDown(unsigned int, const T&);
      template <typename T>
      void runFunc(const T&);
      // Special Member Functions
    public:
      ~SerialTaskQueueChain();
      SerialTaskQueueChain();
      explicit SerialTaskQueueChain(
        std::vector<std::shared_ptr<SerialTaskQueue>>);
      SerialTaskQueueChain(SerialTaskQueueChain const&) = delete;
      SerialTaskQueueChain(SerialTaskQueueChain&&);
      SerialTaskQueueChain& operator=(SerialTaskQueueChain const&) = delete;
      SerialTaskQueueChain& operator=(SerialTaskQueueChain&&);
      // API
    public:
      template <typename T>
      void push(T const&);
    };

    template <typename T>
    void
    SerialTaskQueueChain::push(T const& func)
    {
      hep::concurrency::RecursiveMutexSentry sentry{mutex_, __func__};
      assert(queues_.size() > 0);
      if (queues_.size() == 1) {
        // The mutable is *not* optional, the c++ standard requires this!
        queues_[0]->push([this, func]() mutable { runFunc(func); });
      } else {
        // The mutable is *not* optional, the c++ standard requires this!
        queues_[0]->push([this, func]() mutable { passDown(1, func); });
      }
    }

    template <typename T>
    void
    SerialTaskQueueChain::passDown(unsigned int idx, T const& func)
    {
      hep::concurrency::RecursiveMutexSentry sentry{mutex_, __func__};
      queues_[idx - 1]->pause();
      if ((idx + 1) == queues_.size()) {
        // The mutable is *not* optional, the c++ standard requires this!
        queues_[idx]->push([this, func]() mutable { runFunc(func); });
      } else {
        auto nxt = idx + 1;
        // The mutable is *not* optional, the c++ standard requires this!
        queues_[idx]->push(
          [this, nxt, func]() mutable { passDown(nxt, func); });
      }
    }

    template <typename T>
    void
    SerialTaskQueueChain::runFunc(const T& func)
    {
      try {
        func();
      }
      catch (...) {
        hep::concurrency::RecursiveMutexSentry sentry{mutex_, __func__};
        for (auto I = queues_.rbegin() + 1; I != queues_.rend(); ++I) {
          (*I)->resume();
        }
        throw;
      }
      hep::concurrency::RecursiveMutexSentry sentry{mutex_, __func__};
      for (auto I = queues_.rbegin() + 1; I != queues_.rend(); ++I) {
        (*I)->resume();
      }
    }

  } // namespace concurrency
} // namespace hep

#endif /* hep_concurrency_SerialTaskQueueChain_h */

// Local Variables:
// mode: c++
// End:
