#include "cetlib/sqlite/detail/DefaultDatabaseOpenPolicy.h"
#include "cetlib/sqlite/Exception.h"
#include "cetlib/sqlite/helpers.h"

#include "sqlite3.h"

#include <cassert>

sqlite3*
cet::sqlite::detail::DefaultDatabaseOpenPolicy::open(
  std::string const& filename)
{
  sqlite3* db{nullptr};
  auto const& uri = assembleNoLockURI(filename);
  int const rc{sqlite3_open_v2(uri.c_str(),
                               &db,
                               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
                                 SQLITE_OPEN_URI,
                               nullptr)};
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    throw sqlite::Exception{sqlite::errors::SQLExecutionError}
      << "Failed to open SQLite database\n"
      << "Return code: " << rc;
  }

  assert(db);
  return db;
}
