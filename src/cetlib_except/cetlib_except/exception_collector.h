#ifndef cetlib_except_exception_collector_h
#define cetlib_except_exception_collector_h

// ======================================================================
//
// exception_collector: Ensure that each function/functor in a sequence
//                      of calls is invoked even if a previous function
//                      throws.
//
// Each function/functor must take no arguments and return a void.  The
// exception strings are saved in a cms::Exception for optional rethrow.
// Example:
//
//   exception_collector ec;
//
//   ec.call( bind(&MyClass::myFunction, myClassPtr) );
//   ec.call( bind(&MyClass::myOtherFunction, myClassPtr, myArgPtr) );
//   ec.call( bind(&myFreeFunction, myArgPtr) );
//   if( ec.has_thrown() ) ec.rethrow();
//
// All three functions will be called before any exception escapes.
//
// ======================================================================

#include "cetlib_except/exception.h"

#include <functional>

// ======================================================================

namespace cet {

  class exception_collector {
  public:
    ~exception_collector() noexcept(false);

    // observer:
    bool has_thrown() const;

    // mutators:
    void rethrow();
    void call(std::function<void(void)>);

  private:
    exception exception_{std::string{}};
    bool has_thrown_{false};
  };

} // namespace cet

  // ======================================================================

#endif /* cetlib_except_exception_collector_h */

// Local Variables:
// mode: c++
// End:
