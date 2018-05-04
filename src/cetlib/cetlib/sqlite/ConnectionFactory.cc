#include "cetlib/sqlite/ConnectionFactory.h"

using namespace cet::sqlite;

Connection
ConnectionFactory::make(std::string const& filename)
{
  // Implementation a la Herb Sutter's favorite 10-liner
  std::lock_guard<decltype(mutex_)> hold{mutex_};
  auto m = databaseLocks_[filename].lock();
  if (!m) {
    databaseLocks_[filename] = m = std::make_shared<std::mutex>();
  }
  return Connection{filename, m};
}
