#ifndef canvas_Persistency_Common_AssnsNode_h
#define canvas_Persistency_Common_AssnsNode_h

#include "canvas/Persistency/Common/Ptr.h"

#include <type_traits>

namespace art {

  // for dereference return type
  template <typename L, typename R, typename D>
  struct AssnsNode {
    using first_type = Ptr<L>;
    using second_type = Ptr<R>;
    using data_type = D;
    first_type first{};
    second_type second{};
    D const* data{nullptr};
    AssnsNode() = default;
    AssnsNode(art::Ptr<L> const& l, art::Ptr<R> const& r, D const& d)
      : first{l}, second{r}, data{&d}
    {}
  };

  //=======================================
  // tuple_element<> interface
  namespace detail {
    template <std::size_t I, typename Node>
    struct tuple_element;

    template <typename L, typename R, typename D>
    struct tuple_element<0, AssnsNode<L, R, D>> {
      using type = Ptr<L>;
    };

    template <typename L, typename R, typename D>
    struct tuple_element<1, AssnsNode<L, R, D>> {
      using type = Ptr<R>;
    };

    template <typename L, typename R, typename D>
    struct tuple_element<2, AssnsNode<L, R, D>> {
      using type = D;
    };
  }

  template <std::size_t I, typename Node>
  struct tuple_element;

  template <std::size_t I, typename L, typename R, typename D>
  struct tuple_element<I, AssnsNode<L, R, D>> {
    using type = typename detail::tuple_element<I, AssnsNode<L, R, D>>::type;
  };

  template <std::size_t I, typename Node>
  using tuple_element_t = typename tuple_element<I, Node>::type;

  //=======================================
  // get<> interface
  namespace detail {
    template <typename L, typename R, typename D>
    struct get_helper {
      using node_t = AssnsNode<L, R, D>;
      using first_type = typename node_t::first_type;
      using second_type = typename node_t::second_type;

      // Indexed accessors
      template <std::size_t I>
      static std::enable_if_t<(I == 0), tuple_element_t<I, node_t> const&>
      get_element(node_t const& r)
      {
        return r.first;
      }

      template <std::size_t I>
      static std::enable_if_t<(I == 1), tuple_element_t<I, node_t> const&>
      get_element(node_t const& r)
      {
        return r.second;
      }

      template <std::size_t I>
      static std::enable_if_t<(I == 2), tuple_element_t<I, node_t> const&>
      get_element(node_t const& r)
      {
        return *r.data;
      }

      // By-type accessors
      template <typename T>
      static std::enable_if_t<std::is_same_v<T, first_type>, T const&>
      get_element(node_t const& r)
      {
        return r.first;
      }

      template <typename T>
      static std::enable_if_t<std::is_same_v<T, second_type>, T const&>
      get_element(node_t const& r)
      {
        return r.second;
      }

      template <typename T>
      static std::enable_if_t<std::is_same_v<T, D>, T const&>
      get_element(node_t const& r)
      {
        return *r.data;
      }
    };
  }

  template <typename T, typename L, typename R, typename D>
  auto const&
  get(AssnsNode<L, R, D> const& r)
  {
    return detail::get_helper<L, R, D>::template get_element<T>(r);
  }

  template <std::size_t I, typename L, typename R, typename D>
  auto const&
  get(AssnsNode<L, R, D> const& r)
  {
    return detail::get_helper<L, R, D>::template get_element<I>(r);
  }
}

#endif /* canvas_Persistency_Common_AssnsNode_h */

// Local Variables:
// mode: c++
// End:
