#include "hep_concurrency/ThreadSafeOutputFileStream.h"
// vim: set sw=2 expandtab :

#include "hep_concurrency/RecursiveMutex.h"

#include <fstream>
#include <sstream>
#include <string>
#include <utility>

using namespace std;

namespace hep {
  namespace concurrency {

    ThreadSafeOutputFileStream::~ThreadSafeOutputFileStream() { file_.close(); }

    ThreadSafeOutputFileStream::ThreadSafeOutputFileStream(
      string const& filename)
      : file_{filename}
    {}

    ThreadSafeOutputFileStream::operator bool() const
    {
      RecursiveMutexSentry sentry{mutex_, __func__};
      return static_cast<bool>(file_);
    }

    void
    ThreadSafeOutputFileStream::write(string&& msg)
    {
      RecursiveMutexSentry sentry{mutex_, __func__};
      file_ << forward<string>(msg);
    }

  } // namespace concurrency
} // namespace hep
