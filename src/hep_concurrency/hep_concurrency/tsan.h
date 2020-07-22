#ifndef hep_concurrency_tsan_h
#define hep_concurrency_tsan_h
// vim: set sw=2 expandtab :

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <thread>

#if defined(__SANITIZE_THREAD__)
#define ANNOTATE_HAPPENS_BEFORE(addr)                                          \
  AnnotateHappensBefore(__FILE__, __LINE__, (void*)(addr))
#define ANNOTATE_HAPPENS_AFTER(addr)                                           \
  AnnotateHappensAfter(__FILE__, __LINE__, (void*)(addr))
#define ANNOTATE_BENIGN_RACE_SIZED(addr, size, desc)                           \
  AnnotateBenignRaceSized(                                                     \
    __FILE__, __LINE__, (void*)(addr), (unsigned long)(size), (desc))
#define ANNOTATE_BENIGN_RACE(addr, desc)                                       \
  AnnotateBenignRace(__FILE__, __LINE__, (void*)(addr), (desc))
#define ANNOTATE_THREAD_IGNORE_READS_BEGIN                                     \
  AnnotateIgnoreReadsBegin(__FILE__, __LINE__)
#define ANNOTATE_THREAD_IGNORE_READS_END                                       \
  AnnotateIgnoreReadsEnd(__FILE__, __LINE__)
#define ANNOTATE_THREAD_IGNORE_WRITES_BEGIN                                    \
  AnnotateIgnoreWritesBegin(__FILE__, __LINE__)
#define ANNOTATE_THREAD_IGNORE_WRITES_END                                      \
  AnnotateIgnoreWritesEnd(__FILE__, __LINE__)
#define ANNOTATE_THREAD_IGNORE_SYNC_BEGIN                                      \
  AnnotateIgnoreSyncBegin(__FILE__, __LINE__)
#define ANNOTATE_THREAD_IGNORE_SYNC_END                                        \
  AnnotateIgnoreSyncEnd(__FILE__, __LINE__)
#define ANNOTATE_THREAD_IGNORE_BEGIN                                           \
  ANNOTATE_THREAD_IGNORE_READS_BEGIN;                                          \
  ANNOTATE_THREAD_IGNORE_WRITES_BEGIN;                                         \
  ANNOTATE_THREAD_IGNORE_SYNC_BEGIN
#define ANNOTATE_THREAD_IGNORE_END                                             \
  ANNOTATE_THREAD_IGNORE_READS_END;                                            \
  ANNOTATE_THREAD_IGNORE_WRITES_END;                                           \
  ANNOTATE_THREAD_IGNORE_SYNC_END
extern "C" {
void AnnotateHappensBefore(const char*, int, void*);
void AnnotateHappensAfter(const char*, int, void*);
void AnnotateBenignRaceSized(const char*,
                             int,
                             void*,
                             unsigned long,
                             const char*);
void AnnotateBenignRace(const char*, int, void*, const char*);
void AnnotateIgnoreReadsBegin(const char*, int);
void AnnotateIgnoreReadsEnd(const char*, int);
void AnnotateIgnoreWritesBegin(const char*, int);
void AnnotateIgnoreWritesEnd(const char*, int);
void AnnotateIgnoreSyncBegin(const char*, int);
void AnnotateIgnoreSyncEnd(const char*, int);
} // extern "C"
#else // defined(__SANITIZE_THREAD__)
#define ANNOTATE_HAPPENS_BEFORE(addr)
#define ANNOTATE_HAPPENS_AFTER(addr)
#define ANNOTATE_BENIGN_RACE_SIZED(addr, size, desc)
#define ANNOTATE_BENIGN_RACE(addr, desc)
#define ANNOTATE_THREAD_IGNORE_READS_BEGIN
#define ANNOTATE_THREAD_IGNORE_READS_END
#define ANNOTATE_THREAD_IGNORE_WRITES_BEGIN
#define ANNOTATE_THREAD_IGNORE_WRITES_END
#define ANNOTATE_THREAD_IGNORE_SYNC_BEGIN
#define ANNOTATE_THREAD_IGNORE_SYNC_END
#define ANNOTATE_THREAD_IGNORE_BEGIN
#define ANNOTATE_THREAD_IGNORE_END
#endif // defined(__SANITIZE_THREAD__)

namespace hep {
  namespace concurrency {

    extern int intentionalDataRace_;
    std::thread::id getThreadID();
    unsigned long long getTSC();
    unsigned long long getTSCP(unsigned& cpuidx);

  } // namespace concurrency
} // namespace hep

#define INTENTIONAL_DATA_RACE(ENV_VAR)                                         \
  {                                                                            \
    if (std::getenv(#ENV_VAR)) {                                               \
      std::ostringstream buf;                                                  \
      buf << "-----> Making data race " #ENV_VAR " "                           \
          << "tid: " << hep::concurrency::getThreadID() << " " << __FILE__     \
          << ":" << __LINE__ << "\n";                                          \
      hep::concurrency::intentionalDataRace_ =                                 \
        hep::concurrency::intentionalDataRace_ + 1;                            \
      std::cerr << buf.str();                                                  \
    }                                                                          \
  }

  // Local Variables:
  // mode: c++
  // End:

#endif /* hep_concurrency_tsan_h */
