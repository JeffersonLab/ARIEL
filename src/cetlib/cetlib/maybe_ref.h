#ifndef cetlib_maybe_ref_h
#define cetlib_maybe_ref_h

// ======================================================================
//
// maybe_ref<>: Reference-like types that may refer to nothing
//
// A maybe_ref<> object has the following members of special interest:
//   - isValid() : does it have a referent?
//   - ref() : produce a C++ reference to its referent (iff it is valid)
//
// When valid, a maybe_ref<> object mimics a C++ reference in that it
// takes no ownership of its referent, thus is unaware of the referent's
// lifetime.  Therefore it is the user's responsibility to keep the
// referent alive so long as there is a valid maybe_ref<> to it.
//
// Unlike a C++ reference, a maybe_ref<> object can be reseated (made to
// refer to a different referent).
//
// When invalid, a maybe_ref<> object throws if asked to produce a C++
// reference.
//
// One typical usage pattern:
//   cet::maybe_ref<T const> m( call some accessor );
//   if ( !m ) return;  // or continue
//   T const & r( m.ref() );
//
// ======================================================================

#include <ostream>
#include <stdexcept>

namespace cet {
  template <class T>
  class maybe_ref;

  template <class T>
  void swap(maybe_ref<T>&, maybe_ref<T>&);

  template <class T>
  bool operator==(maybe_ref<T> const& left, maybe_ref<T> const& right);

  template <class T>
  std::ostream& operator<<(std::ostream& os, maybe_ref<T> const& item);
}

// ======================================================================

template <class T>
class cet::maybe_ref {
public:
  typedef T value_type;

  maybe_ref() : ptr_(0) {}
  explicit maybe_ref(T& t) : ptr_(&t) {}

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  bool
  isValid() const
  {
    return ptr_;
  }
  explicit operator bool() const { return isValid(); }

  void
  reseat()
  {
    ptr_ = 0;
  }
  void
  reseat(T& p)
  {
    ptr_ = &p;
  }

  T&
  ref()
  {
    return ref_iff_valid();
  }
  T const&
  ref() const
  {
    return ref_iff_valid();
  }

  void
  swap(maybe_ref& other)
  {
    std::swap(ptr_, other.ptr_);
  }

private:
  T* ptr_;

  T&
  ref_iff_valid() const
  {
    if (isValid())
      return *ptr_;

    // TODO: consider throwing a cet::exception
    throw std::logic_error("cet::maybe_ref<>: referent does not exist");
  }

}; // maybe_ref<>

// ======================================================================

template <class T>
inline void
cet::swap(maybe_ref<T>& r1, maybe_ref<T>& r2)
{
  r1.swap(r2);
}

template <class T>
inline bool
cet::operator==(maybe_ref<T> const& left, maybe_ref<T> const& right)
{
  return (&left.ref()) == (&right.ref());
}

template <class T>
inline std::ostream&
cet::operator<<(std::ostream& os, maybe_ref<T> const& item)
{
  os << &item.ref();
  return os;
}

  // ======================================================================

#endif /* cetlib_maybe_ref_h */

// Local Variables:
// mode: c++
// End:
