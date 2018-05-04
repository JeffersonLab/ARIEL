/**
 * @file ForEachAssociatedGroup.h
 * @brief  Helper functions to access associations in order
 *
 * No additional linking is required to use these functions.
 *
 * Provided functions:
 *
 * * `art::for_each_group()` executing the provided function on each
 *   of the elements associated to the same object, for each object
 *
 * * `art::for_each_group_with_left()` executing the provided function
 *   on each of the elements associated to the same object, for each
 *   object, while also providing a reference to the object.
 */

#ifndef canvas_Persistency_Common_AssnsAlgorithms_h
#define canvas_Persistency_Common_AssnsAlgorithms_h

#include "canvas/Persistency/Common/Assns.h"

// range library
#include "range/v3/algorithm/for_each.hpp"
#include "range/v3/view/all.hpp"
#include "range/v3/view/group_by.hpp"
#include "range/v3/view/map.hpp"
#include "range/v3/view/transform.hpp"

// C/C++ standard libraries
#include <iterator> // std::next()

namespace art {
  /**
   * @brief  Helper functions to access associations in order
   * @tparam A type of association being read
   * @tparam F type of functor to be called on each associated group
   * @param assns the association being read
   * @param func functor to be called on each associated group
   *
   * This function takes two input arguments, a constant reference to
   * the association data product itself (`assns`), and the function
   * (`func`) to be operated on each of the group of associated objects.
   * This function represents the association data product as
   * a range of ranges representing the right hand side in the
   * collection, hence the function provided as the second argument
   * should assume that it will be operating on a range of art::Ptr
   * to the associated data products grouped by the data product
   * they are associated with.
   *
   * Example: assuming that a module with input tag stored in `fTrackTag` has
   * created associations of each track to its hits, the total charge for each
   * track can be extracted by:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto assns = art::getValidHandle<art::Assns<recob::Track, recob::Hit>>
   *   (fTrackTag);
   *
   * std::vector<double> totalCharge;
   * art::for_each_group(*assns,
   *   [&totalCharge](auto hits)
   *     {
   *       double total = 0.;
   *       for (auto iHit = begin(hits); iHit != end(hits); ++iHit)
   *         total += (*iHit)->Integral();
   *       totalCharge.push_back(total);
   *     }
   *   );
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * A number of important points need to be realised about this example:
   *
   *  * the requirements of this function translate, for this example, into:
   *      * at the creation of the associations data product, for each track the
   *        _complete_ sequence of hits must be associated, one hit after the
   *        other (via `addSingle`); if the order of the hits is relevant (it is
   *        not in this specific example), hits must be associated in that order
   *      * each track must have at least one associated hit;
   *      * the original association has to be stored with `recob::Track` as
   *        _left_ key and `recob::Hit` as _right_ key;
   *  * we use here a lambda function as `func`; any object
   *    behaving as a function and able to accept the range of hits as its only
   *    argument will work just as well;
   *  * `func` will be called once for every track (but if a track has no
   *    associated hit, that track will be skipped, and if a track appears in
   *    more than one association sequence, like in (T1,H1) (T1,H2) (T2,H4)
   *    (T1,H3), then that track will appear as many times);
   *  * `func` does not return any value: the results are accumulated in a new
   *    sequence;
   *  * `func` receives a range of _art_ pointers (`art::Ptr<recob::Hit>`) which
   *    needs to be navigated with the `begin()`/`end()` free functions (don't
   *    specify their namespace: C++ will figure out!); double dereferencing
   *    is needed: the first (`*iHit`) will turn the range iterator into the
   *    pointed `art::Ptr<recob::Hit>`, and the second (`(...)->Integral()`)
   *    accesses the `recob::Hit` pointed by the _art_ pointer;
   *  * `func` does not know which track the hits are associated to, and it
   *    assumes that the first sequence of hits is associated to the first track
   *    (so that its total charge will be available in `totalCharge[0]`), the
   *    second sequence to the second track (`totalCharge[1]`) and so on.
   *
   * Therefore, for a `assns` sequence like
   *
   *     (T1,H1) (T1,H2) (T2,H4) (T2,H6) (T1,H5)
   *
   * the function `for_each_group()` will execute two calls:
   *
   *     func({ H1, H2 })
   *     func({ H4, H6, H5 })
   *
   * dealing with the hits associated to `T1` first, and `T2` next.
   */
  template <typename A, typename B, typename D, typename F>
  void
  for_each_group(art::Assns<A, B, D> const& assns, F func)
  {
    ranges::for_each(assns | ranges::view::all |
                       ranges::view::group_by([](auto a1, auto a2) {
                         return a1.first == a2.first;
                       }) |
                       ranges::view::transform([](auto pairs) {
                         return pairs | ranges::view::values;
                       }),
                     func);
  }

  /*
   * @brief  Helper functions to access associations in order
   * @tparam A type of association being read
   * @tparam F type of functor to be called on each LHS and associated RHS
   * @param assns the association being read
   * @param func functor to be called on each LHS and associated RHS
   *
   * This function performs the following:
   *  -- takes an association collection as the first argument
   *  -- calls the provided callable object
   * For example, given an art::Assns<L, R>, it will transform this
   * collection to a form that supports calling a function with two arguments
   * (auto const & left, auto rights)
   * The provided callable is invoked for each unique left.
   *
   */
  template <typename A, typename B, typename D, typename F>
  void
  for_each_group_with_left(art::Assns<A, B, D> const& assns, F func)
  {
    for_each_pair(assns, [&func](auto rng) {
      auto rights = rng | ranges::view::values;
      auto lefts = rng | ranges::view::keys;
      auto const& left = **ranges::begin(lefts);
      func(left, rights);
    });
  }

  template <typename A, typename B, typename D, typename F>
  void
  for_each_pair(art::Assns<A, B, D> const& assns, F func)
  {
    ranges::for_each(assns | ranges::view::all |
                       ranges::view::group_by(
                         [](auto a1, auto a2) { return a1.first == a2.first; }),
                     func);
  }

} // namespace art

#endif /* canvas_Persistency_Common_AssnsAlgorithms_h */

// Local Variables:
// mode: c++
// End:
