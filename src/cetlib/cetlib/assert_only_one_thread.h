#ifndef cetlib_assert_only_one_thread_h
#define cetlib_assert_only_one_thread_h

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
#include <iostream>
#include <mutex>

namespace cet {
  namespace detail {

    class ThreadCounter {
    public:
      explicit ThreadCounter(char const* filename,
                             unsigned const linenum,
                             char const* funcname)
        : filename_{filename}, linenum_{linenum}, funcname_{funcname}
      {}

      class Sentry; // Only the sentry can access the members

    private:
      std::atomic<unsigned> counter_{0u};
      std::string const filename_;
      unsigned const linenum_;
      std::string const funcname_;
    };

    class ThreadCounter::Sentry {
    public:
      Sentry(ThreadCounter& tc, bool const terminate = true)
        : tc_{tc}, terminate_{terminate}
      {
        if (++tc_.counter_ != 1u) {
          // Do not guard the abort!
          {
            std::lock_guard<decltype(m_)> hold{m_};
            std::cerr
              << "Failed assert--more than one thread accessing location:\n"
              << "  " << tc_.filename_ << ':' << tc_.linenum_ << '\n'
              << "  function: " << tc_.funcname_ << '\n';
          }
          if (terminate_) {
            std::abort();
          }
        }
      }

      ~Sentry() noexcept { --tc_.counter_; }

    private:
      ThreadCounter& tc_;
      bool const terminate_;
      std::mutex m_{};
    };
  }
}

#define CONCATENATE_HIDDEN(a, b) a##b
#define CONCATENATE(a, b) CONCATENATE_HIDDEN(a, b)

#ifndef NDEBUG
#define CET_ASSERT_ONLY_ONE_THREAD()                                           \
  static cet::detail::ThreadCounter CONCATENATE(s, __LINE__){                  \
    __FILE__, __LINE__, __func__};                                             \
  cet::detail::ThreadCounter::Sentry CONCATENATE(hold, __LINE__)               \
  {                                                                            \
    CONCATENATE(s, __LINE__)                                                   \
  }
#else
#define CET_ASSERT_ONLY_ONE_THREAD() ((void)0)
#endif

#endif /* cetlib_assert_only_one_thread_h */

// Local variables:
// mode: c++
// End:
