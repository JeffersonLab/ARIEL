#include "cetlib/sqlite/detail/bind_parameters.h"
#include "cetlib/sqlite/Exception.h"

using namespace cet::sqlite;

namespace {
  [[noreturn]] void
  throw_bind_failure(std::string const& column_type, int const rc)
  {
    throw Exception{errors::SQLExecutionError} << "Failed to bind "
                                               << column_type << ".\n"
                                               << "Return code: " << rc << '\n';
  }
}

void
cet::sqlite::detail::bind_one_parameter(sqlite3_stmt* s,
                                        std::size_t const idx,
                                        double const v)
{
  int const rc{sqlite3_bind_double(s, idx, v)};
  if (rc != SQLITE_OK)
    throw_bind_failure("double", rc);
}

void
cet::sqlite::detail::bind_one_parameter(sqlite3_stmt* s,
                                        std::size_t const idx,
                                        int const v)
{
  int const rc{sqlite3_bind_int(s, idx, v)};
  if (rc != SQLITE_OK)
    throw_bind_failure("int", rc);
}

void
cet::sqlite::detail::bind_one_parameter(sqlite3_stmt* s,
                                        std::size_t const idx,
                                        std::uint32_t const v)
{
  int const rc{sqlite3_bind_int64(s, idx, v)};
  if (rc != SQLITE_OK)
    throw_bind_failure("int64", rc);
}

void
cet::sqlite::detail::bind_one_parameter(sqlite3_stmt* s,
                                        std::size_t const idx,
                                        sqlite_int64 const v)
{
  int const rc{sqlite3_bind_int64(s, idx, v)};
  if (rc != SQLITE_OK)
    throw_bind_failure("int64", rc);
}

void
cet::sqlite::detail::bind_one_parameter(sqlite3_stmt* s,
                                        std::size_t const idx,
                                        std::string const& v)
{
  int const rc{sqlite3_bind_text(s, idx, v.c_str(), v.size(), nullptr)};
  if (rc != SQLITE_OK)
    throw_bind_failure("text", rc);
}

void
cet::sqlite::detail::bind_one_null(sqlite3_stmt* s, std::size_t const idx)
{
  int const rc{sqlite3_bind_null(s, idx)};
  if (rc != SQLITE_OK)
    throw_bind_failure("null", rc);
}
