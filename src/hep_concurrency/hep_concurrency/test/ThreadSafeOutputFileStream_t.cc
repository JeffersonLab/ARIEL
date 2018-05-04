#include "hep_concurrency/ThreadSafeOutputFileStream.h"
// vim: set sw=2 expandtab :

#include <atomic>
#include <sstream>
#include <thread>
#include <vector>

using namespace std;
using namespace hep::concurrency;

namespace {

  unsigned const nThreads{3};
  atomic<int> nWaitingForThreads{0};

  void
  logToFile(unsigned const thread_index, ThreadSafeOutputFileStream& f)
  {
    --nWaitingForThreads;
    while (nWaitingForThreads != 0)
      ;
    // Spin until all threads have been launched.
    ostringstream oss;
    oss << "Thread index: " << thread_index << " Entry: ";
    auto const& prefix = oss.str();
    for (int i{}; i < 4; ++i) {
      f.write(prefix + to_string(i) + "\n");
    }
  }

} // unnamed namespace

int
main()
{
  ThreadSafeOutputFileStream f{"thread_safe_ofstream_test.txt"};
  vector<thread> threads;
  nWaitingForThreads = nThreads;
  for (unsigned i{}; i < nThreads; ++i) {
    threads.emplace_back(logToFile, i, ref(f));
  }
  for (auto& thread : threads) {
    thread.join();
  }
}
