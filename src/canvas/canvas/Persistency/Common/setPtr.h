#ifndef canvas_Persistency_Common_setPtr_h
#define canvas_Persistency_Common_setPtr_h

#include "canvas/Persistency/Common/detail/maybeCastObj.h"
#include "canvas/Utilities/uniform_type_name.h"
#include "cetlib/map_vector.h"
#include "cetlib_except/demangle.h"

#include <iterator>
#include <typeinfo>

namespace art {
  template <class COLLECTION>
  void setPtr(COLLECTION const& coll,
              std::type_info const& iToType,
              unsigned long iIndex,
              void const*& oPtr);

  template <typename T>
  void setPtr(cet::map_vector<T> const& obj,
              std::type_info const& iToType,
              unsigned long iIndex,
              void const*& oPtr);
}

template <class Collection>
void
art::setPtr(Collection const& coll,
            std::type_info const& iToType,
            unsigned long iIndex,
            void const*& oPtr)
{
  using product_type = Collection;
  auto it = coll.begin();
  if (iIndex >= coll.size()) {
    throw Exception{
      errors::LogicError,
      "An out-of-bounds error was encountered while setting an art::Ptr.\n"}
      << "An attempt was made to access an element with index " << iIndex
      << " for a container with a size of " << coll.size() << ".\n"
      << "The container is of type " +
           cet::demangle_symbol(typeid(Collection).name())
      << ".\n";
  }
  advance(it, iIndex);
  oPtr = detail::maybeCastObj(detail::GetProduct<product_type>::address(it),
                              iToType);
}

template <typename T>
void
art::setPtr(cet::map_vector<T> const& obj,
            std::type_info const& iToType,
            unsigned long iIndex,
            void const*& oPtr)
{
  detail::value_type_helper vh;
  std::string const wanted_type =
    uniform_type_name(cet::demangle_symbol(iToType.name()));
  static size_t pos = vh.look_past_pair<T>();
  auto const it = obj.findOrThrow(cet::map_vector_key{iIndex});
  assert(it != obj.end());
  if ((pos < wanted_type.size()) && vh.starts_with_pair(wanted_type, pos)) {
    // Want value_type, not mapped_type;
    oPtr = detail::maybeCastObj(&*it, iToType);
  } else {
    oPtr = detail::maybeCastObj(&it->second, iToType);
  }
}

#endif /* canvas_Persistency_Common_setPtr_h */

// Local Variables:
// mode: c++
// End:
