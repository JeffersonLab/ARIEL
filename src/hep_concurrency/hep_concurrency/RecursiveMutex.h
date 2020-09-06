#ifndef hep_concurrency_RecursiveMutex_h
#define hep_concurrency_RecursiveMutex_h
// vim: set sw=2 expandtab :

#include "hep_concurrency/tsan.h"

#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

namespace hep {
  namespace concurrency {

    struct RecursiveMutexCleaner;

    class RecursiveMutex {
    public:
      // Special Member Functions
      RecursiveMutex(std::string const& name = "");
      RecursiveMutex(RecursiveMutex const&) = delete;
      RecursiveMutex(RecursiveMutex&&) = delete;
      RecursiveMutex& operator=(RecursiveMutex const&) = delete;
      RecursiveMutex& operator=(RecursiveMutex&&) = delete;

      // Member Functions -- API
      void lock(std::string const& opName = "");
      void unlock(std::string const& opName = "");

    private:
      friend struct RecursiveMutexCleaner;
      static std::mutex* heldMutex_;
      using held_map_t =
        std::map<std::thread::id, std::vector<RecursiveMutex*>>;
      static held_map_t* held_;

      // Static Member Functions
      static void startup();
      static void shutdown();
      static bool threadHoldsMutex(long const tid, unsigned long addr);

      // Used to make sure we only try to lock when mutex_ is unowned
      // or we are the owner.
      std::condition_variable cv_;

      // The mutex for cv_.
      std::mutex cvMutex_;

      // The mutex the user is trying to lock.
      std::mutex mutex_;

      // The tid of the thread that locked mutex_.
      std::thread::id owner_;

      // Used to allow recursive locking of mutex_, but we only
      // lock/unlock it once.
      unsigned lockCount_;

      // For tracing.
      std::string name_;
    };

    class RecursiveMutexSentry {
    public:
      ~RecursiveMutexSentry();
      RecursiveMutexSentry(RecursiveMutex&, std::string const& name = "");
      RecursiveMutexSentry(RecursiveMutexSentry const&) = delete;
      RecursiveMutexSentry(RecursiveMutexSentry&&) = delete;
      RecursiveMutexSentry& operator=(RecursiveMutexSentry const&) = delete;
      RecursiveMutexSentry& operator=(RecursiveMutexSentry&&) = delete;

    private:
      RecursiveMutex* mutex_;
      std::string* name_;
    };

  } // namespace concurrency
} // namespace hep

#endif /* hep_concurrency_RecursiveMutex_h */

// Local Variables:
// mode: c++
// End:
