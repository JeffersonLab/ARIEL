#ifndef canvas_Persistency_Common_getElementAddresses_h
#define canvas_Persistency_Common_getElementAddresses_h

#include "canvas/Persistency/Common/detail/maybeCastObj.h"
#include "canvas/Utilities/uniform_type_name.h"

#include <string>
#include <typeinfo>
#include <vector>

namespace art {
  template <typename Collection>
  void getElementAddresses(Collection const& coll,
                           std::type_info const& iToType,
                           std::vector<unsigned long> const& indices,
                           std::vector<void const*>& oPtr);

  template <typename T>
  void getElementAddresses(cet::map_vector<T> const& obj,
                           std::type_info const& iToType,
                           std::vector<unsigned long> const& indices,
                           std::vector<void const*>& oPtr);

  namespace detail {
    class value_type_helper;
  }
}

class art::detail::value_type_helper {
public:
  static std::string const&
  pair_stem()
  {
    static std::string const pair_stem_s("std::pair<");
    return pair_stem_s;
  }

  static size_t
  pair_stem_offset()
  {
    static size_t const pair_stem_offset_s = pair_stem().size();
    return pair_stem_offset_s;
  }

  bool
  starts_with_pair(std::string const& type_name, size_t const pos)
  {
    return type_name.compare(pos, pair_stem_offset(), pair_stem()) == 0;
  }

  template <typename T>
  size_t
  look_past_pair()
  {
    static std::string const mapped_type =
      cet::demangle_symbol(typeid(T).name());
    size_t pos = 0;
    while (starts_with_pair(mapped_type, pos)) {
      pos += pair_stem_offset();
    }
    return pos;
  }
};

template <typename Collection>
void
art::getElementAddresses(Collection const& coll,
                         std::type_info const& iToType,
                         std::vector<unsigned long> const& indices,
                         std::vector<void const*>& oPtr)
{
  oPtr.reserve(indices.size());
  for (auto const index : indices) {
    auto it = coll.cbegin();
    advance(it, index);
    oPtr.push_back(detail::maybeCastObj(
      detail::GetProduct<Collection>::address(it), iToType));
  }
}

template <typename T>
void
art::getElementAddresses(cet::map_vector<T> const& obj,
                         std::type_info const& iToType,
                         std::vector<unsigned long> const& indices,
                         std::vector<void const*>& oPtr)
{
  detail::value_type_helper vh;
  std::string const wanted_type =
    uniform_type_name(cet::demangle_symbol(iToType.name()));
  static size_t const pos = vh.look_past_pair<T>();
  oPtr.reserve(indices.size());
  if ((pos < wanted_type.size()) && vh.starts_with_pair(wanted_type, pos)) {
    // Want value_type.
    for (auto const index : indices) {
      auto it = obj.find(cet::map_vector_key{index});
      auto ptr = (it == obj.cend()) ? nullptr : &*it;
      oPtr.push_back(detail::maybeCastObj(ptr, iToType));
    }
  } else {
    // Want mapped_type.
    for (auto const index : indices) {
      auto ptr = obj.getOrNull(cet::map_vector_key{index});
      oPtr.push_back(detail::maybeCastObj(ptr, iToType));
    }
  }
}

#endif /* canvas_Persistency_Common_getElementAddresses_h */

// Local Variables:
// mode: c++
// End:
