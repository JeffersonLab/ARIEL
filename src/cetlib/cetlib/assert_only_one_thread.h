#ifndef cetlib_assert_only_one_thread_h
#define cetlib_assert_only_one_thread_h
// vim: set sw=2 expandtab :

// ===================================================================
// CET_ASSERT_ONLY_ONE_THREAD()
//
// This macro is a utility that can be called wherever it is expected
// for only one thread to be accessing a particular block of code.
//
// It has similar semantics to the standard 'assert' macro:
//
//   - It is disabled if NDEBUG is defined
//   - If more than one thread is accessing that block of code at one
//     time, std::abort is called.
//   - It is encouraged to be used whenever single-threaded execution
//     of a code block is a pre-condition.
//
// If the std::abort() is called, file, line-number, and function-name
// information will be provided.
// ===================================================================

#include <atomic>
#include <cstdlib>
#include <iostream>
#include <string>

namespace cet::detail {

  class ThreadCounter {
  public:
    explicit ThreadCounter(char const* filename,
                           unsigned const linenum,
                           char const* funcname)
      : filename_{filename}, linenum_{linenum}, funcname_{funcname}
    {}

    class Sentry {
    public:
      ~Sentry() noexcept { --tc_.counter_; }
      Sentry(ThreadCounter& tc, bool const terminate = true) : tc_{tc}
      {
        if (++tc_.counter_ == 1u) {
          return;
        }
        std::cerr << "Failed assert--more than one thread accessing location:\n"
                  << "  " << tc_.filename_ << ':' << tc_.linenum_ << '\n'
                  << "  function: " << tc_.funcname_ << '\n';
        if (terminate) {
          std::abort();
        }
      }

    private:
      ThreadCounter& tc_;
    };

  private:
    std::string const filename_;
    unsigned const linenum_;
    std::string const funcname_;
    std::atomic<unsigned> counter_{0u};
  };

} // namespace cet::detail

#ifdef NDEBUG
#define CET_ASSERT_ONLY_ONE_THREAD()
#else // NDEBUG
#define CET_ASSERT_ONLY_ONE_THREAD()                                           \
  static cet::detail::ThreadCounter s_tc_{__FILE__, __LINE__, __func__};       \
  cet::detail::ThreadCounter::Sentry sentry_tc_ { s_tc_ }
#endif // NDEBUG

#endif /* cetlib_assert_only_one_thread_h */

// Local variables:
// mode: c++
// End:
