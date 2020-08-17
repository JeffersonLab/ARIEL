#ifndef art_root_io_RootDB_tkeyvfs_h
#define art_root_io_RootDB_tkeyvfs_h

// Save db to root file on close.
#include <sqlite3.h>

class TFile;

extern "C" {
int tkeyvfs_init();
int tkeyvfs_open_v2_noroot(char const* filename, sqlite3** ppDb, int flags);
int tkeyvfs_open_v2(char const* filename,
                    sqlite3** ppDb,
                    int flags,
                    TFile* rootFile);
}

#endif /* art_root_io_RootDB_tkeyvfs_h */

// Local Variables:
// mode: c++
// End:
