#ifndef hep_concurrency_Likely_h
#define hep_concurrency_Likely_h
#include "hep_concurrency/GCCPrerequisite.h"

#if GCC_PREREQUISITE(3, 0, 0)

#if defined(NO_LIKELY)
#define likely(x) (x)
#define unlikely(x) (x)
#elif defined(REVERSE_LIKELY)
#define unlikely(x) (__builtin_expect(x, true))
#define likely(x) (__builtin_expect(x, false))
#else
#define likely(x) (__builtin_expect(x, true))
#define unlikely(x) (__builtin_expect(x, false))
#endif

#else
#define NO_LIKELY
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#endif /* hep_concurrency_Likely_h */
