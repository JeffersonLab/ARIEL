#ifndef cetlib_sqlite_ConnectionFactory_h
#define cetlib_sqlite_ConnectionFactory_h
// vim: set sw=2 expandtab :

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
#include "hep_concurrency/RecursiveMutex.h"

#include <map>
#include <memory>
#include <string>
#include <utility>

namespace cet::sqlite {

  class ConnectionFactory {
  public:
    template <typename DatabaseOpenPolicy = detail::DefaultDatabaseOpenPolicy,
              typename... PolicyArgs>
    auto make_connection(std::string const& file_name, PolicyArgs&&...)
      -> Connection*;

  private:
    std::map<std::string, std::weak_ptr<hep::concurrency::RecursiveMutex>>
      databaseLocks_;
    hep::concurrency::RecursiveMutex mutex_{"ConnectionFactory::mutex_"};
  };

  template <typename DatabaseOpenPolicy, typename... PolicyArgs>
  auto
  ConnectionFactory::make_connection(std::string const& filename,
                                     PolicyArgs&&... policyArgs) -> Connection*
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
    auto ret = new Connection{
      filename,
      shared_ptr_to_mutex,
      DatabaseOpenPolicy{std::forward<PolicyArgs>(policyArgs)...}};
    return ret;
  }

} // namespace cet::sqlite

#endif /* cetlib_sqlite_ConnectionFactory_h */

// Local Variables:
// mode: c++
// End:
