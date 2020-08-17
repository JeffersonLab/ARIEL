#include "art_root_io/RootDB/tkeyvfs.h"
#include "cetlib/compiler_macros.h"

#define SQLITE_FCNTL_SIZE_HINT 5
#define SQLITE_FCNTL_CHUNK_SIZE 6
#define SQLITE_FCNTL_SYNC_OMITTED 8

#define SQLITE_FCNTL_DB_UNCHANGED 0xca093fa0

using i64 = sqlite_int64;

// Save db to root file on close.

#ifndef TKEYVFS_NO_ROOT
#include "TFile.h"
#include "TKey.h"
#endif // TKEYVFS_NO_ROOT

#include <array>
#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
}

// Debug tracing
#define TKEYVFS_TRACE 0

namespace {

  class Trace {
  public:
    constexpr Trace(char const* str) noexcept : str_{str}
    {
#if TKEYVFS_TRACE
      fprintf(stderr, "Begin %s ...\n", str_);
#endif
    }
    ~Trace() noexcept
    {
#if TKEYVFS_TRACE
      fprintf(stderr, "End   %s ...\n", str_);
#endif
    }

  private:
    char const* str_ UNUSED_PRIVATE_FIELD;
  };

  // Externally provided ROOT file, must be open.
#ifndef TKEYVFS_NO_ROOT
  TFile* gRootFile;
#endif // TKEYVFS_NO_ROOT

  constexpr i64 mem_page{2048};            // Memory page size
  constexpr std::size_t max_pathname{512}; // Maximum supported path-length
  constexpr int sqlite_default_sector_size{512};

  // The unixFile structure is subclass of sqlite3_file specific to the unix
  // VFS implementations.
  struct unixFile {
    sqlite3_io_methods const* pMethod; //  Always the first entry
#ifndef TKEYVFS_NO_ROOT
    TFile* rootFile;    //  The ROOT file the db is stored in
    int saveToRootFile; //  On close, save db to root file
#endif                  // TKEYVFS_NO_ROOT
    char* pBuf;         //  File contents
    i64 bufAllocated;   //  File buffer size in bytes
    i64 fileSize;       //  Current file size in bytes
    int eFileLock;      //  The type of lock held on this fd
    int lastErrno;      //  The unix errno from last I/O error
    char const* zPath;  //  Name of the file
    int szChunk;        //  Configured by FCNTL_CHUNK_SIZE

    // The next group of variables are used to track whether or not the
    // transaction counter in bytes 24-27 of database files are updated
    // whenever any part of the database changes.  An assertion fault
    // will occur if a file is updated without also updating the
    // transaction counter.  This test is made to avoid new problems
    // similar to the one described by ticket #3584.
    unsigned char transCntrChng; //  True if the transaction counter changed
    unsigned char dbUpdate;      //  True if any part of database file changed
    unsigned char inNormalWrite; //  True if in a normal write operation
  };

  int sqlite3CantopenError(int lineno);

  // VFS calls
  int unixLogErrorAtLine(int errcode,
                         char const* zFunc,
                         char const* zPath,
                         int iLine);
  int robust_open(char const* z, int f, int m);
  void robust_close(unixFile* pFile, int h, int lineno);
  int unixGetTempname(int nBuf, char* zBuf);
  int fcntlSizeHint(unixFile* pFile, i64 nByte);
  int seekAndRead(unixFile* id, sqlite3_int64 offset, void* pBuf, int cnt);
  int seekAndWrite(unixFile* id, i64 offset, void const* pBuf, int cnt);

  int unixOpen(sqlite3_vfs* pVfs,
               char const* zPath,
               sqlite3_file* pFile,
               int flags,
               int* pOutFlags);
  int closeUnixFile(sqlite3_file* id);
  int unixDelete(sqlite3_vfs* NotUsed, char const* zPath, int dirSync);
  int unixAccess(sqlite3_vfs* NotUsed,
                 char const* zPath,
                 int flags,
                 int* pResOut);
  int unixFullPathname(sqlite3_vfs* pVfs,
                       char const* zPath,
                       int nOut,
                       char* zOut);
  void* unixDlOpen(sqlite3_vfs* NotUsed, char const* zFilename);
  void unixDlError(sqlite3_vfs* NotUsed, int nBuf, char* zBufOut);
  void (*unixDlSym(sqlite3_vfs* NotUsed, void* p, char const* zSym))();
  void unixDlClose(sqlite3_vfs* NotUsed, void* pHandle);
  int unixRandomness(sqlite3_vfs* NotUsed, int nBuf, char* zBuf);
  int unixSleep(sqlite3_vfs* NotUsed, int microseconds);
  int unixCurrentTime(sqlite3_vfs* NotUsed, double* prNow);
  int unixCurrentTimeInt64(sqlite3_vfs* NotUsed, sqlite3_int64* piNow);
  int unixGetLastError(sqlite3_vfs* NotUsed, int NotUsed2, char* NotUsed3);

  int
  sqlite3CantopenError(int const lineno)
  {
    fprintf(stderr,
            "tkeyvfs.c: cannot open file at line %d of [%.10s]",
            lineno,
            20 + sqlite3_sourceid());
    return SQLITE_CANTOPEN;
  }
#define SQLITE_CANTOPEN_BKPT sqlite3CantopenError(__LINE__)

  // IoMethods calls
  int
  nolockClose(sqlite3_file* id)
  {
    Trace t{"nolockClose"};
    int val = closeUnixFile(id);
    return val;
  }

  int unixRead(sqlite3_file* id, void* pBuf, int amt, sqlite3_int64 offset);
  int unixWrite(sqlite3_file* id,
                void const* pBuf,
                int amt,
                sqlite3_int64 offset);
  int unixTruncate(sqlite3_file* id, i64 nByte);
  int unixSync(sqlite3_file* id, int flags);
  int unixFileSize(sqlite3_file* id, i64* pSize);
  int nolockLock(sqlite3_file*, int);
  int nolockUnlock(sqlite3_file*, int);
  int nolockCheckReservedLock(sqlite3_file*, int* pResOut);
  int unixFileControl(sqlite3_file* id, int op, void* pArg);
  int unixSectorSize(sqlite3_file*);
  int unixDeviceCharacteristics(sqlite3_file*);

  sqlite3_io_methods const nolockIoMethods{
    1,                         //  iVersion
    nolockClose,               //  xClose
    unixRead,                  //  xRead
    unixWrite,                 //  xWrite
    unixTruncate,              //  xTruncate
    unixSync,                  //  xSync
    unixFileSize,              //  xFileSize
    nolockLock,                //  xLock
    nolockUnlock,              //  xUnlock
    nolockCheckReservedLock,   //  xCheckReservedLock
    unixFileControl,           //  xFileControl
    unixSectorSize,            //  xSectorSize
    unixDeviceCharacteristics, //  xDeviceCapabilities
    nullptr,                   //  v2, xShmMap
    nullptr,                   //  v2, xShmLock
    nullptr,                   //  v2, xShmBarrier
    nullptr,                   //  v2, xShmUnmap
    nullptr,                   //  v3, xFetch
    nullptr                    //  v3, xUnfetch
  };

  sqlite3_io_methods const*
  nolockIoFinder(char const*, unixFile*)
  {
    return &nolockIoMethods;
  }

  // This function - unixLogError_x(), is only ever called via the
  // macro unixLogError().
  //
  // It is invoked after an error occurs in an OS function and errno
  // has been set. It logs a message using sqlite3_log() containing
  // the current value of errno and, if possible, the human-readable
  // equivalent from strerror() or strerror_r().
  //
  // The first argument passed to the macro should be the error code
  // that will be returned to SQLite (e.g. SQLITE_IOERR_DELETE,
  // SQLITE_CANTOPEN).  The two subsequent arguments should be the
  // name of the OS function that failed (e.g. "unlink", "open") and
  // the the associated file-system path, if any.
#define unixLogError(a, b, c) unixLogErrorAtLine(a, b, c, __LINE__)
  int
  unixLogErrorAtLine(int errcode,       //  SQLite error code
                     char const* zFunc, //  Name of OS function that failed
                     char const* zPath, //  File path associated with error
                     int const iLine //  Source line number where error occurred
  )
  {
    int const iErrno = errno; //  Saved syscall error number
    char* zErr = strerror(iErrno);
    if (zPath == nullptr) {
      zPath = "";
    }
    fprintf(stderr,
            "tkeyvfs.c:%d: (%d) %s(%s) - %s",
            iLine,
            iErrno,
            zFunc,
            zPath,
            zErr);
    return errcode;
  }

  // Retry open() calls that fail due to EINTR
  int
  robust_open(char const* z, int const f, int const m)
  {
    Trace tr{"robust_open"};
    int rc;
    do {
      rc = open(z, f, m);
    } while (rc < 0 && errno == EINTR);
    return rc;
  }

  // Close a file descriptor.
  //
  // We assume that close() almost always works, since it is only in a
  // very sick application or on a very sick platform that it might fail.
  // If it does fail, simply leak the file descriptor, but do log the
  // error.
  //
  // Note that it is not safe to retry close() after EINTR since the
  // file descriptor might have already been reused by another thread.
  // So we don't even try to recover from an EINTR.  Just log the error
  // and move on.
  void
  robust_close(unixFile* pFile, int const h, int const lineno)
  {
    Trace tr{"robust_close"};
    if (close(h)) {
      if (pFile) {
        unixLogErrorAtLine(SQLITE_IOERR_CLOSE, "close", pFile->zPath, lineno);
      } else {
        unixLogErrorAtLine(SQLITE_IOERR_CLOSE, "close", nullptr, lineno);
      }
    }
  }

  // This function performs the parts of the "close file" operation
  // common to all locking schemes. It closes the directory and file
  // handles, if they are valid, and sets all fields of the unixFile
  // structure to 0.
  //
  // It is *not* necessary to hold the mutex when this routine is called,
  // even on VxWorks.  A mutex will be acquired on VxWorks by the
  // vxworksReleaseFileId() routine.
  int
  closeUnixFile(sqlite3_file* id)
  {
    auto pFile = reinterpret_cast<unixFile*>(id);
#if TKEYVFS_TRACE
    fprintf(stderr, "Begin closeUnixFile ...\n");
    if (pFile->zPath) {
      fprintf(stderr, "filename: %s\n", pFile->zPath);
    }
#ifndef TKEYVFS_NO_ROOT
    fprintf(stderr, "saveToRootFile: %d\n", pFile->saveToRootFile);
#endif // TKEYVFS_NO_ROOT
#endif // TKEYVFS_TRACE
#ifndef TKEYVFS_NO_ROOT
    if (pFile->saveToRootFile) {
#if TKEYVFS_TRACE
      fprintf(stderr, "fileSize: 0x%016lx\n", pFile->fileSize);
#endif // TKEYVFS_TRACE
      // Create a tkey which will contain the contents
      // of the database in the root file
      auto k = new TKey{pFile->zPath,
                        "sqlite3 database file",
                        TKey::Class(),
                        static_cast<int>(pFile->fileSize) /*nbytes*/,
                        pFile->rootFile /*dir*/};
#if TKEYVFS_TRACE
      //  Ask the key for the size of the database file it contains.
      int const objlen = k->GetObjlen();
      fprintf(stderr, "objlen: %d\n", objlen);
#endif // TKEYVFS_TRACE
      //  Add the new key to the root file toplevel directory.
      //  Note: The tkey is now owned by the root file.
      int const cycle = pFile->rootFile->AppendKey(k);
      //  Get a pointer to the i/o buffer inside the tkey.
      char* p = k->GetBuffer();
      //  Copy the entire in-memory database file into the tkey i/o buffer.
      std::memcpy(p, pFile->pBuf, (size_t)pFile->fileSize);
      //  Write the tkey contents to the root file.
      /* Note: This has not yet written the top-level directory entry for the
       * key.
       */
      int cnt = k->WriteFile(cycle, nullptr /*file*/);
      if (cnt == -1) {
        //  bad
        fprintf(stderr,
                "tkeyvfs: failed to write root tkey containing database "
                "to root file!\n");
      }
      /* Force the root file to flush the top-level directory entry for our tkey
       * to disk. */
      cnt = pFile->rootFile->Write("", TObject::kOverwrite);
      if (cnt < 0) {
        //  bad
        fprintf(stderr, "tkeyvfs: failed to write root file to disk!\n");
      }
    }
#endif // TKEYVFS_NO_ROOT
    if (pFile->pBuf != nullptr) {
      free(pFile->pBuf);
    }
    if (pFile->zPath != nullptr) {
      free((void*)pFile->zPath);
    }
    std::memset(pFile, 0, sizeof(unixFile));
#if TKEYVFS_TRACE
    fprintf(stderr, "End   closeUnixFile ...\n");
#endif // TKEYVFS_TRACE
    return SQLITE_OK;
  }

  /*
  ** Create a temporary file name in zBuf.  zBuf must be allocated
  ** by the calling process and must be big enough to hold at least
  ** pVfs->mxPathname bytes.
  */
  int
  unixGetTempname(int nBuf, char* zBuf)
  {
    Trace tr{"unixGetTempname"};
    static const unsigned char zChars[] = "abcdefghijklmnopqrstuvwxyz"
                                          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                          "0123456789";
    unsigned int i, j;
    // Check that the output buffer is large enough for the temporary file
    // name. If it is not, return SQLITE_ERROR.
    constexpr char const* sqlite_temp_file_prefix{"etilqs_"};
    if ((strlen(sqlite_temp_file_prefix) + 17) >= (size_t)nBuf) {
      return SQLITE_ERROR;
    }
    sqlite3_snprintf(nBuf - 17, zBuf, sqlite_temp_file_prefix);
    j = (int)strlen(zBuf);
    sqlite3_randomness(15, &zBuf[j]);
    for (i = 0; i < 15; i++, j++) {
      zBuf[j] = (char)zChars[((unsigned char)zBuf[j]) % (sizeof(zChars) - 1)];
    }
    zBuf[j] = 0;
    return SQLITE_OK;
  }

  // This function is called to handle the SQLITE_FCNTL_SIZE_HINT
  // file-control operation.
  //
  // If the user has configured a chunk-size for this file, it could be
  // that the file needs to be extended at this point. Otherwise, the
  // SQLITE_FCNTL_SIZE_HINT operation is a no-op for Unix.
  int
  fcntlSizeHint(unixFile* pFile, i64 const nByte)
  {
    Trace tr{"fcntlSizeHint"};
    if (pFile->szChunk) {
      i64 nSize = ((nByte + (pFile->szChunk - 1)) / pFile->szChunk) *
                  pFile->szChunk; // Required file size
      i64 nAlloc = ((nSize + (mem_page - 1)) / mem_page) * mem_page;
      if ((nSize > pFile->fileSize) && (nAlloc > pFile->bufAllocated)) {
        if (nAlloc > pFile->bufAllocated) {
          char* pNewBuf = (char*)realloc(pFile->pBuf, (size_t)nAlloc);
          if (pNewBuf == nullptr) {
            return SQLITE_IOERR_WRITE;
          }
          std::memset(
            pNewBuf + pFile->fileSize, 0, (size_t)(nAlloc - pFile->fileSize));
          pFile->pBuf = pNewBuf;
          pFile->bufAllocated = nAlloc;
        } else {
          std::memset(pFile->pBuf + pFile->fileSize,
                      0,
                      (size_t)(nSize - pFile->fileSize));
        }
        pFile->fileSize = nSize;
      }
    }
    return SQLITE_OK;
  }

  // Seek to the offset passed as the second argument, then read cnt
  // bytes into pBuf. Return the number of bytes actually read.
  //
  // NB:  If you define USE_PREAD or USE_PREAD64, then it might also
  // be necessary to define _XOPEN_SOURCE to be 500.  This varies from
  // one system to another.  Since SQLite does not define USE_PREAD
  // any any form by default, we will not attempt to define _XOPEN_SOURCE.
  // See tickets #2741 and #2681.
  //
  // To avoid stomping the errno value on a failed read the lastErrno value
  // is set before returning.
  int
  seekAndRead(unixFile* id, sqlite3_int64 offset, void* pBuf, int cnt)
  {
    Trace tr{"seekAndRead"};
    if (offset >= id->fileSize) {
      id->lastErrno = 0;
      return 0;
    }
    if ((offset + cnt) > id->fileSize) {
      cnt = (offset + cnt) - id->fileSize;
    }
    std::memcpy(pBuf, id->pBuf + offset, (size_t)cnt);
    return cnt;
  }

  // Seek to the offset in id->offset then read cnt bytes into pBuf.
  // Return the number of bytes actually read.  Update the offset.
  //
  // To avoid stomping the errno value on a failed write the lastErrno value
  // is set before returning.
  int
  seekAndWrite(unixFile* id, i64 offset, void const* pBuf, int cnt)
  {
    Trace tr{"seekAndWrite"};
    unixFile* pFile = (unixFile*)id;
    if ((offset + cnt) > id->bufAllocated) {
      i64 nByte = offset + static_cast<i64>(cnt);
      if (pFile->szChunk) {
        nByte =
          ((nByte + (pFile->szChunk - 1)) / pFile->szChunk) * pFile->szChunk;
      }
      auto newBufSize = ((nByte + mem_page - 1) / mem_page) * mem_page;
      char* pNewBuf = (char*)realloc(id->pBuf, (size_t)(newBufSize));
      if (pNewBuf == nullptr) {
        id->lastErrno = errno;
        return 0;
      }
      if ((offset + (i64)cnt) < newBufSize) {
        i64 zeroCnt = newBufSize - (offset + (i64)cnt);
        std::memset((pNewBuf + offset + (i64)cnt), 0, (size_t)zeroCnt);
      }
      id->pBuf = pNewBuf;
      id->bufAllocated = newBufSize;
    }
    std::memcpy((id->pBuf + offset), pBuf, (size_t)cnt);
    if ((offset + (i64)cnt) > id->fileSize) {
      id->fileSize = offset + (i64)cnt;
    }
    return cnt;
  }

  // --------------------------------------------------------------------------
  //  IoMethods calls

  // Read data from a file into a buffer.  Return SQLITE_OK if all
  // bytes were read successfully and SQLITE_IOERR if anything goes
  // wrong.
  int
  unixRead(sqlite3_file* id, void* pBuf, int amt, sqlite3_int64 offset)
  {
    unixFile* pFile = (unixFile*)id;
    int got;
#if TKEYVFS_TRACE
    fprintf(stderr, "Begin unixRead ...\n");
    if (pFile->zPath) {
      fprintf(stderr, "filename: %s\n", pFile->zPath);
    }
    fprintf(stderr,
            "offset: 0x%016lx  amt: 0x%08x\n",
            (unsigned long long)offset,
            amt);
#endif // TKEYVFS_TRACE
    got = seekAndRead(pFile, offset, pBuf, amt);
    if (got == amt) {
#if TKEYVFS_TRACE
      fprintf(stderr, "End   unixRead ...\n");
#endif // TKEYVFS_TRACE
      return SQLITE_OK;
    } else if (got < 0) {
      //  lastErrno set by seekAndRead
#if TKEYVFS_TRACE
      fprintf(stderr, "End   unixRead ...\n");
#endif // TKEYVFS_TRACE
      return SQLITE_IOERR_READ;
    } else {
      pFile->lastErrno = 0; //  not a system error
      //  Unread parts of the buffer must be zero-filled
      std::memset(&((char*)pBuf)[got], 0, amt - got);
#if TKEYVFS_TRACE
      fprintf(stderr, "End   unixRead ...\n");
#endif // TKEYVFS_TRACE
      return SQLITE_IOERR_SHORT_READ;
    }
  }

  // Write data from a buffer into a file.  Return SQLITE_OK on success
  // or some other error code on failure.
  int
  unixWrite(sqlite3_file* id, void const* pBuf, int amt, sqlite3_int64 offset)
  {
    unixFile* pFile = (unixFile*)id;
    int wrote = 0;
#if TKEYVFS_TRACE
    fprintf(stderr, "Begin unixWrite ...\n");
    if (pFile->zPath) {
      fprintf(stderr, "filename: %s\n", pFile->zPath);
    }
    fprintf(stderr, "offset: 0x%016lx  amt: 0x%08x\n", offset, amt);
#endif // TKEYVFS_TRACE
    /* If we are doing a normal write to a database file (as opposed to
    ** doing a hot-journal rollback or a write to some file other than a
    ** normal database file) then record the fact that the database
    ** has changed.  If the transaction counter is modified, record that
    ** fact too.
    */
    if (pFile->inNormalWrite) {
      pFile->dbUpdate = 1; //  The database has been modified
      if ((offset <= 24) && (offset + amt >= 27)) {
        int rc;
        char oldCntr[4];
        rc = seekAndRead(pFile, 24, oldCntr, 4);
        if (rc != 4 || memcmp(oldCntr, &((char*)pBuf)[24 - offset], 4) != 0) {
          pFile->transCntrChng = 1; //  The transaction counter has changed
        }
      }
    }
    while ((amt > 0) &&
           ((wrote = seekAndWrite(pFile, offset, pBuf, amt)) > 0)) {
      amt -= wrote;
      offset += wrote;
      pBuf = &((char*)pBuf)[wrote];
    }
    if (amt > 0) {
      if (wrote < 0) {
        //  lastErrno set by seekAndWrite
#if TKEYVFS_TRACE
        fprintf(stderr, "End   unixWrite ...\n");
#endif // TKEYVFS_TRACE
        return SQLITE_IOERR_WRITE;
      } else {
        pFile->lastErrno = 0; //  not a system error
#if TKEYVFS_TRACE
        fprintf(stderr, "End   unixWrite ...\n");
#endif // TKEYVFS_TRACE
        return SQLITE_FULL;
      }
    }
#if TKEYVFS_TRACE
    fprintf(stderr, "End   unixWrite ...\n");
#endif // TKEYVFS_TRACE
    return SQLITE_OK;
  }

  /*
  ** Truncate an open file to a specified size
  */
  int
  unixTruncate(sqlite3_file* id, i64 nByte)
  {
    unixFile* pFile = (unixFile*)id;
#if TKEYVFS_TRACE
    fprintf(stderr, "Begin unixTruncate ...\n");
    if (pFile->zPath) {
      fprintf(stderr, "filename: %s\n", pFile->zPath);
    }
    fprintf(stderr, "nByte: 0x%016lx\n", (unsigned long long)nByte);
#endif // TKEYVFS_TRACE
    /* If the user has configured a chunk-size for this file, truncate the
    ** file so that it consists of an integer number of chunks (i.e. the
    ** actual file size after the operation may be larger than the requested
    ** size).
    */
    if (pFile->szChunk) {
      nByte = ((nByte + pFile->szChunk - 1) / pFile->szChunk) * pFile->szChunk;
    }
    if (nByte == 0) {
      free(pFile->pBuf);
      pFile->pBuf = (char*)calloc(1, mem_page);
      if (pFile->pBuf == nullptr) {
        pFile->bufAllocated = 0;
        pFile->fileSize = 0;
        pFile->lastErrno = errno;
#if TKEYVFS_TRACE
        fprintf(stderr, "End   unixTruncate ...\n");
#endif // TKEYVFS_TRACE
        return unixLogError(SQLITE_IOERR_TRUNCATE, "ftruncate", pFile->zPath);
      }
      pFile->bufAllocated = mem_page;
      pFile->fileSize = 0;
    } else {
      i64 newBufSize = ((nByte + mem_page - 1) / mem_page) * mem_page;
      i64 zeroCnt = newBufSize - nByte;
      char* pNewBuf = (char*)realloc(pFile->pBuf, (size_t)newBufSize);
      if (pNewBuf == nullptr) {
        pFile->lastErrno = errno;
#if TKEYVFS_TRACE
        fprintf(stderr, "End   unixTruncate ...\n");
#endif // TKEYVFS_TRACE
        return unixLogError(SQLITE_IOERR_TRUNCATE, "ftruncate", pFile->zPath);
      }
      std::memset((pNewBuf + nByte), 0, (size_t)zeroCnt);
      pFile->pBuf = pNewBuf;
      pFile->bufAllocated = newBufSize;
      pFile->fileSize = nByte;
    }
    /* If we are doing a normal write to a database file (as opposed to
    ** doing a hot-journal rollback or a write to some file other than a
    ** normal database file) and we truncate the file to zero length,
    ** that effectively updates the change counter.  This might happen
    ** when restoring a database using the backup API from a zero-length
    ** source.
    */
    if (pFile->inNormalWrite && (nByte == 0)) {
      pFile->transCntrChng = 1;
    }
#if TKEYVFS_TRACE
    fprintf(stderr, "End   unixTruncate ...\n");
#endif // TKEYVFS_TRACE
    return SQLITE_OK;
  }

  // Make sure all writes to a particular file are committed to disk.
  //
  // If dataOnly==0 then both the file itself and its metadata (file
  // size, access time, etc) are synced.  If dataOnly!=0 then only the
  // file data is synced.
  //
  // Under Unix, also make sure that the directory entry for the file
  // has been created by fsync-ing the directory that contains the file.
  // If we do not do this and we encounter a power failure, the directory
  // entry for the journal might not exist after we reboot.  The next
  // SQLite to access the file will not know that the journal exists (because
  // the directory entry for the journal was never created) and the transaction
  // will not roll back - possibly leading to database corruption.
  int
  unixSync(sqlite3_file* id [[maybe_unused]], int)
  {
#if TKEYVFS_TRACE
    fprintf(stderr, "Begin unixSync ...\n");
    if (((unixFile*)id)->zPath) {
      fprintf(stderr, "filename: %s\n", ((unixFile*)id)->zPath);
    }
#endif // TKEYVFS_TRACE
#if TKEYVFS_TRACE
    fprintf(stderr, "End   unixSync ...\n");
#endif // TKEYVFS_TRACE
    return SQLITE_OK;
  }

  /*
  ** Determine the current size of a file in bytes
  */
  int
  unixFileSize(sqlite3_file* id, i64* pSize)
  {
    unixFile* p = (unixFile*)id;
#if TKEYVFS_TRACE
    fprintf(stderr, "Begin unixFileSize ...\n");
    if (((unixFile*)id)->zPath) {
      fprintf(stderr, "filename: %s\n", ((unixFile*)id)->zPath);
    }
#endif // TKEYVFS_TRACE
    *pSize = p->fileSize;
    /* When opening a zero-size database, the findInodeInfo() procedure
    ** writes a single byte into that file in order to work around a bug
    ** in the OS-X msdos filesystem.  In order to avoid problems with upper
    ** layers, we need to report this file size as zero even though it is
    ** really 1.   Ticket #3260.
    */
    if (*pSize == 1) {
      *pSize = 0;
    }
#if TKEYVFS_TRACE
    fprintf(stderr, "End   unixFileSize ...\n");
#endif // TKEYVFS_TRACE
    return SQLITE_OK;
  }

  int
  nolockLock(sqlite3_file*, int)
  {
#if TKEYVFS_TRACE
    fprintf(stderr, "Begin nolockLock ...\n");
    if (((unixFile*)id)->zPath) {
      fprintf(stderr, "filename: %s\n", ((unixFile*)id)->zPath);
    }
#endif // TKEYVFS_TRACE
#if TKEYVFS_TRACE
    fprintf(stderr, "End   nolockLock ...\n");
#endif // TKEYVFS_TRACE
    return SQLITE_OK;
  }

  int
  nolockUnlock(sqlite3_file* id [[maybe_unused]], int)
  {
#if TKEYVFS_TRACE
    fprintf(stderr, "Begin nolockUnlock ...\n");
    if (((unixFile*)id)->zPath) {
      fprintf(stderr, "filename: %s\n", ((unixFile*)id)->zPath);
    }
#endif // TKEYVFS_TRACE
#if TKEYVFS_TRACE
    fprintf(stderr, "End   nolockUnlock ...\n");
#endif // TKEYVFS_TRACE
    return SQLITE_OK;
  }

  int
  nolockCheckReservedLock(sqlite3_file*, int* pResOut)
  {
#if TKEYVFS_TRACE
    fprintf(stderr, "Begin nolockCheckReservedLock ...\n");
    if (((unixFile*)id)->zPath) {
      fprintf(stderr, "filename: %s\n", ((unixFile*)id)->zPath);
    }
#endif // TKEYVFS_TRACE
    *pResOut = 0;
#if TKEYVFS_TRACE
    fprintf(stderr, "End   nolockCheckReservedLock ...\n");
#endif // TKEYVFS_TRACE
    return SQLITE_OK;
  }

  /*
  ** Information and control of an open file handle.
  */
  int
  unixFileControl(sqlite3_file* id, int op, void* pArg)
  {
    auto uFile = reinterpret_cast<unixFile*>(id);
    auto op_l = static_cast<long int>(op);
#if TKEYVFS_TRACE
    fprintf(stderr, "Begin unixFileControl ...\n");
    if (uFile->zPath) {
      fprintf(stderr, "filename: %s\n", uFile->zPath);
    }
#endif // TKEYVFS_TRACE
    switch (op_l) {
      case SQLITE_FCNTL_LOCKSTATE: {
#if TKEYVFS_TRACE
        fprintf(stderr, "op: LOCKSTATE\n");
#endif // TKEYVFS_TRACE
        *(int*)pArg = uFile->eFileLock;
        // SQLITE_LOCK_NONE
#if TKEYVFS_TRACE
        fprintf(stderr, "End   unixFileControl ...\n");
#endif // TKEYVFS_TRACE
        return SQLITE_OK;
      }
      case SQLITE_LAST_ERRNO: {
#if TKEYVFS_TRACE
        fprintf(stderr, "op: LAST_ERRNO\n");
#endif // TKEYVFS_TRACE
        *(int*)pArg = uFile->lastErrno;
#if TKEYVFS_TRACE
        fprintf(stderr, "End   unixFileControl ...\n");
#endif // TKEYVFS_TRACE
        return SQLITE_OK;
      }
      case SQLITE_FCNTL_CHUNK_SIZE: {
#if TKEYVFS_TRACE
        fprintf(stderr, "op: CHUNK_SIZE\n");
        fprintf(stderr, "szChunk: %d\n", *(int*)pArg);
#endif // TKEYVFS_TRACE
        uFile->szChunk = *(int*)pArg;
#if TKEYVFS_TRACE
        fprintf(stderr, "End   unixFileControl ...\n");
#endif // TKEYVFS_TRACE
        return SQLITE_OK;
      }
      case SQLITE_FCNTL_SIZE_HINT: {
#if TKEYVFS_TRACE
        fprintf(stderr, "op: SIZE_HINT\n");
        fprintf(stderr, "hint: 0x%016lx\n", *(i64*)pArg);
#endif // TKEYVFS_TRACE
        int val = fcntlSizeHint(uFile, *(i64*)pArg);
#if TKEYVFS_TRACE
        fprintf(stderr, "End   unixFileControl ...\n");
#endif // TKEYVFS_TRACE
        return val;
      }
        /* The pager calls this method to signal that it has done
        ** a rollback and that the database is therefore unchanged and
        ** it hence it is OK for the transaction change counter to be
        ** unchanged.
        */
      case SQLITE_FCNTL_DB_UNCHANGED: {
#if TKEYVFS_TRACE
        fprintf(stderr, "op: DB_UNCHANGED\n");
#endif // TKEYVFS_TRACE
        uFile->dbUpdate = 0;
#if TKEYVFS_TRACE
        fprintf(stderr, "End   unixFileControl ...\n");
#endif // TKEYVFS_TRACE
        return SQLITE_OK;
      }
      case SQLITE_FCNTL_SYNC_OMITTED: {
#if TKEYVFS_TRACE
        fprintf(stderr, "op: SYNC_OMITTED\n");
        fprintf(stderr, "End   unixFileControl ...\n");
#endif                    // TKEYVFS_TRACE
        return SQLITE_OK; //  A no-op
      }
    }
#if TKEYVFS_TRACE
    fprintf(stderr, "End   unixFileControl ...\n");
#endif // TKEYVFS_TRACE
    return SQLITE_NOTFOUND;
  }

  // Return the sector size in bytes of the underlying block device for
  // the specified file. This is almost always 512 bytes, but may be
  // larger for some devices.
  //
  // SQLite code assumes this function cannot fail. It also assumes that
  // if two files are created in the same file-system directory (i.e.
  // a database and its journal file) that the sector size will be the
  // same for both.
  int
  unixSectorSize(sqlite3_file*)
  {
    Trace tr{"unixSectorSize"};
    return sqlite_default_sector_size;
  }

  int
  unixDeviceCharacteristics(sqlite3_file*)
  {
    Trace tr{"unixDeviceCharacteristics"};
    return 0; // Always 0 for unix.
  }

  // Find the current time (in Universal Coordinated Time).  Write into *piNow
  // the current time and date as a Julian Day number times 86_400_000.  In
  // other words, write into *piNow the number of milliseconds since the Julian
  // epoch of noon in Greenwich on November 24, 4714 B.C according to the
  // proleptic Gregorian calendar.
  //
  // On success, return 0.  Return 1 if the time and date cannot be found.
  int
  unixCurrentTimeInt64(sqlite3_vfs*, sqlite3_int64* piNow)
  {
    Trace tr{"unixCurrentTimeInt64"};
    constexpr sqlite3_int64 unixEpoch = 24405875 * (sqlite3_int64)8640000;
    struct timeval sNow;
    gettimeofday(&sNow, 0);
    *piNow =
      unixEpoch + 1000 * (sqlite3_int64)sNow.tv_sec + sNow.tv_usec / 1000;
    return 0;
  }

  // --------------------------------------------------------------------------
  //  VFS calls

  /*
  ** Open the file zPath.
  **
  ** Previously, the SQLite OS layer used three functions in place of this
  ** one:
  **
  **     sqlite3OsOpenReadWrite();
  **     sqlite3OsOpenReadOnly();
  **     sqlite3OsOpenExclusive();
  **
  ** These calls correspond to the following combinations of flags:
  **
  **     ReadWrite() ->     (READWRITE | CREATE)
  **     ReadOnly()  ->     (READONLY)
  **     OpenExclusive() -> (READWRITE | CREATE | EXCLUSIVE)
  **
  ** The old OpenExclusive() accepted a boolean argument - "delFlag". If
  ** true, the file was configured to be automatically deleted when the
  ** file handle closed. To achieve the same effect using this new
  ** interface, add the DELETEONCLOSE flag to those specified above for
  ** OpenExclusive().
  */
  int
  unixOpen(
    sqlite3_vfs* /*pVfs*/, /* The VFS for which this is the xOpen method */
    char const* zPath,     //  Pathname of file to be opened
    sqlite3_file* pFile,   //  The file descriptor to be filled in
    int flags,             //  Input flags to control the opening
    int* pOutFlags         //  Output flags returned to SQLite core
  )
  {
    auto p = reinterpret_cast<unixFile*>(pFile);
    int const eType = (flags & 0xFFFFFF00); //  Type of file to open
    int rc = SQLITE_OK;
    int const isDelete = (flags & SQLITE_OPEN_DELETEONCLOSE);
    int const isCreate = (flags & SQLITE_OPEN_CREATE);
    int const isReadWrite = (flags & SQLITE_OPEN_READWRITE);
    char zTmpname[max_pathname + 1];
    auto zName = zPath;
#if TKEYVFS_TRACE
    fprintf(stderr, "Begin unixOpen ...\n");
    if (zPath != nullptr) {
      fprintf(stderr, "filename: %s\n", zPath);
    }
#endif // TKEYVFS_TRACE
    std::memset(p, 0, sizeof(unixFile));
    if (pOutFlags) {
      *pOutFlags = flags;
    }
    if (!zName) {
      rc = unixGetTempname(max_pathname + 1, zTmpname);
      if (rc != SQLITE_OK) {
        return rc;
      }
      zName = zTmpname;
    }
    if (zName != nullptr) {
      p->zPath = (char*)malloc(strlen(zName) + 1);
      if (p->zPath != nullptr) {
        strcpy((char*)p->zPath, zName);
      }
    }
    p->lastErrno = 0;
    p->pMethod = &nolockIoMethods;
#ifndef TKEYVFS_NO_ROOT
    p->rootFile = nullptr;
    if (eType & SQLITE_OPEN_MAIN_DB) {
      p->rootFile = gRootFile;
    }
    p->saveToRootFile =
      (p->rootFile && p->rootFile->IsWritable() &&
       (eType & SQLITE_OPEN_MAIN_DB) && (isCreate || isReadWrite) && !isDelete);
#endif // TKEYVFS_NO_ROOT
    if ((eType & SQLITE_OPEN_MAIN_DB) && !isCreate) {
      i64 nBytes = 0;
      i64 nAlloc = 0;
#ifndef TKEYVFS_NO_ROOT
      char* pKeyBuf = nullptr;
      // Read the highest numbered cycle of the tkey which contains
      // the database from the root file.
      TKey* k = p->rootFile->GetKey(p->zPath, 9999 /*cycle*/);
      // Force the tkey to allocate an i/o buffer for its contents.
      k->SetBuffer();
      // Read the contents of the tkey from the root file.
      bool const status = k->ReadFile();
      if (!status) {
#if TKEYVFS_TRACE
        fprintf(stderr, "End   unixOpen ...\n");
#endif // TKEYVFS_TRACE
        rc = unixLogError(SQLITE_CANTOPEN_BKPT, "open", zName);
        return rc;
      }
      // Get a pointer to the tkey i/o buffer.
      pKeyBuf = k->GetBuffer();
      // Get the size of the contained database file from the tkey.
      nBytes = k->GetObjlen();
      // Allocate enough memory pages to contain the database file.
      nAlloc = ((nBytes + mem_page - 1) / mem_page) * mem_page;
      p->pBuf = (char*)malloc((size_t)nAlloc);
#else  // TKEYVFS_NO_ROOT
      //  If not using root, a database file read is a noop.
      nBytes = 0;
      nAlloc = mem_page;
      p->pBuf = (char*)calloc(1, mem_page);
#endif // TKEYVFS_NO_ROOT
      if (p->pBuf == nullptr) {
#if TKEYVFS_TRACE
        fprintf(stderr, "End   unixOpen ...\n");
#endif // TKEYVFS_TRACE
        rc = unixLogError(SQLITE_CANTOPEN_BKPT, "open", zName);
        return rc;
      }
#ifndef TKEYVFS_NO_ROOT
      /* Copy the entire database file from the tkey i/o buffer
      ** into our in-memory database. */
      std::memcpy(p->pBuf, pKeyBuf, (size_t)nBytes);
#endif // TKEYVFS_NO_ROOT
      p->bufAllocated = nAlloc;
      p->fileSize = nBytes;
    } else {
      p->pBuf = (char*)calloc(1, mem_page);
      if (p->pBuf == nullptr) {
#if TKEYVFS_TRACE
        fprintf(stderr, "End   unixOpen ...\n");
#endif // TKEYVFS_TRACE
        rc = unixLogError(SQLITE_CANTOPEN_BKPT, "open", zName);
        return rc;
      }
      p->bufAllocated = mem_page;
      p->fileSize = 0;
    }
    rc = SQLITE_OK;
#if TKEYVFS_TRACE
    fprintf(stderr, "End   unixOpen ...\n");
#endif // TKEYVFS_TRACE
    return rc;
  }

  /*
  ** Delete the file at zPath. If the dirSync argument is true, fsync()
  ** the directory after deleting the file.
  */
  int
  unixDelete(
    sqlite3_vfs*,          //  VFS containing this as the xDelete method
    char const* /*zPath*/, /* Name of file to be deleted */
    int /*dirSync*/        /* If true, fsync() directory after deleting file */
  )
  {
    int rc = SQLITE_OK;
#if TKEYVFS_TRACE
    fprintf(stderr, "Begin unixDelete ...\n");
    if (zPath != nullptr) {
      fprintf(stderr, "filename: %s\n", zPath);
    }
#endif // TKEYVFS_TRACE
#if TKEYVFS_TRACE
    fprintf(stderr, "End   unixDelete ...\n");
#endif // TKEYVFS_TRACE
    return rc;
  }

  /*
  ** Test the existance of or access permissions of file zPath. The
  ** test performed depends on the value of flags:
  **
  **     SQLITE_ACCESS_EXISTS: Return 1 if the file exists
  **     SQLITE_ACCESS_READWRITE: Return 1 if the file is read and writable.
  **     SQLITE_ACCESS_READONLY: Return 1 if the file is readable.
  **
  ** Otherwise return 0.
  */
  int
  unixAccess(sqlite3_vfs*,          //  The VFS containing this xAccess method
             char const* /*zPath*/, /* Path of the file to examine */
             int flags,   //  What do we want to learn about the zPath file?
             int* pResOut //  Write result boolean here
  )
  {
#if TKEYVFS_TRACE
    fprintf(stderr, "Begin unixAccess ...\n");
    if (zPath != nullptr) {
      fprintf(stderr, "filename: %s\n", zPath);
    }
#endif // TKEYVFS_TRACE
    switch (flags) {
      case SQLITE_ACCESS_EXISTS:
#if TKEYVFS_TRACE
        fprintf(stderr, "op: SQLITE_ACCESS_EXISTS\n");
#endif // TKEYVFS_TRACE
        break;
      case SQLITE_ACCESS_READWRITE:
#if TKEYVFS_TRACE
        fprintf(stderr, "op: SQLITE_ACCESS_READWRITE\n");
#endif // TKEYVFS_TRACE
        break;
      case SQLITE_ACCESS_READ:
#if TKEYVFS_TRACE
        fprintf(stderr, "op: SQLITE_ACCESS_READ\n");
#endif // TKEYVFS_TRACE
        break;
      default:
        assert(!"Invalid flags argument");
    }
    *pResOut = 0;
#if TKEYVFS_TRACE
    fprintf(stderr, "End   unixAccess ...\n");
#endif // TKEYVFS_TRACE
    return SQLITE_OK;
  }

  /*
  ** Turn a relative pathname into a full pathname. The relative path
  ** is stored as a nul-terminated string in the buffer pointed to by
  ** zPath.
  **
  ** zOut points to a buffer of at least sqlite3_vfs.mxPathname bytes
  ** (in this case, max_pathname bytes). The full-path is written to
  ** this buffer before returning.
  */
  int
  unixFullPathname(sqlite3_vfs* pVfs [[maybe_unused]], //  Pointer to vfs object
                   char const* zPath, //  Possibly relative input path
                   int nOut,          //  Size of output buffer in bytes
                   char* zOut         //  Output buffer
  )
  {
#if TKEYVFS_TRACE
    fprintf(stderr, "Begin unixFullPathName ...\n");
    if (zPath != nullptr) {
      fprintf(stderr, "filename: %s\n", zPath);
    }
#endif // TKEYVFS_TRACE
    assert(pVfs->mxPathname == max_pathname);
    zOut[nOut - 1] = '\0';
    sqlite3_snprintf(nOut, zOut, "%s", zPath);
#if TKEYVFS_TRACE
    fprintf(stderr, "End   unixFullPathName ...\n");
#endif // TKEYVFS_TRACE
    return SQLITE_OK;
  }

  // Interfaces for opening a shared library, finding entry points
  // within the shared library, and closing the shared library.
  void*
  unixDlOpen(sqlite3_vfs*, char const* zFilename)
  {
    Trace tr{"unixFullPathName"};
    void* p = dlopen(zFilename, RTLD_NOW | RTLD_GLOBAL);
    return p;
  }

  /*
  ** SQLite calls this function immediately after a call to unixDlSym() or
  ** unixDlOpen() fails (returns a null pointer). If a more detailed error
  ** message is available, it is written to zBufOut. If no error message
  ** is available, zBufOut is left unmodified and SQLite uses a default
  ** error message.
  */
  void
  unixDlError(sqlite3_vfs*, int nBuf, char* zBufOut)
  {
    Trace tr{"unixDlError"};
    char const* zErr = dlerror();
    if (zErr) {
      sqlite3_snprintf(nBuf, zBufOut, "%s", zErr);
    }
  }

  void (*unixDlSym(sqlite3_vfs*, void* p, char const* zSym))()
  {
    Trace tr{"unixDlSym"};
    /*
    ** GCC with -pedantic-errors says that C90 does not allow a void* to be
    ** cast into a pointer to a function.  And yet the library dlsym() routine
    ** returns a void* which is really a pointer to a function.  So how do we
    ** use dlsym() with -pedantic-errors?
    **
    ** Variable x below is defined to be a pointer to a function taking
    ** parameters void* and char const* and returning a pointer to a function.
    ** We initialize x by assigning it a pointer to the dlsym() function.
    ** (That assignment requires a cast.)  Then we call the function that
    ** x points to.
    **
    ** This work-around is unlikely to work correctly on any system where
    ** you really cannot cast a function pointer into void*.  But then, on the
    ** other hand, dlsym() will not work on such a system either, so we have
    ** not really lost anything.
    */
    auto x = (void (*(*)(void*, char const*))())dlsym;
    return (*x)(p, zSym);
  }

  void
  unixDlClose(sqlite3_vfs*, void* pHandle)
  {
    Trace tr{"unixDlClose"};
    dlclose(pHandle);
  }

  // Write nBuf bytes of random data to the supplied buffer zBuf.
  int
  unixRandomness(sqlite3_vfs*, int nBuf, char* zBuf)
  {
    assert((size_t)nBuf >= (sizeof(time_t) + sizeof(int)));
#if TKEYVFS_TRACE
    fprintf(stderr, "Begin unixRandomness ...\n");
#endif // TKEYVFS_TRACE
    // We have to initialize zBuf to prevent valgrind from reporting
    // errors.  The reports issued by valgrind are incorrect - we would
    // prefer that the randomness be increased by making use of the
    // uninitialized space in zBuf - but valgrind errors tend to worry
    // some users.  Rather than argue, it seems easier just to initialize
    // the whole array and silence valgrind, even if that means less randomness
    // in the random seed.
    //
    // When testing, initializing zBuf[] to zero is all we do.  That means
    // that we always use the same random number sequence.  This makes the
    // tests repeatable.
    std::memset(zBuf, 0, nBuf);
    {
      int fd = robust_open("/dev/urandom", O_RDONLY, 0);
      if (fd < 0) {
        time_t t;
        time(&t);
        std::memcpy(zBuf, &t, sizeof(t));
        int pid = getpid();
        std::memcpy(&zBuf[sizeof(t)], &pid, sizeof(pid));
        assert(sizeof(t) + sizeof(pid) <= (size_t)nBuf);
        nBuf = sizeof(t) + sizeof(pid);
      } else {
        do {
          nBuf = read(fd, zBuf, nBuf);
        } while (nBuf < 0 && errno == EINTR);
        robust_close(0, fd, __LINE__);
      }
    }
#if TKEYVFS_TRACE
    fprintf(stderr, "End   unixRandomness ...\n");
#endif // TKEYVFS_TRACE
    return nBuf;
  }

  // Sleep for a little while.  Return the amount of time slept.
  // The argument is the number of microseconds we want to sleep.
  // The return value is the number of microseconds of sleep actually
  // requested from the underlying operating system, a number which
  // might be greater than or equal to the argument, but not less
  // than the argument.
  int
  unixSleep(sqlite3_vfs*, int const microseconds)
  {
    Trace tr{"unixSleep"};
    usleep(microseconds);
    return microseconds;
  }

  // Find the current time (in Universal Coordinated Time).  Write the
  // current time and date as a Julian Day number into *prNow and
  // return 0.  Return 1 if the time and date cannot be found.
  int
  unixCurrentTime(sqlite3_vfs*, double* prNow)
  {
    Trace tr{"unixCurrentTime"};
    sqlite3_int64 i;
    unixCurrentTimeInt64(0, &i);
    *prNow = i / 86400000.0;
    return 0;
  }

  // We added the xGetLastError() method with the intention of providing
  // better low-level error messages when operating-system problems come up
  // during SQLite operation.  But so far, none of that has been implemented
  // in the core.  So this routine is never called.  For now, it is merely
  // a place-holder.
  int
  unixGetLastError(sqlite3_vfs*, int, char*)
  {
    Trace tr{"unixGetLastError"};
    return 0;
  }

#ifndef TKEYVFS_NO_ROOT
  class RootFileSentry {
  public:
    explicit RootFileSentry(TFile* const fPtr) noexcept { gRootFile = fPtr; }
    ~RootFileSentry() noexcept { gRootFile = nullptr; }
  };
#endif
}

extern "C" {
int
tkeyvfs_init()
{
  static sqlite3_vfs vfs{
    1,                      // iVersion
    sizeof(unixFile),       // szOsFile
    max_pathname,           // mxPathname
    0,                      // pNext
    "tkeyvfs",              // zName
    (void*)&nolockIoFinder, // pAppData
    unixOpen,               // xOpen
    unixDelete,             // xDelete
    unixAccess,             // xAccess
    unixFullPathname,       // xFullPathname
    unixDlOpen,             // xDlOpen
    unixDlError,            // xDlError
    unixDlSym,              // xDlSym
    unixDlClose,            // xDlClose
    unixRandomness,         // xRandomness
    unixSleep,              // xSleep
    unixCurrentTime,        // xCurrentTime
    unixGetLastError,       // xGetLastError
    nullptr,                // v2, xCurrentTimeInt64
    nullptr,                // v3, xSetSystemCall
    nullptr,                // v3, xGetSystemCall
    nullptr                 // v3, xNextSystemCall
  };
  sqlite3_vfs_register(&vfs, 0);
  return SQLITE_OK;
}

int
tkeyvfs_open_v2_noroot(char const* filename, // Database filename (UTF-8)
                       sqlite3** ppDb,       // OUT: SQLite db handle
                       int const flags       // Flags
)
{
  return sqlite3_open_v2(filename, ppDb, flags, nullptr);
}

int
tkeyvfs_open_v2(char const* filename, // Database filename (UTF-8)
                sqlite3** ppDb,       // OUT: SQLite db handle
                int const flags,      // Flags
                TFile* rootFile // IN-OUT: Root file, must be already open.
)
{
  RootFileSentry rfs{rootFile};
  // Note that the sentry *is* the correct thing to do, here:
  // gRootFile is required in unixOpen(), which is called as part of
  // the chain of functions of which sqlite3_open_v2() is the first
  // call. By the time we return from sqlite3_open_v2() then, we no
  // longer require gRootFile and the sentry can do the job of
  // cleaning up when it goes out of scope.
  return sqlite3_open_v2(filename, ppDb, flags, "tkeyvfs");
}
}
