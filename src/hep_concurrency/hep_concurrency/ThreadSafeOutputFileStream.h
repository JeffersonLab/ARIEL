#ifndef hep_concurrency_ThreadSafeOutputFileStream_h
#define hep_concurrency_ThreadSafeOutputFileStream_h
// vim: set sw=2 expandtab :

#include "hep_concurrency/RecursiveMutex.h"

#include <fstream>
#include <string>

namespace hep {
  namespace concurrency {

    class ThreadSafeOutputFileStream {
      // Data Members.
    private:
      mutable hep::concurrency::RecursiveMutex mutex_{
        "ThreadSafeOutputFileStream::mutex_"};
      std::ofstream file_{};
      // Special Member Functions.
    public:
      ~ThreadSafeOutputFileStream();
      ThreadSafeOutputFileStream(std::string const& filename);
      // API
    public:
      explicit operator bool() const;
      void write(std::string&& msg);
    };

  } // namespace concurrency
} // namespace hep

#endif /* hep_concurrency_ThreadSafeOutputFileStream_h */

// Local Variables:
// mode: c++
// End:
