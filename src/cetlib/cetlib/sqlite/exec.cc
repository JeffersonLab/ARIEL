#include "cetlib/sqlite/exec.h"
#include "cetlib/sqlite/Exception.h"

void
cet::sqlite::exec(sqlite3* db, std::string const& ddl)
{
  char* errmsg{nullptr};
  if (sqlite3_exec(db, ddl.c_str(), nullptr, nullptr, &errmsg) != SQLITE_OK) {
    std::string msg{errmsg};
    sqlite3_free(errmsg);
    throw Exception{sqlite::errors::SQLExecutionError} << msg;
  }
}
