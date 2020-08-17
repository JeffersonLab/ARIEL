#include "cetlib/sqlite/ConnectionFactory.h"
// vim: set sw=2 expandtab :

using namespace cet::sqlite;

Connection*
ConnectionFactory::make_connection(std::string const& filename)
{
  // Implementation a la Herb Sutter's favorite 10-liner
  hep::concurrency::RecursiveMutexSentry sentry{mutex_, __func__};
  // Note: Convert the weak_ptr to a shared_ptr using the member
  // function lock(), this is not an operation on the mutex.
  auto shared_ptr_to_mutex = databaseLocks_[filename].lock();
  if (!shared_ptr_to_mutex) {
    using namespace std::string_literals;
    databaseLocks_[filename] = shared_ptr_to_mutex =
      std::make_shared<hep::concurrency::RecursiveMutex>(
        "ConnectionFactory::databaseLocks_["s + filename + "]"s);
  }
  auto ret = new Connection{filename, shared_ptr_to_mutex};
  return ret;
}
