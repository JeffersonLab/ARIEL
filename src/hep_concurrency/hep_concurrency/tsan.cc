#include "hep_concurrency/tsan.h"

#include <x86intrin.h>

using namespace std;

namespace hep {
  namespace concurrency {

    int intentionalDataRace_{0};

    thread_local int ignoreBalance_{0};

    thread::id
    getThreadID()
    {
      auto tid = this_thread::get_id();
      return tid;
    }

  } // namespace concurrency
} // namespace hep
