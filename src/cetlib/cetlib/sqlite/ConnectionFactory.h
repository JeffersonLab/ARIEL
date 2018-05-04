#ifndef cetlib_sqlite_ConnectionFactory_h
#define cetlib_sqlite_ConnectionFactory_h

// ====================================================================
// ConnectionFactory
//
// This factory is the only way in which a cet::sqlite::Connection
// object can be created.
//
// For each database filename, there is an associated mutex that is
// shared across Connections referring to the same filename.  Whenever
// an insertion is being performed across multiple Connections, the
// shared mutex guarantees that the database is updated in a
// thread-safe manner.
//
// This facility is necessary because the locking mechanisms that
// SQLite relies on are deficient for NFS systems.
//
// The typical use case is:
//
//   ConnectionFactory factory;
//   auto c1 = factory.make(":memory:");
//   auto c2 = factory.make<MyDBOpenPolicy>("a.db", openPolicyArgs...);
//   auto c3 = factory.make(":memory:");
//
// In the above, c1 and c3 will refer to the same in-memory database.
// To enable thread-safe insertion of data into the DB, consider using
// the Ntuple facility.
// ====================================================================

#include "cetlib/sqlite/Connection.h"
#include "cetlib/sqlite/detail/DefaultDatabaseOpenPolicy.h"

#include <map>
#include <memory>
#include <mutex>
#include <string>

namespace cet {
  namespace sqlite {

    class ConnectionFactory {
    public:
      template <typename DatabaseOpenPolicy = detail::DefaultDatabaseOpenPolicy,
                typename... PolicyArgs>
      auto make(std::string const& file_name, PolicyArgs&&...) -> Connection;

    private:
      std::map<std::string, std::weak_ptr<std::mutex>> databaseLocks_;
      std::mutex mutex_;
    };

  } // namespace sqlite
} // cet

template <typename DatabaseOpenPolicy, typename... PolicyArgs>
auto
cet::sqlite::ConnectionFactory::make(std::string const& filename,
                                     PolicyArgs&&... policyArgs) -> Connection
{
  // Implementation a la Herb Sutter's favorite 10-liner
  std::lock_guard<decltype(mutex_)> hold{mutex_};
  auto m = databaseLocks_[filename].lock();
  if (!m) {
    databaseLocks_[filename] = m = std::make_shared<std::mutex>();
  }
  return Connection{
    filename, m, DatabaseOpenPolicy{std::forward<PolicyArgs>(policyArgs)...}};
}

#endif /* cetlib_sqlite_ConnectionFactory_h */

// Local Variables:
// mode: c++
// End:
