#include "art_root_io/RootDB/tkeyvfs.h"

extern "C" {
#include <sqlite3.h>
}

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "TFile.h"

namespace {
  constexpr bool no_root{TKEYVFS_NO_ROOT};
  constexpr bool use_root{!no_root};
}

using namespace std;

static int
callback(void*, int cnt, char** vals, char** col_name)
{
  int i = 0;
  for (i = 0; i < cnt; ++i) {
    if (vals[i]) {
      printf("%s: %s\n", col_name[i], vals[i]);
    } else {
      printf("%s: %s\n", col_name[i], "NULL");
    }
  }
  return 0;
}

int
main(int argc, char** argv)
{
  sqlite3* db = nullptr;
  char* error_msg = nullptr;
  int err = 0;
  if (argc < 4 || argc > 5) {
    fprintf(stderr,
            "usage: %s: <op> <db-filename> <db-tkeyname> [<sql-statement>]\n",
            argv[0]);
    exit(1);
  }
  tkeyvfs_init();
  TFile* rootFile = nullptr;
  int flags{};
  if (!strcmp(argv[1], "r")) {
    if (use_root) {
      rootFile = new TFile(argv[2]);
    }
    flags = SQLITE_OPEN_READONLY;
  } else if (!strcmp(argv[1], "u")) {
    if (use_root) {
      rootFile = new TFile(argv[2], "UPDATE");
    }
    flags = SQLITE_OPEN_READWRITE;
  } else if (!strcmp(argv[1], "w")) {
    if (use_root) {
      rootFile = new TFile(argv[2], "RECREATE");
    }
    flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  } else {
    fprintf(stderr, "Unrecognized file mode designator %s", argv[1]);
    exit(1);
  }

  if (use_root) {
    err = tkeyvfs_open_v2(argv[3], &db, flags, rootFile);
  } else {
    err = tkeyvfs_open_v2_noroot(argv[2], &db, flags);
  }

  if (err) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    exit(1);
  }
  if (argc == 5) {
    err = sqlite3_exec(db, argv[4], callback, 0, &error_msg);
  } else {
    char* buf = nullptr;
    size_t n = 0;
    getline(&buf, &n, stdin);
    err = sqlite3_exec(db, buf, callback, 0, &error_msg);
    free(buf);
  }
  if (err != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", error_msg);
    sqlite3_free(error_msg);
  }
  sqlite3_close(db);
}
