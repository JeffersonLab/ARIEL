#ifndef cetlib_registry_via_id_h
#define cetlib_registry_via_id_h

// ======================================================================
//
// registry_via_id<K,V>: A singleton std::map<K,V> requiring that V::id()
//                       exists and yields values of type K
//
// ======================================================================

#include "cetlib_except/exception.h"

#include <iterator>
#include <map>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace cet {
  template <class K, class V>
  class registry_via_id;

  namespace detail {
    template <class K, class V, K (V::*)() const = &V::id>
    struct must_have_id {
      typedef K type;
    };
  }
}

// ======================================================================

template <class K, class V>
class cet::registry_via_id {
  // non-instantiable (and non-copyable, just in case):
  registry_via_id() = delete;
  registry_via_id(registry_via_id const&) = delete;
  void operator=(registry_via_id const&) = delete;

public:
  typedef std::map<K const, V> collection_type;
  typedef typename collection_type::key_type key_type;
  typedef typename collection_type::mapped_type mapped_type;
  typedef typename collection_type::value_type value_type;
  typedef typename collection_type::size_type size_type;
  typedef typename collection_type::const_iterator const_iterator;

  // observers:
  static bool
  empty()
  {
    return the_registry_().empty();
  }
  static size_type
  size()
  {
    return the_registry_().size();
  }

  // iterators:
  static const_iterator
  begin()
  {
    return the_registry_().begin();
  }
  static const_iterator
  end()
  {
    return the_registry_().end();
  }
  static const_iterator
  cbegin()
  {
    return the_registry_().cbegin();
  }
  static const_iterator
  cend()
  {
    return the_registry_().cend();
  }

  // mutators:
  // A single V;
  static typename detail::must_have_id<K, V>::type put(V const& value);
  // A range of iterator to V.
  template <class FwdIt>
  static std::enable_if_t<
    std::is_same<typename std::iterator_traits<FwdIt>::value_type,
                 mapped_type>::value,
    void>
  put(FwdIt begin, FwdIt end);
  // A range of iterator to std::pair<K, V>. For each pair, first ==
  // second.id() is a prerequisite.
  template <class FwdIt>
  static std::enable_if_t<
    std::is_same<typename std::iterator_traits<FwdIt>::value_type,
                 value_type>::value,
    void>
  put(FwdIt begin, FwdIt end);
  // A collection_type. For each value_type, first == second.id() is a
  // prerequisite.
  static void put(collection_type const& c);

  // accessors:
  static collection_type const&
  get() noexcept
  {
    return the_registry_();
  }
  static V const& get(K const& key);
  static bool get(K const& key, V& value) noexcept;

private:
  // encapsulated singleton:
  static collection_type&
  the_registry_()
  {
    static collection_type the_registry;
    return the_registry;
  }

}; // registry_via_id<>

// ----------------------------------------------------------------------
// put() overloads:

template <class K, class V>
typename cet::detail::must_have_id<K, V>::type
cet::registry_via_id<K, V>::put(V const& value)
{
  K id = value.id();
  the_registry_().emplace(id, value);
  return id;
}

template <class K, class V>
template <class FwdIt>
inline auto
cet::registry_via_id<K, V>::put(FwdIt b, FwdIt e) -> std::enable_if_t<
  std::is_same<typename std::iterator_traits<FwdIt>::value_type,
               mapped_type>::value,
  void>
{
  for (; b != e; ++b)
    (void)put(*b);
}

template <class K, class V>
template <class FwdIt>
inline auto
cet::registry_via_id<K, V>::put(FwdIt b, FwdIt e) -> std::enable_if_t<
  std::is_same<typename std::iterator_traits<FwdIt>::value_type,
               value_type>::value,
  void>
{
  the_registry_().insert(b, e);
}

template <class K, class V>
inline void
cet::registry_via_id<K, V>::put(collection_type const& c)
{
  put(c.cbegin(), c.cend());
}

// ----------------------------------------------------------------------
// get() overloads:

template <class K, class V>
V const&
cet::registry_via_id<K, V>::get(K const& key)
{
  const_iterator it = the_registry_().find(key);
  if (it == the_registry_().end())
    throw cet::exception("cet::registry_via_id")
      << "Key \"" << key << "\" not found in registry";
  return it->second;
}

template <class K, class V>
bool
cet::registry_via_id<K, V>::get(K const& key, V& value) noexcept
{
  bool result;
  const_iterator it = the_registry_().find(key);
  if (it == the_registry_().end()) {
    result = false;
  } else {
    value = it->second;
    result = true;
  }
  return result;
}

#endif /* cetlib_registry_via_id_h */

// Local Variables:
// mode: c++
// End:
