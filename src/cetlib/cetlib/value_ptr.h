#ifndef cetlib_value_ptr_h
#define cetlib_value_ptr_h

// ======================================================================
//
// value_ptr: A pointer treating its pointee as-if contained by value
//
// ----------------------------------------------------------------------
//
// This smart pointer template mimics value semantics for its pointee:
//   - the pointee lifetime matches the pointer lifetime, and
//   - the pointee is copied whenever the pointer is copied.
//
// Having such a template provides a standard vocabulary to denote such
// pointers, with no need for further comment or other documentation to
// describe the semantics involved.
//
// As a small bonus, this template's c'tors ensure that all instance
// variables are initialized.
//
// ----------------------------------------------------------------------
//
// Originally inspired by Item 31 in
//   Herb Sutter:  _More Exceptional C++_, Addison-Wesley, 2002.
//   ISBN 0-201-70434-X
// and its predecessor
//   Herb Sutter:  Smart Pointer Members.  GotW #62, undated.
//   http://www.gotw.ca/gotw/062.htm
// and independently by
//   Alan Griffiths:  "Ending with the grin," 1999.
//   URL http://www.octopull.demon.co.uk/arglib/TheGrin.html
// and later by
//   Axter (David Maisonave):  Clone Smart Pointer (clone_ptr). 2005.
//   http://www.codeguru.com/cpp/cpp/algorithms/general/article.php/c10407
//
// ----------------------------------------------------------------------
//
// Alternative names for consideration (shown alphabetically):
//   clone_ptr, cloned_ptr, cloning_ptr, copycat_ptr, copied_ptr,
//   copying_ptr, dup_ptr, duplicate_ptr, duplicating_ptr,
//   matched_ptr, matching_ptr, replicating_ptr, twin_ptr, twinning_ptr
//
// ----------------------------------------------------------------------
//
// Questions:
//   - Should value_ptr be specialized to work with array types a la
//     unique_ptr?
//   - Should value_ptr take an allocator argument in addition to a
//     cloner and a deleter?  (Only the cloner would use the allocator.)
//   - This implementation assumes the cloner and deleter types are
//     stateless; are these viable assumptions?
//   - If cloners and deleters are allowed to be stateful, what policies
//     should apply when they are being copied during a value_ptr copy?
//   - We have operators ==, !=, < ; should <=  >  >=  be supported?
//   - With which, if any, other smart pointers should this template
//     innately interoperate, and to what degree?
//
// ======================================================================

#include <cstddef>
#include <exception>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace cet {
  namespace _ {
    template <class T>
    struct has_clone;

    template <
      class Element,
      bool = std::is_polymorphic<Element>::value&& _::has_clone<Element>::value>
    struct default_action;

    template <class Element>
    struct default_action<Element, false>;
  }

  template <class Element>
  struct default_copy;
  template <class Element>
  struct default_clone;

  template <class Element,
            class Cloner = _::default_action<Element>,
            class Deleter = std::default_delete<Element>>
  class value_ptr;

  template <class E, class C, class D>
  void swap(value_ptr<E, C, D>&, value_ptr<E, C, D>&) noexcept;

  template <class E, class C, class D>
  bool operator==(value_ptr<E, C, D> const&, value_ptr<E, C, D> const&);
  template <class E, class C, class D>
  bool operator!=(value_ptr<E, C, D> const&, value_ptr<E, C, D> const&);

  template <class E, class C, class D>
  bool operator==(value_ptr<E, C, D> const&, std::nullptr_t const&);
  template <class E, class C, class D>
  bool operator!=(value_ptr<E, C, D> const&, std::nullptr_t const&);

  template <class E, class C, class D>
  bool operator==(std::nullptr_t const&, value_ptr<E, C, D> const&);
  template <class E, class C, class D>
  bool operator!=(std::nullptr_t const&, value_ptr<E, C, D> const&);

  template <class E, class C, class D>
  bool operator<(value_ptr<E, C, D> const&, value_ptr<E, C, D> const&);

  template <class E, class C, class D>
  bool operator>(value_ptr<E, C, D> const&, value_ptr<E, C, D> const&);

  template <class E, class C, class D>
  bool operator<=(value_ptr<E, C, D> const&, value_ptr<E, C, D> const&);

  template <class E, class C, class D>
  bool operator>=(value_ptr<E, C, D> const&, value_ptr<E, C, D> const&);
}

// ======================================================================

template <class T>
struct cet::_::has_clone {
private:
  typedef char (&yes_t)[1];
  typedef char (&no_t)[2];

  template <class U, U* (U::*)() const = &U::clone>
  struct cloneable {
  };

  template <class U>
  static yes_t test(cloneable<U>*);
  template <class>
  static no_t test(...);

public:
  static bool constexpr value{sizeof(test<T>(0)) == sizeof(yes_t)};
}; // has_clone<>

// ----------------------------------------------------------------------

template <class Element>
struct cet::default_copy {
public:
  Element*
  operator()(Element* p) const
  {
    return new Element{*p};
  }
}; // default_copy<>

// ----------------------------------------------------------------------

template <class Element>
struct cet::default_clone {
public:
  Element*
  operator()(Element* p) const
  {
    return p->clone();
  }

}; // default_clone<>

// ----------------------------------------------------------------------

template <class Element, bool>
struct cet::_::default_action : public default_clone<Element> {
public:
  using default_clone<Element>::operator();

}; // default_action<>

template <class Element>
struct cet::_::default_action<Element, false> : public default_copy<Element> {
public:
  using default_copy<Element>::operator();

}; // default_action<>

// ----------------------------------------------------------------------

template <class Element, class Cloner, class Deleter>
class cet::value_ptr {
public:
  // --- publish our template parameters and variations thereof:
  using element_type = Element;
  using cloner_type = Cloner;
  using deleter_type = Deleter;
  using pointer = typename std::add_pointer<Element>::type;
  // TODO: use Deleter::pointer, if it exists, in lieu of above
  using reference = typename std::add_lvalue_reference<Element>::type;

private:
  template <class P>
  struct is_compatible
    : public std::is_convertible<typename std::add_pointer<P>::type, pointer> {
  };

public:
  // default c'tor:
  constexpr value_ptr() noexcept : p{nullptr} {}

  // ownership-taking c'tors:
  constexpr value_ptr(std::nullptr_t) noexcept : p{nullptr} {}

  template <class E2>
  explicit value_ptr(E2* other) noexcept : p{other}
  {
    static_assert(is_compatible<E2>::value,
                  "value_ptr<>'s pointee type is incompatible!");
    static_assert(
      !std::is_polymorphic<E2>::value ||
        !(std::is_same<Cloner, _::default_action<Element, false>>::value),
      "value_ptr<>'s pointee type would slice when copying!");
  }

  // copying c'tors:
  value_ptr(value_ptr const& other) : p{clone_from(other.p)} {}

  template <class E2>
  value_ptr(value_ptr<E2, Cloner, Deleter> const& other,
            std::enable_if_t<is_compatible<E2>::value>* = nullptr)
    : p{clone_from(other.p)}
  {}

  // moving c'tors:
  value_ptr(value_ptr&& other) noexcept : p{other.release()} {}

  template <class E2>
  value_ptr(value_ptr<E2, Cloner, Deleter>&& other,
            std::enable_if_t<is_compatible<E2>::value>* = nullptr) noexcept
    : p(other.release())
  {}

  // d'tor:
  ~value_ptr() noexcept { reset(); }

  // copying assignments:
  value_ptr& operator=(std::nullptr_t) noexcept
  {
    reset(nullptr);
    return *this;
  }

  value_ptr&
  operator=(value_ptr const& other)
  {
    value_ptr tmp{other};
    swap(tmp);
    return *this;
  }

  template <class E2>
  std::enable_if_t<is_compatible<E2>::value, value_ptr&>
  operator=(value_ptr<E2, Cloner, Deleter> const& other)
  {
    value_ptr tmp{other};
    swap(tmp);
    return *this;
  }

  // moving assignments:
  value_ptr&
  operator=(value_ptr&& other) noexcept
  {
    value_ptr tmp{std::move(other)};
    swap(tmp);
    return *this;
  }

  template <class E2>
  std::enable_if_t<is_compatible<E2>::value, value_ptr&>
  operator=(value_ptr<E2, Cloner, Deleter>&& other) noexcept
  {
    value_ptr tmp{std::move(other)};
    swap(tmp);
    return *this;
  }

  // observers:
  reference operator*() const { return *get(); }
  pointer operator->() const noexcept { return get(); }
  pointer
  get() const noexcept
  {
    return p;
  }

  explicit operator bool() const noexcept { return get(); }

  // modifiers:
  pointer
  release() noexcept
  {
    pointer old = p;
    p = nullptr;
    return old;
  }
  void
  reset(pointer t = pointer()) noexcept
  {
    std::swap(p, t);
    Deleter()(t);
  }
  void
  swap(value_ptr& other) noexcept
  {
    std::swap(p, other.p);
  }

private:
  pointer p;

  template <class P>
  pointer
  clone_from(P const p) const
  {
    return p ? Cloner()(p) : nullptr;
  }

}; // value_ptr<>

// ======================================================================
// non-member functions:

// ----------------------------------------------------------------------
// non-member swap:

template <class E, class C, class D>
void
cet::swap(value_ptr<E, C, D>& x, value_ptr<E, C, D>& y) noexcept
{
  x.swap(y);
}

// ----------------------------------------------------------------------
// non-member (in)equality comparison:

template <class E, class C, class D>
bool
cet::operator==(value_ptr<E, C, D> const& x, value_ptr<E, C, D> const& y)
{
  return x.get() == y.get();
}

template <class E, class C, class D>
bool
cet::operator!=(value_ptr<E, C, D> const& x, value_ptr<E, C, D> const& y)
{
  return !operator==(x, y);
}

template <class E, class C, class D>
bool
cet::operator==(value_ptr<E, C, D> const& x, std::nullptr_t const& y)
{
  return x.get() == y;
}

template <class E, class C, class D>
bool
cet::operator!=(value_ptr<E, C, D> const& x, std::nullptr_t const& y)
{
  return !operator==(x, y);
}

template <class E, class C, class D>
bool
cet::operator==(std::nullptr_t const& x, value_ptr<E, C, D> const& y)
{
  return x == y.get();
}

template <class E, class C, class D>
bool
cet::operator!=(std::nullptr_t const& x, value_ptr<E, C, D> const& y)
{
  return !operator==(x, y);
}

// ----------------------------------------------------------------------
// non-member ordering:

template <class E, class C, class D>
bool
cet::operator<(value_ptr<E, C, D> const& x, value_ptr<E, C, D> const& y)
{
  using CT = std::common_type_t<typename value_ptr<E, C, D>::pointer,
                                typename value_ptr<E, C, D>::pointer>;
  return std::less<CT>{}(x.get(), y.get());
}

template <class E, class C, class D>
bool
cet::operator>(value_ptr<E, C, D> const& x, value_ptr<E, C, D> const& y)
{
  return y < x;
}

template <class E, class C, class D>
bool
cet::operator<=(value_ptr<E, C, D> const& x, value_ptr<E, C, D> const& y)
{
  return !(y < x);
}

template <class E, class C, class D>
bool
cet::operator>=(value_ptr<E, C, D> const& x, value_ptr<E, C, D> const& y)
{
  return !(x < y);
}

  // ======================================================================

#endif /* cetlib_value_ptr_h */

// Local Variables:
// mode: c++
// End:
