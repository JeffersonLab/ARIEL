#ifndef cetlib_registry_h
#define cetlib_registry_h

// ======================================================================
//
// registry<K,V>: A singleton std::map<K,V>
//
// ======================================================================

#include "cetlib_except/exception.h"
#include <map>
#include <stdexcept>
#include <utility>

namespace cet {
  template <class K, class V>
  class registry;
}

// ======================================================================

template <class K, class V>
class cet::registry {
  // non-instantiable (and non-copyable, just in case):
  registry() = delete;
  registry(registry const&) = delete;
  void operator=(registry const&) = delete;

  // shorthand:
  typedef std::map<K const, V> reg_t;
  typedef typename reg_t::const_iterator iter_t;

public:
  typedef iter_t const_iterator;

  static bool
  empty()
  {
    return the_registry_().empty();
  }
  static typename reg_t::size_type
  size()
  {
    return the_registry_().size();
  }

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

  static void put(K const& key, V const& value);

  static V const& get(K const& key);
  static bool get(K const& key, V& value) throw();

private:
  // encapsulated singleton:
  static reg_t&
  the_registry_()
  {
    static reg_t the_registry;
    return the_registry;
  }

}; // registry<>

// ----------------------------------------------------------------------

template <class K, class V>
void
cet::registry<K, V>::put(K const& key, V const& value)
{
  the_registry_().insert(std::make_pair(key, value));
}

template <class K, class V>
V const&
cet::registry<K, V>::get(K const& key)
{
  iter_t it = the_registry_().find(key);
  if (it == the_registry_().end())
    throw cet::exception("cet::registry")
      << "Key \"" << key << "\" not found in registry";
  return it->second;
}

template <class K, class V>
bool
cet::registry<K, V>::get(K const& key, V& value) throw() try {
  value = get(key);
  return true;
}
catch (cet::exception const&) {
  return false;
}

// ======================================================================

#endif /* cetlib_registry_h */

// Local Variables:
// mode: c++
// End:
