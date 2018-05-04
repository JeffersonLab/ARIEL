// vim: set sw=2 expandtab :
#include "hep_concurrency/SerialTaskQueueChain.h"

#include "hep_concurrency/RecursiveMutex.h"
#include "hep_concurrency/SerialTaskQueue.h"

#include <cassert>
#include <memory>
#include <utility>
#include <vector>

using namespace std;

namespace hep {
  namespace concurrency {

    SerialTaskQueueChain::~SerialTaskQueueChain() {}

    SerialTaskQueueChain::SerialTaskQueueChain() {}

    SerialTaskQueueChain::SerialTaskQueueChain(
      vector<shared_ptr<SerialTaskQueue>> queues)
    {
      queues_ = move(queues);
    }

    SerialTaskQueueChain::SerialTaskQueueChain(SerialTaskQueueChain&& rhs)
    {
      queues_ = move(rhs.queues_);
    }

    SerialTaskQueueChain&
    SerialTaskQueueChain::operator=(SerialTaskQueueChain&& rhs)
    {
      RecursiveMutexSentry sentry{mutex_, __func__};
      queues_ = move(rhs.queues_);
      return *this;
    }

  } // namespace concurrency
} // namespace hep
