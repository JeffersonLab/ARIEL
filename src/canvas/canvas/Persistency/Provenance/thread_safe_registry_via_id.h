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
#include "hep_concurrency/RecursiveMutex.h"
#include "hep_concurrency/tsan.h"

#include <map>
#include <type_traits>

namespace art {
  template <typename K, typename M>
  class thread_safe_registry_via_id {
  public: // Types
    using collection_type = std::map<K, M>;
    using value_type = typename collection_type::value_type;

  public: // Static API
    template <typename C>
    static void put(C const& container);
    static auto emplace(value_type const& value);
    static auto emplace(K const& key, M const& mapped);
    static bool empty();
    static collection_type const& get();
    static bool get(K const& key, M& mapped);
    static auto&
    getMutex()
    {
      static hep::concurrency::RecursiveMutex m{"art::tsr<K,M>::m"};
      return m;
    }
    static auto
    instance(bool cleanup = false)
    {
      hep::concurrency::RecursiveMutexSentry sentry{getMutex(), __func__};
      static collection_type* me = new collection_type{};
      if (cleanup) {
        delete me;
        me = nullptr;
        return me;
      }
      if (me == nullptr) {
        // We have been cleaned up and are now being used again.
        me = new collection_type{};
      }
      return me;
    }
  };

  template <typename K, typename M>
  template <typename C>
  void
  thread_safe_registry_via_id<K, M>::put(C const& container)
  {
    hep::concurrency::RecursiveMutexSentry sentry{getMutex(), __func__};
    auto me = instance();
    for (auto const& e : container) {
      me->emplace(e);
    }
  }

  template <typename K, typename M>
  auto
  thread_safe_registry_via_id<K, M>::emplace(value_type const& value)
  {
    hep::concurrency::RecursiveMutexSentry sentry{getMutex(), __func__};
    auto ret = instance()->emplace(value);
    return ret;
  }

  template <typename K, typename M>
  auto
  thread_safe_registry_via_id<K, M>::emplace(K const& key, M const& mapped)
  {
    hep::concurrency::RecursiveMutexSentry sentry{getMutex(), __func__};
    auto ret = instance()->emplace(key, mapped);
    return ret;
  }

  template <typename K, typename M>
  bool
  thread_safe_registry_via_id<K, M>::empty()
  {
    hep::concurrency::RecursiveMutexSentry sentry{getMutex(), __func__};
    auto ret = instance()->empty();
    return ret;
  }

  template <typename K, typename M>
  auto
  thread_safe_registry_via_id<K, M>::get() -> collection_type const&
  {
    auto const& ret = *instance();
    return ret;
  }

  template <typename K, typename M>
  bool
  thread_safe_registry_via_id<K, M>::get(K const& k, M& mapped)
  {
    hep::concurrency::RecursiveMutexSentry sentry{getMutex(), __func__};
    auto me = instance();
    auto it = me->find(k);
    if (it != me->cend()) {
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
