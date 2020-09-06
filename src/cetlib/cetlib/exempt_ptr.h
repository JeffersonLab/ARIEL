#ifndef cetlib_exempt_ptr_h
#define cetlib_exempt_ptr_h

// ======================================================================
//
// exempt_ptr: A pointer taking no notice of its pointee's lifetime
//             (thus exempt from any role in managing same, and thus
//             copyable with no impact on its pointee)
//
// ----------------------------------------------------------------------
//
// This (not very) smart pointer template is intended as a replacement
// for bare/native/raw/built-in C++ pointers, especially when used to
// communicate with (say) legacy code that traffics in such pointers.
//
// Having such a template provides a standard vocabulary to denote
// non-owning pointers, with no need for further comment or other
// documentation to describe the semantics involved.
//
// As a small bonus, this template's c'tors ensure that all instance
// variables are initialized.
//
// ----------------------------------------------------------------------
//
// Questions:
//   - We have operators ==, !=, < ; should <=  >  >=  be supported?
//   - With which, if any, other smart pointers should this template
//     innately interoperate, and to what degree?
//
// ======================================================================

#include <cstddef>
#include <exception>
#include <functional>
#include <type_traits>
#include <utility>

namespace cet {

  template <class Element>
  class exempt_ptr;

  template <class E>
  void swap(exempt_ptr<E>&, exempt_ptr<E>&) noexcept;

  template <class E>
  exempt_ptr<E> make_exempt_ptr(E*) noexcept;

  template <class E>
  bool operator==(exempt_ptr<E> const&, exempt_ptr<E> const&);

  template <class E>
  bool operator!=(exempt_ptr<E> const&, exempt_ptr<E> const&);

  template <class E>
  bool operator==(exempt_ptr<E> const&, std::nullptr_t const&);

  template <class E>
  bool operator!=(exempt_ptr<E> const&, std::nullptr_t const&);

  template <class E>
  bool operator==(std::nullptr_t const&, exempt_ptr<E> const&);

  template <class E>
  bool operator!=(std::nullptr_t const&, exempt_ptr<E> const&);

  template <class E>
  bool operator<(exempt_ptr<E> const&, exempt_ptr<E> const&);

  template <class E>
  bool operator>(exempt_ptr<E> const&, exempt_ptr<E> const&);

  template <class E>
  bool operator<=(exempt_ptr<E> const&, exempt_ptr<E> const&);

  template <class E>
  bool operator>=(exempt_ptr<E> const&, exempt_ptr<E> const&);
}

// ======================================================================

template <class Element>
class cet::exempt_ptr {
public:
  // --- publish our template parameter and variations thereof:
  using element_type = Element;
  using pointer = std::add_pointer_t<Element>;
  using reference = std::add_lvalue_reference_t<Element>;

private:
  template <class P>
  struct is_compatible
    : public std::is_convertible<std::add_pointer_t<P>, pointer> {};

  template <class P>
  static constexpr bool is_compatible_v = is_compatible<P>::value;

public:
  // --- default c'tor:
  constexpr exempt_ptr() noexcept : p{nullptr} {}

  // pointer-accepting c'tors:
  constexpr exempt_ptr(std::nullptr_t) noexcept : p{nullptr} {}
  explicit exempt_ptr(pointer other) noexcept : p{other} {}

  template <class E2>
  exempt_ptr(E2* other,
             std::enable_if_t<is_compatible_v<E2>>* = nullptr) noexcept
    : p{other}
  {}

  // copying c'tors:
  // use compiler-generated copy c'tor
  template <class E2>
  exempt_ptr(exempt_ptr<E2> const& other,
             std::enable_if_t<is_compatible_v<E2>>* = nullptr) noexcept
    : p{other.get()}
  {}

  // pointer-accepting assignments:
  exempt_ptr& operator=(std::nullptr_t) noexcept
  {
    reset(nullptr);
    return *this;
  }

  template <class E2>
  std::enable_if_t<is_compatible_v<E2>, exempt_ptr&>
  operator=(E2* other) noexcept
  {
    reset(other);
    return *this;
  }

  // copying assignments:
  // use compiler-generated copy assignment
  template <class E2>
  std::enable_if_t<is_compatible_v<E2>, exempt_ptr&>
  operator=(exempt_ptr<E2> const& other) noexcept
  {
    reset(other.get());
    return *this;
  }

  // observers:
  reference operator*() const noexcept { return *get(); }
  pointer operator->() const noexcept { return get(); }
  pointer
  get() const noexcept
  {
    return p;
  }
  bool
  empty() const noexcept
  {
    return get() == nullptr;
  }
  explicit operator bool() const noexcept { return !empty(); }

  // modifiers:
  pointer
  release() noexcept
  {
    pointer old = get();
    reset();
    return old;
  }
  void
  reset(pointer t = nullptr) noexcept
  {
    p = t;
  }
  void
  swap(exempt_ptr& other) noexcept
  {
    std::swap(p, other.p);
  }

private:
  pointer p;

}; // exempt_ptr<>

// ======================================================================
// non-member functions:

// ----------------------------------------------------------------------
// non-member swap:

template <class E>
inline void
cet::swap(exempt_ptr<E>& x, exempt_ptr<E>& y) noexcept
{
  x.swap(y);
}

// ----------------------------------------------------------------------
// non-member make_exempt_ptr:

template <class E>
cet::exempt_ptr<E>
cet::make_exempt_ptr(E* p) noexcept
{
  return exempt_ptr<E>(p);
}

// ----------------------------------------------------------------------
// non-member (in)equality comparison:

template <class E>
bool
cet::operator==(cet::exempt_ptr<E> const& x, cet::exempt_ptr<E> const& y)
{
  return x.get() == y.get();
}

template <class E>
bool
cet::operator!=(cet::exempt_ptr<E> const& x, cet::exempt_ptr<E> const& y)
{
  return !operator==(x, y);
}

template <class E>
bool
cet::operator==(exempt_ptr<E> const& x, std::nullptr_t const& y)
{
  return x.get() == y;
}

template <class E>
bool
cet::operator!=(exempt_ptr<E> const& x, std::nullptr_t const& y)
{
  return !operator==(x, y);
}

template <class E>
bool
cet::operator==(std::nullptr_t const& x, exempt_ptr<E> const& y)
{
  return x == y.get();
}

template <class E>
bool
cet::operator!=(std::nullptr_t const& x, exempt_ptr<E> const& y)
{
  return !operator==(x, y);
}

// ----------------------------------------------------------------------
// non-member ordering:

template <class E>
bool
cet::operator<(cet::exempt_ptr<E> const& x, cet::exempt_ptr<E> const& y)
{
  using CT = std::common_type_t<typename exempt_ptr<E>::pointer,
                                typename exempt_ptr<E>::pointer>;
  return std::less<CT>{}(x.get(), y.get());
}

template <class E>
bool
cet::operator>(cet::exempt_ptr<E> const& x, cet::exempt_ptr<E> const& y)
{
  return y < x;
}

template <class E>
bool
cet::operator<=(cet::exempt_ptr<E> const& x, cet::exempt_ptr<E> const& y)
{
  return !(y < x);
}

template <class E>
bool
cet::operator>=(cet::exempt_ptr<E> const& x, cet::exempt_ptr<E> const& y)
{
  return !(x < y);
}

// ======================================================================

#endif /* cetlib_exempt_ptr_h */

// Local Variables:
// mode: c++
// End:
