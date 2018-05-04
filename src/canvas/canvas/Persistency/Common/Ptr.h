#ifndef canvas_Persistency_Common_Ptr_h
#define canvas_Persistency_Common_Ptr_h

////////////////////////////////////////////////////////////////////////
// Ptr and related functions.
//
// A Ptr is a persistent smart pointer to an item in a collection where
// the collection is in the art::Event.
//
// How to construct a Ptr<T>:
//
// 1. Default constructor.
//      Ptr();
//
//    Constructs an invalid Ptr.
//
// 2. From a handle to a collection and an index into that collection.
//      template <typename H>
//      Ptr(H const&, key_type);
//
// 3. From a ProductID.
//      Ptr(ProductID); // Invalid ("null") Ptr.
//
//      Ptr(ProductID, key_type, EDProductGetter const*);
//
//    Obtain the ProductID from the collection handle or the result of
//    Event::put(). Obtain the EDProductGetter from the event using the
//    ProductID.
//
// 4. From a Ptr<U> where U is a base or sub class of T.
//      Ptr(Ptr<U> const&);
//
// 5. From a ProductID and an existing resolved pointer.
//      Ptr(ProductID, T const*, key_type)
//
//    This signature is for expert-level or internal use only: it is a
//    pre-condition that the pointer must be the item at the index
//    specified in product with the given ID. Errors will *not* be
//    detected.
//
////////////////////////////////////////////////////////////////////////
#include "canvas/Persistency/Common/EDProduct.h"
#include "canvas/Persistency/Common/EDProductGetter.h"
#include "canvas/Persistency/Common/GetProduct.h"
#include "canvas/Persistency/Common/RefCore.h"
#include "canvas/Persistency/Common/traits.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Utilities/Exception.h"
#include "canvas/Utilities/ensurePointer.h"
#include "cetlib_except/demangle.h"

#include <cassert>
#include <cstddef>
#include <functional> // for std::hash
#include <list>
#include <ostream>
#include <type_traits>
#include <vector>

namespace art {
  template <typename T>
  class Ptr;

  template <typename T, typename U>
  std::enable_if_t<std::is_same<T, U>::value || std::is_base_of<T, U>::value ||
                     std::is_base_of<U, T>::value,
                   bool>
  operator==(Ptr<T> const& lhs, Ptr<U> const& rhs);
  template <typename T, typename U>
  std::enable_if_t<std::is_same<T, U>::value || std::is_base_of<T, U>::value ||
                     std::is_base_of<U, T>::value,
                   bool>
  operator!=(Ptr<T> const& lhs, Ptr<U> const& rhs);
  template <typename T, typename U>
  std::enable_if_t<std::is_same<T, U>::value || std::is_base_of<T, U>::value ||
                     std::is_base_of<U, T>::value,
                   bool>
  operator<(Ptr<T> const& lhs, Ptr<U> const& rhs);

  // Fill a vector of Ptrs from a persistent collection. Alternatively,
  // construct a PtrVector.
  template <typename T, typename H>
  void fill_ptr_vector(std::vector<Ptr<T>>& ptrs, H const& h);

  // Fill a list of Ptrs from a persistent collection.
  template <typename T, typename H>
  void fill_ptr_list(std::list<Ptr<T>>& ptrs, H const& h);

  // Specialize EnsurePointer for Ptr.
  namespace detail {
    template <typename TO, typename PTRVAL>
    struct EnsurePointer<TO, Ptr<PTRVAL>>;
  }

  template <typename T>
  std::ostream& operator<<(std::ostream&, Ptr<T> const&);
}

template <typename T>
class art::Ptr {
public:
  typedef std::size_t key_type;
  typedef T value_type;
  typedef T const* const_pointer;
  typedef T const& const_reference;

  // 1.
  Ptr() = default;

  // 2.
  template <typename H>
  Ptr(H const& handle, key_type idx);

  // 3A.
  explicit Ptr(ProductID const id);

  // 3B.
  Ptr(ProductID const productID,
      key_type itemKey,
      EDProductGetter const* prodGetter);

  // 4.
  template <typename U>
  Ptr(Ptr<U> const& iOther,
      std::enable_if_t<std::is_base_of<T, U>::value>* dummy = nullptr);

  template <typename U>
  Ptr(Ptr<U> const& iOther,
      std::enable_if_t<std::is_base_of<U, T>::value>* dummy = nullptr);

  // 5. See notes above.
  Ptr(ProductID const productID, T const* item, key_type itemKey);

  // Accessors.
  T const& operator*() const;
  T const* operator->() const;
  T const* get() const;
  bool isNull() const; // Checks for valid key.
  bool isNonnull() const;

  // Checks if collection is in memory or available
  // in the event. No type checking is done.
  bool isAvailable() const;

  ProductID id() const;
  key_type key() const;
  bool hasCache() const;
  RefCore const& refCore() const;

  explicit operator bool() const;
  EDProductGetter const* productGetter() const;

  // MUST UPDATE WHEN CLASS IS CHANGED!
  static short
  Class_Version()
  {
    return 10;
  }

private:
  template <typename C>
  T const* getItem_(C const* product, key_type iKey);
  void getData_() const;

  RefCore core_{};
  key_type key_{key_traits<key_type>::value};
};

template <typename TO, typename PTRVAL>
struct art::detail::EnsurePointer<TO, art::Ptr<PTRVAL>> {
  TO operator()(Ptr<PTRVAL>& from) const;
  TO operator()(Ptr<PTRVAL> const& from) const;
};

namespace art {
  namespace detail {
    // FIXME: The code of ItemGetter, including specializations, would
    // be completely unnecessary if Handle were to provide access to
    // the setPtr() function of wrapper. As it is, some
    // container-specific code is duplicated between here and
    // art::Wrapper, leading to multiple points of maintenance (and
    // failure).
    template <typename T, typename C>
    class ItemGetter;
    template <typename T>
    class ItemGetter<T, cet::map_vector<T>>;
    template <typename T>
    class ItemGetter<std::pair<cet::map_vector_key, T>, cet::map_vector<T>>;
  }
}

template <typename T, typename C>
class art::detail::ItemGetter {
public:
  T const* operator()(C const* product, typename Ptr<T>::key_type iKey) const;
};

template <typename T, typename C>
inline T const*
art::detail::ItemGetter<T, C>::operator()(C const* product,
                                          typename Ptr<T>::key_type iKey) const
{
  assert(product != nullptr);
  auto it = product->begin();
  advance(it, iKey);
  T const* address = detail::GetProduct<C>::address(it);
  return address;
}

template <typename T>
class art::detail::ItemGetter<T, cet::map_vector<T>> {
public:
  T const* operator()(cet::map_vector<T> const* product,
                      typename Ptr<T>::key_type iKey) const;
};

template <typename T>
inline T const*
art::detail::ItemGetter<T, cet::map_vector<T>>::operator()(
  cet::map_vector<T> const* product,
  typename Ptr<T>::key_type iKey) const
{
  assert(product != nullptr);
  cet::map_vector_key k(iKey);
  return product->getOrNull(k);
}

template <typename T>
class art::detail::ItemGetter<std::pair<cet::map_vector_key, T>,
                              cet::map_vector<T>> {
public:
  std::pair<cet::map_vector_key, T> const* operator()(
    cet::map_vector<T> const* product,
    typename Ptr<T>::key_type iKey) const;
};

template <typename T>
inline std::pair<cet::map_vector_key, T> const*
art::detail::ItemGetter<std::pair<cet::map_vector_key, T>, cet::map_vector<T>>::
operator()(cet::map_vector<T> const* product,
           typename Ptr<T>::key_type const iKey) const
{
  assert(product != nullptr);
  cet::map_vector_key k(static_cast<unsigned>(iKey));
  auto it = product->find(k);
  if (it == product->end()) {
    return nullptr;
  } else {
    return &(*it);
  }
}

////////////////////////////////////
// Ptr implementation.

template <typename T>
template <typename H>
inline art::Ptr<T>::Ptr(H const& handle, typename Ptr<T>::key_type const idx)
  : core_{handle.id(), getItem_(handle.product(), idx), nullptr}, key_{idx}
{
  if (core_.isNull()) {
    throw Exception(errors::InvalidReference)
      << "Attempt to construct a Ptr from a Handle with invalid ProductID. "
         "Perhaps a\ndefault-constructed Ptr is what you want?";
  }
}

template <typename T>
inline art::Ptr<T>::Ptr(ProductID const productID)
  : core_{productID, 0, nullptr}, key_{key_traits<key_type>::value}
{}

template <typename T>
inline art::Ptr<T>::Ptr(ProductID const productID,
                        key_type const itemKey,
                        EDProductGetter const* prodGetter)
  : core_{productID, 0, prodGetter}, key_{itemKey}
{}

template <typename T>
template <typename U>
inline art::Ptr<T>::Ptr(Ptr<U> const& iOther,
                        std::enable_if_t<std::is_base_of<T, U>::value>*)
  : core_(iOther.id(),
          (iOther.hasCache() ? static_cast<T const*>(iOther.get()) :
                               static_cast<T const*>(nullptr)),
          iOther.productGetter())
  , key_{iOther.key()}
{}

template <typename T>
template <typename U>
inline art::Ptr<T>::Ptr(Ptr<U> const& iOther,
                        std::enable_if_t<std::is_base_of<U, T>::value>*)
  : core_{iOther.id(), dynamic_cast<T const*>(iOther.get()), nullptr}
  , key_{iOther.key()}
{}

template <typename T>
inline art::Ptr<T>::Ptr(ProductID const productID,
                        T const* item,
                        key_type const itemKey)
  : core_{productID, item, nullptr}, key_{itemKey}
{}

template <typename T>
inline T const& art::Ptr<T>::operator*() const
{
  return *get();
}

template <typename T>
inline T const* art::Ptr<T>::operator->() const
{
  getData_();
  return reinterpret_cast<T const*>(core_.productPtr());
}

template <typename T>
inline T const*
art::Ptr<T>::get() const
{
  return isNull() ? nullptr : this->operator->();
}

template <typename T>
inline bool
art::Ptr<T>::isNull() const
{
  return !isNonnull();
}

template <typename T>
inline bool
art::Ptr<T>::isNonnull() const
{
  return key_traits<key_type>::value != key_;
}

template <typename T>
inline bool
art::Ptr<T>::isAvailable() const
{
  return core_.isAvailable();
}

template <typename T>
inline auto
art::Ptr<T>::id() const -> ProductID
{
  return core_.id();
}

template <typename T>
inline auto
art::Ptr<T>::key() const -> key_type
{
  return key_;
}

template <typename T>
inline bool
art::Ptr<T>::hasCache() const
{
  return nullptr != core_.productPtr();
}

template <typename T>
inline auto
art::Ptr<T>::refCore() const -> RefCore const&
{
  return core_;
}

template <typename T>
inline art::Ptr<T>::operator bool() const
{
  return isNonnull() && isAvailable();
}

template <typename T>
inline auto
art::Ptr<T>::productGetter() const -> EDProductGetter const*
{
  return core_.productGetter();
}

template <typename T>
template <typename C>
inline T const*
art::Ptr<T>::getItem_(C const* product, key_type iKey)
{
  return detail::ItemGetter<T, C>()(product, iKey);
}

template <typename T>
void
art::Ptr<T>::getData_() const
{
  if (!hasCache()) {
    EDProduct const* prod =
      productGetter() ? productGetter()->getIt() : nullptr;
    if (prod == nullptr) {
      art::Exception e(errors::ProductNotFound);
      e << "A request to resolve an art::Ptr to a product containing items of "
           "type: "
        << cet::demangle_symbol(typeid(T).name()) << " with ProductID "
        << core_.id()
        << "\ncannot be satisfied because the product cannot be found.\n";
      if (productGetter() == nullptr) {
        e << "The productGetter was not set -- are you trying to dereference a "
             "Ptr during mixing?\n";
      } else {
        e << "Probably the branch containing the product is not stored in the "
             "input file.\n";
      }
      throw e;
    }
    void const* ad = nullptr;
    prod->setPtr(typeid(T), key_, ad);
    core_.setProductPtr(ad);
  }
}

template <typename T, typename U>
inline auto
art::operator==(Ptr<T> const& lhs, Ptr<U> const& rhs)
  -> std::enable_if_t<std::is_same<T, U>::value ||
                        std::is_base_of<T, U>::value ||
                        std::is_base_of<U, T>::value,
                      bool>
{
  return lhs.refCore() == rhs.refCore() && lhs.key() == rhs.key();
}

template <typename T, typename U>
inline auto
art::operator!=(Ptr<T> const& lhs, Ptr<U> const& rhs)
  -> std::enable_if_t<std::is_same<T, U>::value ||
                        std::is_base_of<T, U>::value ||
                        std::is_base_of<U, T>::value,
                      bool>
{
  return !(lhs == rhs);
}

template <typename T, typename U>
inline auto
art::operator<(Ptr<T> const& lhs, Ptr<U> const& rhs)
  -> std::enable_if_t<std::is_same<T, U>::value ||
                        std::is_base_of<T, U>::value ||
                        std::is_base_of<U, T>::value,
                      bool>
{
  // The ordering of integer keys guarantees that the ordering of Ptrs within
  // a collection will be identical to the ordering of the referenced objects in
  // the collection.
  return (lhs.refCore() == rhs.refCore() ? lhs.key() < rhs.key() :
                                           lhs.refCore() < rhs.refCore());
}

template <typename T, typename H>
void
art::fill_ptr_vector(std::vector<Ptr<T>>& ptrs, H const& h)
{
  for (std::size_t i = 0, sz = h->size(); i != sz; ++i) {
    ptrs.emplace_back(h, i);
  }
}

template <typename T, typename H>
void
art::fill_ptr_list(std::list<Ptr<T>>& ptrs, H const& h)
{
  for (std::size_t i = 0, sz = h->size(); i != sz; ++i) {
    ptrs.emplace_back(h, i);
  }
}

template <typename TO, typename PTRVAL>
TO
art::detail::EnsurePointer<TO, art::Ptr<PTRVAL>>::operator()(
  Ptr<PTRVAL>& from) const
{
  return detail::addr<TO, PTRVAL const>(*from);
}

template <typename TO, typename PTRVAL>
TO
art::detail::EnsurePointer<TO, art::Ptr<PTRVAL>>::operator()(
  Ptr<PTRVAL> const& from) const
{
  return detail::addr<TO, PTRVAL const>(*from);
}

template <typename T>
std::ostream&
art::operator<<(std::ostream& os, Ptr<T> const& p)
{
  os << "(" << p.id() << ", " << p.key() << ")";
  return os;
}

// Specialization of std::hash for art::Ptr
namespace std {
  template <class T>
  struct hash<art::Ptr<T>> {
    using ptr_t = art::Ptr<T>;
    using key_t = typename ptr_t::key_type;

    size_t
    operator()(ptr_t const& p) const
    {
      return std::hash<art::ProductID>()(p.id()) ^ std::hash<key_t>()(p.key());
    }
  };
}

#endif /* canvas_Persistency_Common_Ptr_h */

// Local Variables:
// mode: c++
// End:
