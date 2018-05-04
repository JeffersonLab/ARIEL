#include "cetlib/sqlite/Connection.h"

using namespace cet::sqlite;

// The move operations CANNOT move the mutex as the mutex must be
// shared across each connection.  The move c'tor/assignment operator
// must therefore copy the shared pointer to the mutex.

Connection::Connection(Connection&& c) noexcept : m_{c.m_}
{
  db_ = c.db_;
  c.db_ = nullptr;
}

Connection&
Connection::operator=(Connection&& c) noexcept
{
  m_ = c.m_;
  db_ = c.db_;
  c.db_ = nullptr;
  return *this;
}

Connection::~Connection() noexcept
{
  // It is safe to call sqlite3_close on a null db_.
  sqlite3_close(db_);
}
