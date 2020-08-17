#ifndef cetlib_compiler_macros_h
#define cetlib_compiler_macros_h

////////////////////////////////////////////////////////////////////////
// Define GCC and Clang version tests:
////////////////////////////////////////////////////////////////////////

#ifndef GCC_VERSION
#if defined __GNUC_PATCHLEVEL__
#define GCC_VERSION                                                            \
  (100000 * __GNUC__ + 1000 * __GNUC_MINOR__ + __GNUC_PATCHLEVEL__)
#elif defined __GNUC_MINOR__
#define GCC_VERSION (10000 * __GNUC__ + 1000 * __GNUC_MINOR__)
#elif defined __GNUC__
#define GCC_VERSION (10000 * __GNUC__)
#else
#define GCC_VERSION 0
#endif
#endif

#ifndef CLANG_VERSION
#if defined __clang_patchlevel__
#define CLANG_VERSION                                                          \
  (100000 * __clang_major__ + 1000 * __clang_minor__ + __clang_patchlevel__)
#elif defined __clang_minor__
#define CLANG_VERSION (10000 * __clang_major__ + 1000 * __clang_minor__)
#elif defined __clang_major__
#define CLANG_VERSION (10000 * __clang_major__)
#else
#define CLANG_VERSION 0
#endif
#endif

#ifndef GCC_IS_AT_LEAST
#ifndef __clang__
#define GCC_IS_AT_LEAST(major, minor, patch)                                   \
  GCC_VERSION >= ((100000 * (major)) + (1000 * (minor)) + (patch))
#else
#define GCC_IS_AT_LEAST(major, minor, patch) 0
#endif
#endif

#ifdef __apple_build_version__
#ifndef APPLE_CLANG_IS_AT_LEAST
#define APPLE_CLANG_IS_AT_LEAST(major, minor, patch)                           \
  CLANG_VERSION >= ((100000 * (major)) + (1000 * (minor)) + (patch))
#endif
#ifndef CLANG_IS_AT_LEAST
#define CLANG_IS_AT_LEAST(major, minor, patch) 0
#endif
#else
#ifndef CLANG_IS_AT_LEAST
#define CLANG_IS_AT_LEAST(major, minor, patch)                                 \
  CLANG_VERSION >= ((100000 * (major)) + (1000 * (minor)) + (patch))
#endif
#ifndef APPLE_CLANG_IS_AT_LEAST
#define APPLE_CLANG_IS_AT_LEAST(major, minor, patch) 0
#endif
#endif

////////////////////////////////////////////////////////////////////////
// Define macros EXTERN_C_FUNC_DECLARE_START and
// EXTERN_C_FUNC_DECLARE_END to allow for C++ return types on extern "C"
// functions.
////////////////////////////////////////////////////////////////////////

#ifndef EXTERN_C_FUNC_DECLARE_START_DETAIL
#define EXTERN_C_FUNC_DECLARE_START_DETAIL extern "C" {

#ifdef __clang__
#define EXTERN_C_FUNC_DECLARE_START                                            \
  _Pragma("clang diagnostic push")                                             \
    _Pragma("clang diagnostic ignored \"-Wreturn-type-c-linkage\"")            \
      EXTERN_C_FUNC_DECLARE_START_DETAIL
#define EXTERN_C_FUNC_DECLARE_END                                              \
  }                                                                            \
  _Pragma("clang diagnostic pop")
#else
#define EXTERN_C_FUNC_DECLARE_START EXTERN_C_FUNC_DECLARE_START_DETAIL
#define EXTERN_C_FUNC_DECLARE_END }
#endif
#endif

////////////////////////////////////////////////////////////////////////
// Define FALLTHROUGH macro to allow case fallthrough.
////////////////////////////////////////////////////////////////////////

#ifndef FALLTHROUGH
#if __cplusplus >= 201703L
#if __has_cpp_attribute(fallthrough)
#define FALLTHROUGH [[fallthrough]]
#else
#define FALLTHROUGH [[gnu::fallthrough]]
#endif
#else
#define FALLTHROUGH while (0)
#endif
#endif

////////////////////////////////////////////////////////////////////////
// Define IGNORE_FALLTHROUGH_START and IGNORE_FALLTHROUGH_END to ignore
// implicit fallthrough warnings (e.g.) in included headers.
////////////////////////////////////////////////////////////////////////

#ifndef IGNORE_FALLTHROUGH_START
#if defined(__clang__) || GCC_IS_AT_LEAST(7, 1, 0)
#define IGNORE_FALLTHROUGH_START                                               \
  _Pragma("GCC diagnostic push")                                               \
    _Pragma("GCC diagnostic ignored \"-Wimplicit-fallthrough\"")
#define IGNORE_FALLTHROUGH_END _Pragma("GCC diagnostic pop")
#else
#define IGNORE_FALLTHROUGH_START
#define IGNORE_FALLTHROUGH_END
#endif
#endif

////////////////////////////////////////////////////////////////////////
// Define UNUSED_PRIVATE_FIELD to label a private unused class data
// member.
////////////////////////////////////////////////////////////////////////

#ifndef UNUSED_PRIVATE_FIELD
#ifdef __clang__
#if __has_cpp_attribute(maybe_unused)
#define UNUSED_PRIVATE_FIELD [[maybe_unused]]
#else
#define UNUSED_PRIVATE_FIELD [[gnu::unused]]
#endif
#else
#define UNUSED_PRIVATE_FIELD
#endif
#endif

#endif /* cetlib_compiler_macros_h */

// Local Variables:
// mode: c++
// End:
