#ifndef canvas_Persistency_Common_Wrapper_h
#define canvas_Persistency_Common_Wrapper_h
// vim: set sw=2:

// =====================================================================
// Wrapper: A class template that inherits from art::EDProduct, thus
//          providing the representation needed for providing products
//          of type T.  Each instantiation also includes:
//            - a Boolean value corresponding to the presence of the
//              product in the file
//            - the RangeSet corresponding to the set of events
//              processed in creating the product.
// =====================================================================

#include "canvas/Persistency/Common/EDProduct.h"
#include "canvas/Persistency/Common/detail/aggregate.h"
#include "canvas/Utilities/DebugMacros.h"
#include "cetlib/metaprogramming.h"
#include "cetlib_except/demangle.h"

#include <memory>
#include <string>
#include <vector>

namespace art {
  template <typename T>
  class Wrapper;

  // Implementation detail declarations.
  namespace detail {

    using cet::enable_if_function_exists_t;

    // has_size_member
    template <typename T, typename = void>
    struct has_size_member : std::false_type {
    };

    template <typename T>
    struct has_size_member<
      T,
      enable_if_function_exists_t<size_t (T::*)() const, &T::size>>
      : std::true_type {
    };

    // has_makePartner_member
    template <typename T, typename = void>
    struct has_makePartner_member : std::false_type {
    };

    template <typename T>
    struct has_makePartner_member<
      T,
      enable_if_function_exists_t<std::unique_ptr<EDProduct> (T::*)(
                                    std::type_info const&) const,
                                  &T::makePartner>> : std::true_type {
    };
  }

  template <typename T, bool = detail::has_size_member<T>::value>
  struct productSize;

  template <typename T>
  struct DoMakePartner;

  template <typename T>
  struct DoNotMakePartner;

  template <typename T>
  struct DoSetPtr;

  template <typename T>
  struct DoNotSetPtr;
}

////////////////////////////////////////////////////////////////////////
// Definition of art::Wrapper<T>
template <typename T>
class art::Wrapper : public art::EDProduct {
public:
  Wrapper() = default;

  explicit Wrapper(std::unique_ptr<T> ptr);
  virtual ~Wrapper() = default;

  T const* product() const;
  T const* operator->() const;

  // MUST UPDATE WHEN CLASS IS CHANGED!
  static short
  Class_Version()
  {
    return 11;
  }

private:
  void fillView(std::vector<void const*>& view) const override;

  std::string productSize() const override;
  void do_combine(EDProduct* product) override;

  void do_setRangeSetID(unsigned) override;
  unsigned do_getRangeSetID() const override;

  std::unique_ptr<EDProduct> do_makePartner(
    std::type_info const& wanted_type) const override;

  bool
  isPresent_() const override
  {
    return present;
  }
  std::type_info const* typeInfo_() const override;

  void do_setPtr(std::type_info const& toType,
                 unsigned long index,
                 void const*& ptr) const override;

  void do_getElementAddresses(std::type_info const& toType,
                              std::vector<unsigned long> const& indices,
                              std::vector<void const*>& ptr) const override;

  T&& refOrThrow(T* ptr);

  bool present{false};
  unsigned rangeSetID{-1u};
  T obj{};

}; // Wrapper<>

////////////////////////////////////////////////////////////////////////
// Implementation details.

#include "canvas/Persistency/Common/GetProduct.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "canvas/Persistency/Common/getElementAddresses.h"
#include "canvas/Persistency/Common/setPtr.h"

#include "boost/lexical_cast.hpp"
#include "canvas/Persistency/Common/traits.h"
#include "canvas/Utilities/Exception.h"
#include <memory>
#include <type_traits>

#include <deque>
#include <list>
#include <set>
#include <string>
#include <typeinfo>
#include <vector>

////////////////////////////////////////////////////////////////////////
// Wrapper member functions.
template <typename T>
art::Wrapper<T>::Wrapper(std::unique_ptr<T> ptr)
  : present{ptr.get() != 0}, rangeSetID{-1u}, obj(refOrThrow(ptr.get()))
{}

template <typename T>
T const*
art::Wrapper<T>::product() const
{
  return present ? &obj : nullptr;
}

template <typename T>
T const* art::Wrapper<T>::operator->() const
{
  return product();
}

template <typename T>
std::type_info const*
art::Wrapper<T>::typeInfo_() const
{
  return SupportsView<T>::type_id();
}

template <typename T>
void
art::Wrapper<T>::fillView(std::vector<void const*>& view) const
{
  MaybeFillView<T>::fill(obj, view);
}

template <typename T>
std::string
art::Wrapper<T>::productSize() const
{
  return art::productSize<T>()(obj);
}

template <typename T>
void
art::Wrapper<T>::do_combine(art::EDProduct* p)
{
  if (!p->isPresent())
    return;

  auto wp = static_cast<Wrapper<T>*>(p);
  detail::CanBeAggregated<T>::aggregate(obj, *wp->product());

  // The presence for the combined product is 'true', if we get this
  // far.
  present = true;
}

template <typename T>
void
art::Wrapper<T>::do_setRangeSetID(unsigned const id)
{
  rangeSetID = id;
}

template <typename T>
unsigned
art::Wrapper<T>::do_getRangeSetID() const
{
  return rangeSetID;
}

template <typename T>
std::unique_ptr<art::EDProduct>
art::Wrapper<T>::do_makePartner(std::type_info const& wanted_wrapper) const
{
  std::unique_ptr<art::EDProduct> retval;
  std::conditional_t<detail::has_makePartner_member<T>::value,
                     DoMakePartner<T>,
                     DoNotMakePartner<T>>
    maybe_maker;
  retval = maybe_maker(obj, wanted_wrapper);
  return retval;
}

template <typename T>
inline void
art::Wrapper<T>::do_setPtr(std::type_info const& toType,
                           unsigned long index,
                           void const*& ptr) const
{
  std::conditional_t<has_setPtr<T>::value, DoSetPtr<T>, DoNotSetPtr<T>>
    maybe_filler;
  maybe_filler(this->obj, toType, index, ptr);
}

template <typename T>
inline void
art::Wrapper<T>::do_getElementAddresses(
  std::type_info const& toType,
  std::vector<unsigned long> const& indices,
  std::vector<void const*>& ptrs) const
{
  std::conditional_t<has_setPtr<T>::value, DoSetPtr<T>, DoNotSetPtr<T>>
    maybe_filler;
  maybe_filler(this->obj, toType, indices, ptrs);
}

template <typename T>
inline T&&
art::Wrapper<T>::refOrThrow(T* ptr)
{
  if (ptr) {
    return std::move(*ptr);
  } else {
    throw Exception(errors::NullPointerError)
      << "Attempt to construct " << cet::demangle_symbol(typeid(*this).name())
      << " from nullptr.\n";
  }
}

////////////////////////////////////////////////////////////////////////
// Metafunction support for compile-time selection of code used in
// Wrapper implementation.

namespace art {

  template <typename T>
  struct productSize<T, true> {
    std::string
    operator()(T const& obj) const
    {
      return boost::lexical_cast<std::string>(obj.size());
    }
  };

  template <typename T>
  struct productSize<T, false> {
    std::string
    operator()(T const&) const
    {
      return "-";
    }
  };

  template <class E>
  struct productSize<std::vector<E>, false>
    : public productSize<std::vector<E>, true> {
  };

  template <class E>
  struct productSize<std::list<E>, false>
    : public productSize<std::list<E>, true> {
  };

  template <class E>
  struct productSize<std::deque<E>, false>
    : public productSize<std::deque<E>, true> {
  };

  template <class E>
  struct productSize<std::set<E>, false>
    : public productSize<std::set<E>, true> {
  };

  template <class E>
  struct productSize<PtrVector<E>, false>
    : public productSize<PtrVector<E>, true> {
  };

  template <class E>
  struct productSize<cet::map_vector<E>, false>
    : public productSize<cet::map_vector<E>, true> {
  };

  template <typename T>
  struct DoMakePartner {
    std::unique_ptr<EDProduct>
    operator()(T const& obj, std::type_info const& wanted_wrapper_type) const
    {
      return obj.makePartner(wanted_wrapper_type);
    }
  };

  template <typename T>
  struct DoNotMakePartner {
    std::unique_ptr<EDProduct>
    operator()(T const&, std::type_info const&) const
    {
      throw Exception(errors::LogicError, "makePartner")
        << "Attempted to make partner of a product ("
        << cet::demangle_symbol(typeid(T).name())
        << ") that does not know how!\n"
        << "Please report to the ART framework developers.\n";
    }
  };

  template <typename T>
  struct DoSetPtr {
    void operator()(T const& obj,
                    std::type_info const& toType,
                    unsigned long index,
                    void const*& ptr) const;
    void operator()(T const& obj,
                    std::type_info const& toType,
                    std::vector<unsigned long> const& index,
                    std::vector<void const*>& ptrs) const;
  };

  template <typename T>
  struct DoNotSetPtr {
    void
    operator()(T const&,
               std::type_info const&,
               unsigned long,
               void const*&) const
    {
      throw Exception(errors::ProductDoesNotSupportPtr)
        << "The product type " << cet::demangle_symbol(typeid(T).name())
        << "\ndoes not support art::Ptr\n";
    }

    void
    operator()(T const&,
               std::type_info const&,
               std::vector<unsigned long> const&,
               std::vector<void const*>&) const
    {
      throw Exception(errors::ProductDoesNotSupportPtr)
        << "The product type " << cet::demangle_symbol(typeid(T).name())
        << "\ndoes not support art::PtrVector\n";
    }
  };

  template <typename T>
  void
  DoSetPtr<T>::operator()(T const& obj,
                          std::type_info const& toType,
                          unsigned long const index,
                          void const*& ptr) const
  {
    // setPtr is the name of an overload set; each concrete collection
    // T should supply a setPtr function, in the same namespace at
    // that in which T is defined, or in the 'art' namespace.
    setPtr(obj, toType, index, ptr);
  }

  template <typename T>
  void
  DoSetPtr<T>::operator()(T const& obj,
                          std::type_info const& toType,
                          std::vector<unsigned long> const& indices,
                          std::vector<void const*>& ptr) const
  {
    // getElementAddresses is the name of an overload set; each
    // concrete collection T should supply a getElementAddresses
    // function, in the same namespace at that in which T is
    // defined, or in the 'art' namespace.
    getElementAddresses(obj, toType, indices, ptr);
  }
}

#endif /* canvas_Persistency_Common_Wrapper_h */

// Local Variables:
// mode: c++
// End:
