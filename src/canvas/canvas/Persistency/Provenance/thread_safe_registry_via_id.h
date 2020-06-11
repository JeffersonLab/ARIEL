#ifndef canvas_Persistency_Provenance_thread_safe_registry_via_id_h
#define canvas_Persistency_Provenance_thread_safe_registry_via_id_h

// ===================================================================
// thread_safe_registry_via_id
//
// Provides safe access to registry, which never shrinks, but can grow
// and be read from multiple threads.
//
// This DOES NOT prevent reading during a write, nor vice versa.  If
// one needs to make sure that the contents of the registry are not
// changing during a read, then a guard must be placed around the
// registry traversal.
//
// Inefficiencies:
//
// - To insert pairs from an std::map<K,M> object, one must emplace
//   each pair one at a time.  This seems to be very inefficient;
//   however, unless we are willing to pay the consequences for
//   changing the underlying types of some of the persisted objects
//   that are represented in memory by the below registry, there may
//   not be a better solution.
// ===================================================================

#include "canvas/Persistency/Provenance/Hash.h"
#include "tbb/concurrent_unordered_map.h"

#include <type_traits>
#include <functional>

namespace art {

  namespace detail {
    template <typename T>
    struct is_instantiation_of_hash : std::false_type {
    };

    template <int I>
    struct is_instantiation_of_hash<art::Hash<I>> : std::true_type {
    };

    template <typename H>
    struct hash_to_size_t {

      std::enable_if_t<is_instantiation_of_hash<H>::value, std::size_t>
      operator()(H const& id) const
      {
        std::ostringstream os;
        id.print(os);
        return std::hash<std::string>{}(os.str());
      }
    };
  }

  template <typename K, typename M>
  class thread_safe_registry_via_id {
  public:
    using collection_type =
      tbb::concurrent_unordered_map<K, M, detail::hash_to_size_t<K>>;
    using value_type = typename collection_type::value_type;

    template <typename C>
    static void put(C const& container);
    static auto emplace(value_type const& value);
    static auto emplace(K const& key, M const& mapped);
    static bool empty();
    static collection_type const& get();
    static bool get(K const& key, M& mapped);

  private:
    static auto&
    instance()
    {
      static collection_type me;
      return me;
    }
  };

  template <typename K, typename M>
  template <typename C>
  void
  thread_safe_registry_via_id<K, M>::put(C const& container)
  {
    auto& me = instance();
    for (auto const& e : container) {
      me.emplace(e);
    }
  }

  template <typename K, typename M>
  auto
  thread_safe_registry_via_id<K, M>::emplace(value_type const& value)
  {
    return instance().emplace(value);
  }

  template <typename K, typename M>
  auto
  thread_safe_registry_via_id<K, M>::emplace(K const& key, M const& mapped)
  {
    return instance().emplace(key, mapped);
  }

  template <typename K, typename M>
  bool
  thread_safe_registry_via_id<K, M>::empty()
  {
    return instance().empty();
  }

  template <typename K, typename M>
  auto
  thread_safe_registry_via_id<K, M>::get() -> collection_type const&
  {
    return instance();
  }

  template <typename K, typename M>
  bool
  thread_safe_registry_via_id<K, M>::get(K const& k, M& mapped)
  {
    auto& me = instance();
    auto it = me.find(k);
    if (it != me.cend()) {
      mapped = it->second;
      return true;
    }
    return false;
  }
}

#endif /* canvas_Persistency_Provenance_thread_safe_registry_via_id_h */

// Local Variables:
// mode: c++
// End:
