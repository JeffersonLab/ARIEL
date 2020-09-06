#ifndef canvas_Persistency_Common_detail_IPRHelper_h
#define canvas_Persistency_Common_detail_IPRHelper_h

// Helper class and associated gubbins for populating the FindOne and
// FindMany query objects for inter-product references.

#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Provenance/ProductToken.h"
#include "canvas/Utilities/Exception.h"
#include "canvas/Utilities/InputTag.h"
#include "canvas/Utilities/ensurePointer.h"

#include <type_traits>
#include <unordered_map>

namespace art::detail {

  template <typename ProdA, typename ProdB, typename Data>
  struct safe_input_tag {
    safe_input_tag(InputTag const& input_tag) : tag{input_tag} {}
    safe_input_tag(ProductToken<Assns<ProdA, ProdB, Data>> const& token)
      : tag{token.inputTag_}
    {}
    art::InputTag tag;
  };

  template <typename ProdA, typename ProdB, typename Data, typename Tag>
  InputTag
  input_tag(Tag const& tag)
  {
    static_assert(
      std::is_convertible_v<Tag, InputTag> ||
        std::is_same_v<Tag, ProductToken<Assns<ProdA, ProdB>>> ||
        std::is_same_v<Tag, ProductToken<Assns<ProdA, ProdB, Data>>>,
      "\n\nart error: The input tag or product token provided to the "
      "smart-query object\n"
      "           constructor has a type that conflicts with that of the "
      "smart-query object.\n");
    return safe_input_tag<ProdA, ProdB, Data>(tag).tag;
  }

  class IPRHelperDef {};

  template <typename ProdA,
            typename ProdB,
            typename Data,
            typename DATACOLL,
            typename EVENT>
  class IPRHelper;

  template <typename DATA>
  class DataCollHelper {
  public:
    void init(size_t size, std::vector<DATA const*>& data) const;
    template <typename ASSNS>
    void fill(ptrdiff_t assns_index,
              ASSNS const& assns,
              size_t data_index,
              std::vector<DATA const*>& data) const;

    void init(size_t size, std::vector<std::vector<DATA const*>>& data) const;
    template <typename ASSNS>
    void fill(ptrdiff_t assns_index,
              ASSNS const& assns,
              size_t data_index,
              std::vector<std::vector<DATA const*>>& data) const;

    void init(size_t, IPRHelperDef&) const;
    template <typename ASSNS>
    void fill(ptrdiff_t, ASSNS const&, size_t, IPRHelperDef&) const;
  };

  // Note that the template parameter Bcoll is determined by the
  // IPRHelper's use by the FindOne and FindMany classes, and is not
  // as free-ranging as one might naively imagine.
  template <typename ProdB>
  class BcollHelper {
  public:
    BcollHelper(InputTag const& assnsTag);
    template <typename Bcoll>
    void init(size_t size, Bcoll& bColl);

    // 1. When Bcoll is a collection of pointer to const B -- one to one.
    template <typename Bcoll>
    std::enable_if_t<std::is_same_v<typename Bcoll::value_type, ProdB const*>>
    fill(size_t index, Ptr<ProdB> const& item, Bcoll& bColl);

    // 2. When Bcoll is a collection of Ptr<B> -- one to one.
    template <typename Bcoll>
    std::enable_if_t<
      std::is_convertible_v<typename Bcoll::value_type, Ptr<ProdB>>>
    fill(size_t index, Ptr<ProdB> const& item, Bcoll& bColl);

    template <typename Bcoll>
    void init(size_t size, std::vector<Bcoll>& bColls) const;

    // 3. When Bcoll is a collection of pointer to const B -- one to many.
    template <typename Bcoll>
    std::enable_if_t<std::is_same_v<typename Bcoll::value_type, ProdB const*>>
    fill(size_t index,
         Ptr<ProdB> const& item,
         std::vector<Bcoll>& bColls) const;

    // 4. When Bcoll is a collection of Ptr<B> -- one to many.
    template <typename Bcoll>
    std::enable_if_t<
      std::is_convertible_v<typename Bcoll::value_type, Ptr<ProdB>>>
    fill(size_t index,
         Ptr<ProdB> const& item,
         std::vector<Bcoll>& bColls) const;

  private:
    InputTag const assnsTag_;
    std::vector<uint8_t> seen_;
  };
}

template <typename ProdA,
          typename ProdB,
          typename Data,
          typename DATACOLL,
          typename EVENT>
class art::detail::IPRHelper {
private:
  // We use IPRHelperDef in place of DATACOLL if Data is void.
  using dataColl_t =
    std::conditional_t<std::is_void_v<Data>, IPRHelperDef, DATACOLL>;

public:
  using shared_exception_t = std::shared_ptr<art::Exception const>;

  IPRHelper(EVENT const& e, InputTag const& tag) : event_{e}, assnsTag_{tag} {}

  // template <typename A, typename B> shared_exception_t operator()(A const& a,
  // B const& b) const
  //  (1) fills in b, and
  //  (2) returns a (shared pointer to) an exception. The pointer is
  //  non-null on failure. Note that the returned 'b' might be empty.
  //
  // 1. When dColl not wanted.
  template <typename Acoll, typename Bcoll>
  shared_exception_t operator()(Acoll const& aColl, Bcoll& bColl) const;

  // 2. Algorithm useful when dealing with collections of Ptrs.
  template <typename Acoll, typename Bcoll>
  shared_exception_t operator()(Acoll const& aColl,
                                Bcoll& bColl,
                                dataColl_t& dColl) const;

private:
  EVENT const& event_;
  InputTag const assnsTag_;
};

// 1.
template <typename ProdA,
          typename ProdB,
          typename Data,
          typename DATACOLL,
          typename EVENT>
template <typename Acoll, typename Bcoll>
inline auto
art::detail::IPRHelper<ProdA, ProdB, Data, DATACOLL, EVENT>::operator()(
  Acoll const& aColl,
  Bcoll& bColl) const -> shared_exception_t
{
  IPRHelperDef dummy;
  return (*this)(aColl, bColl, dummy);
}

// 2.
////////////////////////////////////////////////////////////////////////
// Implementation notes.
//
// The current implementation does not verify that the ProductID of the
// item in the association collection matches that of the item in the
// reference collection before attempting to dereference its Ptr
// (although it does verify ptr.isAvailable()). This means that in the
// case where an association collection refers to multiple available
// AProd collections, all of those collections will be read from file
// even if the reference collection does not include items from one or
// more of those AProd collections.
//
// If one were to provide an implementation that did this, one would
// change the unordered_multimap to key on the full ptr instead of the
// pointer. There is a specialization of std::hash<T> for T =
// art::Ptr<X> to support this.
//
// However, it would be problematic to do the lookup if the reference
// item was not in fact a Ptr. Maybe it would be relatively efficient if
// one were able to do a lookup in the table against an entity not a Ptr
// for which I could write a comparison function that compared the
// ProductID and only if they matched, the pointer with suitable get().
//
// For now however, no-one has requested this,
////////////////////////////////////////////////////////////////////////
template <typename ProdA,
          typename ProdB,
          typename Data,
          typename DATACOLL,
          typename EVENT>
template <typename Acoll, typename Bcoll>
auto
art::detail::IPRHelper<ProdA, ProdB, Data, DATACOLL, EVENT>::
operator()(Acoll const& aColl, Bcoll& bColl, dataColl_t& dColl) const
  -> shared_exception_t
{
  detail::BcollHelper<ProdB> bh(assnsTag_);
  detail::DataCollHelper<Data> dh;
  typename EVENT::template HandleT<Assns<ProdA, ProdB, Data>> assnsHandle;
  event_.getByLabel(assnsTag_, assnsHandle);
  if (!assnsHandle.isValid()) {
    return assnsHandle.whyFailed(); // Failed to get Assns product.
  }
  bh.init(aColl.size(), bColl);
  dh.init(aColl.size(), dColl);
  // Answer cache.
  std::unordered_multimap<typename Ptr<ProdA>::const_pointer,
                          std::pair<Ptr<ProdB>, ptrdiff_t>>
    lookupCache;
  ptrdiff_t counter{0};
  for (auto const& apair : *assnsHandle) {
    if (apair.first.isAvailable()) {
      lookupCache.emplace(
        apair.first.get(),
        typename decltype(lookupCache)::mapped_type(apair.second, counter));
    }
    ++counter;
  }
  // Now use the cache.
  size_t bIndex{0};
  for (typename Acoll::const_iterator i = aColl.begin(), e = aColl.end();
       i != e;
       ++i, ++bIndex) {
    auto foundItems = lookupCache.equal_range(
      ensurePointer<typename Ptr<ProdA>::const_pointer>(i));
    if (foundItems.first != lookupCache.cend()) {
      std::for_each(
        foundItems.first,
        foundItems.second,
        [&bh, &dh, &bColl, bIndex, &assnsHandle, &dColl](auto const& itemPair) {
          bh.fill(bIndex, itemPair.second.first, bColl);
          dh.fill(itemPair.second.second, *assnsHandle, bIndex, dColl);
        });
    }
  }
  return shared_exception_t();
}

template <typename DATA>
inline void
art::detail::DataCollHelper<DATA>::init(size_t const size,
                                        std::vector<DATA const*>& data) const
{
  data.assign(size, 0);
}

template <typename DATA>
template <typename ASSNS>
inline void
art::detail::DataCollHelper<DATA>::fill(ptrdiff_t const assns_index,
                                        ASSNS const& assns,
                                        size_t const data_index,
                                        std::vector<DATA const*>& data) const
{
  data[data_index] = &assns.data(assns_index);
}

template <typename DATA>
inline void
art::detail::DataCollHelper<DATA>::init(
  size_t const size,
  std::vector<std::vector<DATA const*>>& data) const
{
  data.resize(size);
}

template <typename DATA>
template <typename ASSNS>
inline void
art::detail::DataCollHelper<DATA>::fill(
  ptrdiff_t const assns_index,
  ASSNS const& assns,
  size_t const data_index,
  std::vector<std::vector<DATA const*>>& data) const
{
  data[data_index].push_back(&assns.data(assns_index));
}

template <typename DATA>
inline void
art::detail::DataCollHelper<DATA>::init(size_t, IPRHelperDef&) const
{}

template <typename DATA>
template <typename ASSNS>
inline void
art::detail::DataCollHelper<DATA>::fill(ptrdiff_t,
                                        ASSNS const&,
                                        size_t,
                                        IPRHelperDef&) const
{}

template <typename ProdB>
inline art::detail::BcollHelper<ProdB>::BcollHelper(InputTag const& assnsTag)
  : assnsTag_{assnsTag}, seen_()
{}

template <typename ProdB>
template <typename Bcoll>
inline void
art::detail::BcollHelper<ProdB>::init(size_t const size, Bcoll& bColl)
{
  // This works if BColl is a collection of pointers or Ptrs.
  bColl.assign(size, typename Bcoll::value_type{});
  seen_.assign(size, uint8_t{});
}

// 1.
template <typename ProdB>
template <typename Bcoll>
inline std::enable_if_t<
  std::is_same_v<typename Bcoll::value_type, ProdB const*>>
art::detail::BcollHelper<ProdB>::fill(size_t const index,
                                      Ptr<ProdB> const& item,
                                      Bcoll& bColl)
{
  // This works if BColl is a collection of pointers or Ptrs.
  if (seen_[index] == uint8_t(1u)) {
    throw Exception(errors::LogicError)
      << "Attempted to create a FindOne object for a one-many or many-many"
      << " association specified in collection " << assnsTag_ << ".\n";
  } else if (item) {
    bColl[index] = item.get();
    seen_[index] = uint8_t(1u);
  } else {
    throw Exception(errors::LogicError)
      << "Attempted to create a FindOne object where an associated item is "
      << "\nunavailable.\n";
  }
}

// 2.
template <typename ProdB>
template <typename Bcoll>
inline std::enable_if_t<
  std::is_convertible_v<typename Bcoll::value_type, art::Ptr<ProdB>>>
art::detail::BcollHelper<ProdB>::fill(size_t const index,
                                      Ptr<ProdB> const& item,
                                      Bcoll& bColl)
{
  // This works if BColl is a collection of pointers or Ptrs.
  if (seen_[index] == uint8_t(1u)) {
    throw Exception(errors::LogicError)
      << "Attempted to create a FindOne object for a one-many or many-many"
      << " association specified in collection " << assnsTag_ << ".\n";
  }
  bColl[index] = item;
  seen_[index] = uint8_t(1u);
}

template <typename ProdB>
template <typename Bcoll>
inline void
art::detail::BcollHelper<ProdB>::init(size_t const size,
                                      std::vector<Bcoll>& bColls) const
{
  bColls.resize(size);
}

// 3.
template <typename ProdB>
template <typename Bcoll>
inline std::enable_if_t<
  std::is_same_v<typename Bcoll::value_type, ProdB const*>>
art::detail::BcollHelper<ProdB>::fill(size_t const index,
                                      Ptr<ProdB> const& item,
                                      std::vector<Bcoll>& bColls) const
{
  bColls[index].push_back(item ? item.get() : nullptr);
}

// 4.
template <typename ProdB>
template <typename Bcoll>
inline std::enable_if_t<
  std::is_convertible_v<typename Bcoll::value_type, art::Ptr<ProdB>>>
art::detail::BcollHelper<ProdB>::fill(size_t const index,
                                      Ptr<ProdB> const& item,
                                      std::vector<Bcoll>& bColls) const
{
  bColls[index].push_back(item);
}

#endif /* canvas_Persistency_Common_detail_IPRHelper_h */

// Local Variables:
// mode: c++
// End:
