#include <iostream>

#include "fhiclcpp/DatabaseSupport.h"

using namespace std;

int
main(int argc, char* argv[])
{
  if (argc != 3) {
    cerr << argv[0] << ": two arguments required\n"
         << "Usage: " << argv[0] << " fhicl-file database-file\n\n";
    return 1;
  }

  char const* fhiclfile = argv[1];
  char const* dbname = argv[2];

  sqlite3* db = nullptr;

  int rc = sqlite3_open_v2(
    dbname, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
  if (rc != SQLITE_OK) {
    cerr << argv[0] << ": unable to open SQLite3 file " << dbname << '\n';
    return 2;
  }

  fhicl::parse_file_and_fill_db(fhiclfile, db);
  rc = sqlite3_close(db);
  if (rc != SQLITE_OK) {
    cerr << argv[0] << ": failure closing SQLite file " << dbname
         << "; file may be corrupt\n";
    return 3;
  }

  return 0;
}
