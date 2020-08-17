#ifndef canvas_Persistency_Common_detail_aggregate_h
#define canvas_Persistency_Common_detail_aggregate_h

#include "canvas/Utilities/Exception.h"
#include "cetlib/container_algorithms.h"
#include "cetlib/map_vector.h"
#include "cetlib/metaprogramming.h"
#include "cetlib_except/demangle.h"

#include <typeinfo>

#include <array>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <utility>
#include <vector>

namespace CLHEP {
  class HepVector;
  class Hep2Vector;
  class Hep3Vector;
  class HepLorentzVector;
  class HepMatrix;
  class HepSymMatrix;
}

class TH1;

namespace art {
  namespace detail {

    using cet::enable_if_function_exists_t;

    template <typename T, typename = void>
    struct has_aggregate : std::false_type {};

    template <typename T>
    struct has_aggregate<
      T,
      enable_if_function_exists_t<void (T::*)(T const&), &T::aggregate>>
      : std::true_type {};

    template <typename T>
    struct has_aggregate<
      T,
      enable_if_function_exists_t<void (T::*)(T), &T::aggregate>>
      : std::true_type {};

    template <typename T, typename Enable = void>
    struct CanBeAggregated : std::false_type {
      static void
      aggregate(T&, T const&)
      {
        throw art::Exception(art::errors::ProductCannotBeAggregated)
          << "Products of type \"" << cet::demangle_symbol(typeid(T).name())
          << "\" cannot be aggregated.\n"
          << "Please contact artists@fnal.gov.\n";
      }
    };

    // Arithmetic
    template <typename T>
    struct CanBeAggregated<T, std::enable_if_t<std::is_arithmetic<T>::value>>
      : std::true_type {
      static void
      aggregate(T& p, T const other)
      {
        p += other;
      }
    };

    // User-defined
    template <typename T>
    struct CanBeAggregated<T, std::enable_if_t<has_aggregate<T>::value>>
      : std::true_type {
      static void
      aggregate(T& p, T const& other)
      {
        p.aggregate(other);
      }
    };

    template <typename T>
    struct CanBeAggregated<std::vector<T>> : std::true_type {
      static void
      aggregate(std::vector<T>& p, std::vector<T> const& other)
      {
        p.insert(p.cend(), other.cbegin(), other.cend());
      }
    };

    template <typename T>
    struct CanBeAggregated<std::list<T>> : std::true_type {
      static void
      aggregate(std::list<T>& p, std::list<T> const& other)
      {
        p.insert(p.cend(), other.cbegin(), other.cend());
      }
    };

    template <typename T>
    struct CanBeAggregated<std::deque<T>> : std::true_type {
      static void
      aggregate(std::deque<T>& p, std::deque<T> const& other)
      {
        p.insert(p.cend(), other.cbegin(), other.cend());
      }
    };

    // std::array not currently supported by ROOT6
    template <typename T, size_t N>
    struct CanBeAggregated<std::array<T, N>> : std::true_type {
      static void
      aggregate(std::array<T, N>& p, std::array<T, N> const& other)
      {
        cet::transform_all(p, other, begin(p), [](T t1, T const& t2) {
          CanBeAggregated<T>::aggregate(t1, t2);
          return t1;
        });
      }
    };

    // Implementation details for Tuple
    template <std::size_t>
    struct AggregateTuple;

    template <>
    struct AggregateTuple<0u> {
      template <typename Tuple>
      static void
      combine(Tuple&, Tuple const&)
      {}
    };

    template <std::size_t I>
    struct AggregateTuple {
      template <typename Tuple>
      static void
      combine(Tuple& p, Tuple const& other)
      {
        using elem_type = std::tuple_element_t<I, Tuple>;
        CanBeAggregated<elem_type>::aggregate(std::get<I>(p),
                                              std::get<I>(other));
        AggregateTuple<I - 1>::combine(p, other);
      }
    };

    // std::tuple not currently supported by ROOT6
    template <typename... Args>
    struct CanBeAggregated<std::tuple<Args...>> : std::true_type {
      static void
      aggregate(std::tuple<Args...>& p, std::tuple<Args...> const& other)
      {
        AggregateTuple<sizeof...(Args) - 1>::combine(p, other);
      }
    };

    template <typename K, typename V>
    struct CanBeAggregated<std::map<K, V>> : std::true_type {
      static void
      aggregate(std::map<K, V>& p, std::map<K, V> const& other)
      {
        // Maybe throw exception if insert fails.
        p.insert(other.cbegin(), other.cend());
      }
    };

    template <typename K, typename V>
    struct CanBeAggregated<std::pair<K, V>> : std::true_type {
      static void
      aggregate(std::pair<K, V>& p, std::pair<K, V> const& other)
      {
        CanBeAggregated<K>::aggregate(p.first, other.first);
        CanBeAggregated<V>::aggregate(p.second, other.second);
      }
    };

    template <typename K, typename V>
    struct CanBeAggregated<std::multimap<K, V>> : std::true_type {
      static void
      aggregate(std::multimap<K, V>& p, std::multimap<K, V> const& other)
      {
        p.insert(other.cbegin(), other.cend());
      }
    };

    template <typename T>
    struct CanBeAggregated<std::set<T>> : std::true_type {
      static void
      aggregate(std::set<T>& p, std::set<T> const& other)
      {
        // Maybe throw exception if insert fails.
        p.insert(other.cbegin(), other.cend());
      }
    };

    template <typename T>
    struct CanBeAggregated<cet::map_vector<T>> : std::true_type {
      static void
      aggregate(cet::map_vector<T>& p, cet::map_vector<T> const& other)
      {
        // Maybe throw exception if insert fails.
        p.insert(other.cbegin(), other.cend());
      }
    };

    // Discuss with stakeholders
    template <>
    struct CanBeAggregated<std::string> : std::true_type {
      static void
      aggregate(std::string& p, std::string const& other)
      {
        if (p != other)
          throw art::Exception(art::errors::ProductCannotBeAggregated)
            << "Products of type \""
            << cet::demangle_symbol(typeid(std::string).name())
            << "\" cannot be aggregated unless their values are the same.\n"
            << "Values presented were: \"" << p << "\" and \"" << other
            << "\".\n";
      }
    };

    //==============================================================
    // CLHEP specializations

    template <>
    struct CanBeAggregated<CLHEP::HepVector> : std::true_type {
      static void aggregate(CLHEP::HepVector& p, CLHEP::HepVector const& other);
    };

    template <>
    struct CanBeAggregated<CLHEP::Hep2Vector> : std::true_type {
      static void aggregate(CLHEP::Hep2Vector& p,
                            CLHEP::Hep2Vector const& other);
    };

    template <>
    struct CanBeAggregated<CLHEP::Hep3Vector> : std::true_type {
      static void aggregate(CLHEP::Hep3Vector& p,
                            CLHEP::Hep3Vector const& other);
    };

    template <>
    struct CanBeAggregated<CLHEP::HepLorentzVector> : std::true_type {
      static void aggregate(CLHEP::HepLorentzVector& p,
                            CLHEP::HepLorentzVector const& other);
    };

    template <>
    struct CanBeAggregated<CLHEP::HepMatrix> : std::true_type {
      static void aggregate(CLHEP::HepMatrix& p, CLHEP::HepMatrix const& other);
    };

    template <>
    struct CanBeAggregated<CLHEP::HepSymMatrix> : std::true_type {
      static void aggregate(CLHEP::HepSymMatrix& p,
                            CLHEP::HepSymMatrix const& other);
    };

    //==============================================================
    // ROOT-TH1 specializations
    //
    // .. Do not include TH1.h!  Adding the TH1.h header causes woe
    //    since it introduces a definition of a static variable of
    //    type TVersionCheck, whose constructor is defined in the
    //    ROOT_CORE library.  This causes users to link essentially
    //    EVERYTHING against ROOT_CORE.  This is not a problem for
    //    modules/source (they already depend on ROOT_CORE), but it
    //    would introduce an inherent dependency on ROOT for services
    //    as well.  Fortunately, the std::is_base_of<Base,Derived>
    //    implementation only requires that Derived (T) be a complete
    //    type, and not that of Base (TH1).

    template <typename T>
    struct CanBeAggregated<T, std::enable_if_t<std::is_base_of<TH1, T>::value>>
      : std::true_type {
      static void
      aggregate(T& p, T const& other)
      {
        p.Add(&other);
      }
    };
  }
}

#endif /* canvas_Persistency_Common_detail_aggregate_h */

// Local variables:
// mode: c++
// End:
