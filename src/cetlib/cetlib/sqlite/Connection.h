#ifndef cetlib_sqlite_Connection_h
#define cetlib_sqlite_Connection_h
// vim: set sw=2 expandtab :

// ====================================================================
// A Connection is an RAII-motivated class that automatically handles
// opening and closing of the database handle.
//
// It receives a template argument that specifies the database-opening
// policy.  The database closing policy is sqlite3_close(db).
//
// Note that whenever a Connection object is created, database locking
// is automatically disabled to accommodate operations on NFS systems.
// The Connection is created by the ConnectionFactory, which
// constructs a Connection in a thread-safe way.  However, when using
// a connection, the user must ensure that updates to the same
// database via Connection objects is serialized.
// ====================================================================

#include "cetlib/sqlite/Transaction.h"
#include "cetlib/sqlite/detail/bind_parameters.h"
#include "hep_concurrency/RecursiveMutex.h"
#include "hep_concurrency/tsan.h"
#include "sqlite3.h"

#include <memory>
#include <string>
#include <vector>

namespace cet::sqlite {

  class Connection {
    friend class ConnectionFactory;

  public:
    ~Connection() noexcept;
    // It is permitted to create an invalid Connection object
    // through default construction.  However, any connections to an
    // SQLite database must be retrieved through the
    // ConnectionFactory.
    explicit Connection() = default;
    Connection(Connection const&) = delete;
    Connection(Connection&&) = delete;
    Connection& operator=(Connection const&) = delete;
    Connection& operator=(Connection&&) = delete;

  public:
    sqlite3*
    get() const
    {
      return db_;
    }
    operator sqlite3*() { return db_; }
    template <std::size_t NColumns, typename Row>
    int flush_no_throw(std::vector<Row> const& buffer,
                       sqlite3_stmt*& insertStmt);

  private:
    template <typename DatabaseOpenPolicy>
    explicit Connection(std::string const& filename,
                        std::shared_ptr<hep::concurrency::RecursiveMutex>,
                        DatabaseOpenPolicy);

  private:
    sqlite3* db_{nullptr};
    // Shared with other connections to the same database
    std::shared_ptr<hep::concurrency::RecursiveMutex> mutex_{nullptr};
  };

  template <typename DatabaseOpenPolicy>
  Connection::Connection(
    std::string const& filename,
    std::shared_ptr<hep::concurrency::RecursiveMutex> spmutex,
    DatabaseOpenPolicy policy)
    : mutex_{spmutex}
  {
    // No lock necessary since the c'tor is called in a protected
    // environment.
    db_ = policy.open(filename);
  }

  template <std::size_t NColumns, typename Row>
  int
  Connection::flush_no_throw(std::vector<Row> const& buffer,
                             sqlite3_stmt*& insertStmt)
  {
    // Guard against concurrent updates to the same database.
    hep::concurrency::RecursiveMutexSentry sentry{*mutex_, __func__};
    sqlite::Transaction txn{db_};
    for (auto const& r : buffer) {
      sqlite::detail::bind_parameters<Row, NColumns>::bind(insertStmt, r);
      int const rc{sqlite3_step(insertStmt)};
      if (rc != SQLITE_DONE) {
        return rc;
      }
      sqlite3_reset(insertStmt);
    }
    txn.commit();
    return SQLITE_DONE;
  }

} // namespace cet::sqlite

#endif /* cetlib_sqlite_Connection_h */

// Local Variables:
// mode: c++
// End:
