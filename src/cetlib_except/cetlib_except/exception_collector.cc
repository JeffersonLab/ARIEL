// ======================================================================
//
// exception_collector: Ensure that each function/functor in a sequence
//                      of calls is invoked even if a previous function
//                      throws.
//
// ======================================================================

#include "cetlib_except/exception_collector.h"

using namespace cet;

// ======================================================================

exception_collector::~exception_collector() noexcept(false)
{
  rethrow();
}

// ----------------------------------------------------------------------

bool
exception_collector::has_thrown() const
{
  return has_thrown_;
}

void
exception_collector::rethrow()
{
  if (has_thrown_) {
    has_thrown_ = false;
    throw exception_;
  }
}

void
exception_collector::call(std::function<void(void)> f)
{
  try {
    f();
  }
  catch (cet::exception const& e) {
    has_thrown_ = true;
    exception_ << e;
  }
  catch (std::exception const& e) {
    has_thrown_ = true;
    exception_ << e.what();
  }
  catch (...) {
    has_thrown_ = true;
    exception_ << "Unknown exception";
  }
} // call()

// ======================================================================
