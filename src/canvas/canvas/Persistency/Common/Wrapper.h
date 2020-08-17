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
#include "canvas/Persistency/Common/Sampled.h"
#include "canvas/Persistency/Common/detail/ProductTypeIDs.h"
#include "canvas/Persistency/Common/detail/aggregate.h"
#include "canvas/Persistency/Provenance/SubRunID.h"
#include "canvas/Utilities/DebugMacros.h"
#include "cetlib/metaprogramming.h"
#include "cetlib_except/demangle.h"

#include <memory>
#include <string>
#include <vector>

namespace art {
  template <typename T>
  struct prevent_recursion;

  template <typename T>
  class Wrapper;

  // Implementation detail declarations.
  namespace detail {

    using cet::enable_if_function_exists_t;

    // has_size_member
    template <typename T, typename = void>
    struct has_size_member : std::false_type {};

    template <typename T>
    struct has_size_member<
      T,
      enable_if_function_exists_t<size_t (T::*)() const, &T::size>>
      : std::true_type {};

    // has_makePartner_member
    template <typename T, typename = void>
    struct has_makePartner_member : std::false_type {};

    template <typename T>
    struct has_makePartner_member<
      T,
      enable_if_function_exists_t<std::unique_ptr<EDProduct> (T::*)(
                                    std::type_info const&) const,
                                  &T::makePartner>> : std::true_type {};
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

  product_typeids_t do_getTypeIDs() const override;
  std::unique_ptr<EDProduct> do_makePartner(
    std::type_info const& wanted_type) const override;

  unsigned do_getRangeSetID() const override;
  void do_setRangeSetID(unsigned) override;
  void do_combine(EDProduct* product) override;
  std::unique_ptr<EDProduct> do_createEmptySampledProduct(
    InputTag const& tag) const override;

  template <typename>
  friend struct prevent_recursion;

  void do_insertIfSampledProduct(std::string const& dataset,
                                 SubRunID const& id,
                                 std::unique_ptr<EDProduct> product) override;

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

#include "boost/lexical_cast.hpp"
#include "canvas/Persistency/Common/GetProduct.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "canvas/Persistency/Common/getElementAddresses.h"
#include "canvas/Persistency/Common/setPtr.h"
#include "canvas/Persistency/Common/traits.h"
#include "canvas/Utilities/Exception.h"

#include <deque>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>

////////////////////////////////////////////////////////////////////////
// Wrapper member functions.
template <typename T>
art::Wrapper<T>::Wrapper(std::unique_ptr<T> ptr)
  : present{ptr.get() != nullptr}, rangeSetID{-1u}, obj(refOrThrow(ptr.get()))
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
art::product_typeids_t
art::Wrapper<T>::do_getTypeIDs() const
{
  return detail::ProductTypeIDs<T>::get();
}

template <typename T>
std::unique_ptr<art::EDProduct>
art::Wrapper<T>::do_makePartner(std::type_info const& wanted_wrapper) const
{
  std::unique_ptr<art::EDProduct> retval;
  if constexpr (detail::has_makePartner_member<T>::value) {
    retval = obj.makePartner(wanted_wrapper);
  } else {
    throw Exception{errors::LogicError, "makePartner"}
      << "Attempted to make partner of a product ("
      << cet::demangle_symbol(typeid(T).name()) << ") that does not know how!\n"
      << "Please report to the art framework developers.\n";
  }
  return retval;
}

namespace art {
  template <typename T>
  struct prevent_recursion {
    static std::unique_ptr<EDProduct>
    create_empty_sampled_product(InputTag const& tag)
    {
      auto emptySampledProduct = std::make_unique<Sampled<T>>(tag);
      return std::make_unique<Wrapper<Sampled<T>>>(move(emptySampledProduct));
    }

    [[noreturn]] static void
    insert_if_sampled_product(T&,
                              std::string const& dataset,
                              SubRunID const&,
                              std::unique_ptr<EDProduct>)
    {
      throw Exception{errors::LogicError}
        << "An attempt was made to insert a product from dataset '" << dataset
        << "'\ninto a non-sampled product of type '"
        << cet::demangle_symbol(typeid(T).name()) << "'.\n"
        << "Please contact artists@fnal.gov for guidance.";
    }
  };

  template <typename T>
  struct prevent_recursion<Sampled<T>> {
    [[noreturn]] static std::unique_ptr<EDProduct>
    create_empty_sampled_product(InputTag const&)
    {
      throw Exception{errors::LogicError}
        << "An attempt was made to create an empty sampled product\n"
        << "for a sampled product.  This type of recursion is not allowed.\n"
        << "Please contact artists@fnal.gov for guidance.";
    }

    static void
    insert_if_sampled_product(Sampled<T>& obj,
                              std::string const& dataset,
                              SubRunID const& id,
                              std::unique_ptr<EDProduct> product)
    {
      auto& wp = dynamic_cast<Wrapper<T>&>(*product);
      obj.insert(dataset, id, std::move(wp.obj));
    }
  };
}

template <typename T>
std::unique_ptr<art::EDProduct>
art::Wrapper<T>::do_createEmptySampledProduct(InputTag const& tag) const
{
  return prevent_recursion<T>::create_empty_sampled_product(tag);
}

template <typename T>
void
art::Wrapper<T>::do_insertIfSampledProduct(std::string const& dataset,
                                           SubRunID const& id,
                                           std::unique_ptr<EDProduct> product)
{
  prevent_recursion<T>::insert_if_sampled_product(
    obj, dataset, id, move(product));
}

template <typename T>
inline void
art::Wrapper<T>::do_setPtr(std::type_info const& toType,
                           unsigned long const index [[maybe_unused]],
                           void const*& ptr) const
{
  if constexpr (has_setPtr<T>::value) {
    // Allow setPtr customizations by introducing the art::setPtr
    // overload set, and not requiring art::setPtr(...).
    using art::setPtr;
    setPtr(obj, toType, index, ptr);
  } else {
    throw Exception{errors::ProductDoesNotSupportPtr}
      << "The product type " << cet::demangle_symbol(typeid(T).name())
      << " does not support art::Ptr\n";
  }
}

template <typename T>
inline void
art::Wrapper<T>::do_getElementAddresses(
  std::type_info const& toType,
  std::vector<unsigned long> const& indices,
  std::vector<void const*>& ptrs) const
{
  if constexpr (has_setPtr<T>::value) {
    // getElementAddresses is the name of an overload set; each
    // concrete collection T should supply a getElementAddresses
    // function, in the same namespace at that in which T is
    // defined, or in the 'art' namespace.
    using art::getElementAddresses;
    getElementAddresses(obj, toType, indices, ptrs);
  } else {
    throw Exception{errors::ProductDoesNotSupportPtr}
      << "The product type " << cet::demangle_symbol(typeid(T).name())
      << " does not support art::PtrVector\n";
  }
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
    : public productSize<std::vector<E>, true> {};

  template <class E>
  struct productSize<std::list<E>, false>
    : public productSize<std::list<E>, true> {};

  template <class E>
  struct productSize<std::deque<E>, false>
    : public productSize<std::deque<E>, true> {};

  template <class E>
  struct productSize<std::set<E>, false>
    : public productSize<std::set<E>, true> {};

  template <class E>
  struct productSize<PtrVector<E>, false>
    : public productSize<PtrVector<E>, true> {};

  template <class E>
  struct productSize<cet::map_vector<E>, false>
    : public productSize<cet::map_vector<E>, true> {};
}

#endif /* canvas_Persistency_Common_Wrapper_h */

// Local Variables:
// mode: c++
// End:
