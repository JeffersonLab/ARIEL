#ifndef cetlib_no_delete_h
#define cetlib_no_delete_h

/*
  ======================================================================

  no_delete.h

  !!==========================================================!!
  !! Please consult with the art team before using this class !!
  !!==========================================================!!

  Purpose: no_delete provides a way to use std::shared_ptr or
           std::unique_ptr for those cases where the object may be
           either in dynamic (heap) storage, or in static storage, as
           long as which of these applies is known when the shared_ptr
           or unique_ptr is constructed.

  For objects:

  If the object is allocated in dynamic storage, use
  std::make_shared<T>(...);

  If the object "t" is in static storage, use
  std::shared_ptr<T> (&t, cet::no_delete());

  ======================================================================
*/

namespace cet {
  struct no_delete {
    void
    operator()(void const*) const
    {}
  };
}

#endif /* cetlib_no_delete_h */

// Local variables:
// mode: c++
// End:
