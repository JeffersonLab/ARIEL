#include "fhiclcpp/ParameterSetRegistry.h"

#include "cetlib/sqlite/Transaction.h"
#include "cetlib/sqlite/create_table.h"
#include "cetlib/sqlite/exec.h"
#include "cetlib/sqlite/select.h"
#include "fhiclcpp/ParameterSetID.h"
#include "fhiclcpp/exception.h"
#include "fhiclcpp/make_ParameterSet.h"

using fhicl::detail::throwOnSQLiteFailure;

namespace {
  sqlite3*
  openPrimaryDB()
  {
    sqlite3* result = nullptr;
    sqlite3_open(":memory:", &result);
    fhicl::detail::throwOnSQLiteFailure(result);
    using namespace cet::sqlite;
    Transaction txn{result};
    create_table(result,
                 "ParameterSets",
                 column<std::string, primary_key>{"ID"},
                 column<std::string>{"PSetBlob"});
    txn.commit();
    return result;
  }
}

void
fhicl::detail::throwOnSQLiteFailure(sqlite3* db, char* msg)
{
  std::string const msgString{msg ? msg : ""};
  sqlite3_free(msg);
  if (db == nullptr) {
    throw fhicl::exception(fhicl::error::cant_open_db) << "Can't open DB.";
  }
  auto errcode = sqlite3_errcode(db);
  if (errcode != SQLITE_OK) {
    // Caller's responsibility to make sure this really is an error
    // and not (say) SQLITE_ROW or SQLITE_DONE,
    throw exception(error::sql_error, "SQLite error:")
      << sqlite3_errstr(errcode) << " (" << errcode
      << "): " << sqlite3_errmsg(db)
      << (msgString.empty() ? "" : (std::string(". ") + msgString));
  }
}

fhicl::ParameterSetRegistry::~ParameterSetRegistry()
{
  sqlite3_finalize(stmt_);
  try {
    throwOnSQLiteFailure(primaryDB_);
  }
  catch (fhicl::exception const& e) {
    std::cerr << e.what() << '\n';
  }
  catch (...) {
  }
  int retcode;
  do {
    retcode = sqlite3_close(primaryDB_);
  } while (retcode == SQLITE_BUSY);
}

void
fhicl::ParameterSetRegistry::importFrom(sqlite3* db)
{
  assert(db);
  std::lock_guard<decltype(mutex_)> lock{mutex_};

  // This does *not* cause anything new to be imported into the
  // registry itself, just its backing DB.
  sqlite3_stmt* oStmt = nullptr;
  sqlite3* primaryDB = instance_().primaryDB_;

  // Index constraint on ID will prevent duplicates via INSERT OR IGNORE.
  sqlite3_prepare_v2(
    primaryDB,
    "INSERT OR IGNORE INTO ParameterSets(ID, PSetBlob) VALUES(?, ?);",
    -1,
    &oStmt,
    nullptr);
  throwOnSQLiteFailure(primaryDB);

  using namespace cet::sqlite;
  query_result<std::string, std::string> inputPSes;
  inputPSes << select("*").from(db, "ParameterSets");

  for (auto const& row : inputPSes) {
    std::string idString;
    std::string psBlob;
    std::tie(idString, psBlob) = row;

    sqlite3_bind_text(
      oStmt, 1, idString.c_str(), idString.size() + 1, SQLITE_STATIC);
    throwOnSQLiteFailure(primaryDB);
    sqlite3_bind_text(
      oStmt, 2, psBlob.c_str(), psBlob.size() + 1, SQLITE_STATIC);
    throwOnSQLiteFailure(primaryDB);
    switch (sqlite3_step(oStmt)) {
      case SQLITE_DONE:
        break; // OK
      default:
        throwOnSQLiteFailure(primaryDB);
    }
    sqlite3_reset(oStmt);
    throwOnSQLiteFailure(primaryDB);
  }
  sqlite3_finalize(oStmt);
  throwOnSQLiteFailure(primaryDB);
}

void
fhicl::ParameterSetRegistry::exportTo(sqlite3* db)
{
  assert(db);
  std::lock_guard<decltype(mutex_)> lock{mutex_};

  cet::sqlite::Transaction txn{db};
  cet::sqlite::exec(db,
                    "DROP TABLE IF EXISTS ParameterSets;"
                    "CREATE TABLE ParameterSets(ID PRIMARY KEY, PSetBlob);");
  txn.commit();

  sqlite3_stmt* oStmt = nullptr;
  sqlite3_prepare_v2(
    db,
    "INSERT OR IGNORE INTO ParameterSets(ID, PSetBlob) VALUES(?, ?);",
    -1,
    &oStmt,
    nullptr);
  throwOnSQLiteFailure(db);
  for (auto const& p : instance_().registry_) {
    std::string id(p.first.to_string());
    std::string psBlob(p.second.to_compact_string());
    sqlite3_bind_text(oStmt, 1, id.c_str(), id.size() + 1, SQLITE_STATIC);
    throwOnSQLiteFailure(db);
    sqlite3_bind_text(
      oStmt, 2, psBlob.c_str(), psBlob.size() + 1, SQLITE_STATIC);
    throwOnSQLiteFailure(db);
    switch (sqlite3_step(oStmt)) {
      case SQLITE_DONE:
        sqlite3_reset(oStmt);
        throwOnSQLiteFailure(db);
        break; // OK
      default:
        throwOnSQLiteFailure(db);
    }
  }

  sqlite3* const primaryDB{instance_().primaryDB_};
  using namespace cet::sqlite;
  query_result<std::string, std::string> regPSes;
  regPSes << select("*").from(primaryDB, "ParameterSets");

  for (auto const& row : regPSes) {
    std::string idString;
    std::string psBlob;
    std::tie(idString, psBlob) = row;
    sqlite3_bind_text(
      oStmt, 1, idString.c_str(), idString.size() + 1, SQLITE_STATIC);
    throwOnSQLiteFailure(db);
    sqlite3_bind_text(
      oStmt, 2, psBlob.c_str(), psBlob.size() + 1, SQLITE_STATIC);
    throwOnSQLiteFailure(db);
    switch (sqlite3_step(oStmt)) {
      case SQLITE_DONE:
        sqlite3_reset(oStmt);
        throwOnSQLiteFailure(db);
        break; // OK
      default:
        throwOnSQLiteFailure(db);
    }
  }
  sqlite3_finalize(oStmt);
  throwOnSQLiteFailure(db);
}

void
fhicl::ParameterSetRegistry::stageIn()
{
  std::lock_guard<decltype(mutex_)> lock{mutex_};

  sqlite3* primaryDB = instance_().primaryDB_;
  auto& registry = instance_().registry_;
  using namespace cet::sqlite;
  query_result<std::string, std::string> entriesToStageIn;
  entriesToStageIn << select("*").from(primaryDB, "ParameterSets");

  cet::transform_all(entriesToStageIn,
                     std::inserter(registry, std::begin(registry)),
                     [](auto const& row) {
                       std::string idString;
                       std::string psBlob;
                       std::tie(idString, psBlob) = row;
                       ParameterSet pset;
                       fhicl::make_ParameterSet(psBlob, pset);
                       return std::make_pair(ParameterSetID{idString}, pset);
                     });
}

fhicl::ParameterSetRegistry::ParameterSetRegistry()
  : primaryDB_{openPrimaryDB()}
{}

std::recursive_mutex fhicl::ParameterSetRegistry::mutex_{};

auto
fhicl::ParameterSetRegistry::find_(ParameterSetID const& id) -> const_iterator
{
  // No lock here -- it was already acquired by get(...).
  auto it = registry_.find(id);
  if (it == registry_.cend()) {
    // Look in primary DB for this ID and its contained IDs.
    if (stmt_ == nullptr) {
      sqlite3_prepare_v2(primaryDB_,
                         "SELECT PSetBlob FROM ParameterSets WHERE ID = ?;",
                         -1,
                         &stmt_,
                         nullptr);
      throwOnSQLiteFailure(primaryDB_);
    }
    auto idString = id.to_string();
    auto result = sqlite3_bind_text(
      stmt_, 1, idString.c_str(), idString.size() + 1, SQLITE_STATIC);
    throwOnSQLiteFailure(primaryDB_);
    result = sqlite3_step(stmt_);
    switch (result) {
      case SQLITE_ROW: // Found the ID in the DB.
      {
        ParameterSet pset;
        fhicl::make_ParameterSet(
          reinterpret_cast<char const*>(sqlite3_column_text(stmt_, 0)), pset);
        // Put into the registry without triggering ParameterSet::id().
        it = registry_.emplace(id, pset).first;
      } break;
      case SQLITE_DONE:
        break; // Not here.
      default:
        throwOnSQLiteFailure(primaryDB_);
    }
    sqlite3_reset(stmt_);
  }
  return it;
}
