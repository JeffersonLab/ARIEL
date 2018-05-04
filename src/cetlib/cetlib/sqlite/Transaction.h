#ifndef cetlib_sqlite_Transaction_h
#define cetlib_sqlite_Transaction_h

// ====================================================================
// Transaction
//
// A Transaction object encapsulates a single SQLite transaction. Such
// transactions cannot be nested. Look into using SAVEPOINT if you
// need nesting.
//
// Creating a Transaction begins a transaction in the given db, which
// is to have been already be opened.
//
// A transaction is committed by specifying 'commit()', which can be
// called ONLY ONCE per transaction object.  If 'commit' is not called
// before the Transaction object is destroyed, then the transaction is
// rolled back.
// ====================================================================

struct sqlite3;

namespace cet {
  namespace sqlite {

    class Transaction {
    public:
      explicit Transaction(sqlite3* db);
      ~Transaction() noexcept;

      void commit();

      // Transactions may not be copied or moved.
      Transaction(Transaction const&) = delete;
      Transaction(Transaction&&) = delete;
      Transaction& operator=(Transaction const&) = delete;
      Transaction& operator=(Transaction&&) = delete;

    private:
      sqlite3* db_;
    };

  } // sqlite
} // cet

#endif /* cetlib_sqlite_Transaction_h */

// Local Variables:
// mode: c++
// End:
