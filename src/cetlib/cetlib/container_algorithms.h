#ifndef cetlib_container_algorithms_h
#define cetlib_container_algorithms_h

// ======================================================================
//
// container_algorithms: Function templates wrapping standard algorithms
//
// Avoids incompatible iterators and reduces some duplication
//
// ======================================================================

#include <algorithm>
#include <iterator>

namespace cet {

  // Wrappers for std::for_each().
  template <class FwdCont, class Func>
  auto for_all(FwdCont&, Func);
  template <class FwdCont, class Func>
  auto for_all(FwdCont const&, Func);

  // Wrappers for std::copy().
  template <class FwdCont, class FwdIter>
  auto copy_all(FwdCont&, FwdIter);
  template <class FwdCont, class FwdIter>
  auto copy_all(FwdCont const&, FwdIter);

  // Wrappers for std::copy_if().
  template <class FwdCont, class FwdIter, class Pred>
  auto copy_if_all(FwdCont&, FwdIter, Pred);
  template <class FwdCont, class FwdIter, class Pred>
  auto copy_if_all(FwdCont const&, FwdIter, Pred);

  // Wrappers for std::find().
  template <class FwdCont, class Datum>
  auto find_in_all(FwdCont&, Datum const&);
  template <class FwdCont, class Datum>
  auto find_in_all(FwdCont const&, Datum const&);
  template <class FwdCont, class Datum>
  bool search_all(FwdCont const&, Datum const&);

  // Wrapper for std::binary_search().
  template <class FwdCont, class Datum>
  bool binary_search_all(FwdCont const&, Datum const&);

  // Wrappers for std::lower_bound().
  template <class FwdCont, class Datum>
  auto lower_bound_all(FwdCont&, Datum const&);
  template <class FwdCont, class Datum>
  auto lower_bound_all(FwdCont const&, Datum const&);
  template <class FwdCont, class Datum, class Pred>
  auto lower_bound_all(FwdCont&, Datum const&, Pred);
  template <class FwdCont, class Datum, class Pred>
  auto lower_bound_all(FwdCont const&, Datum const&, Pred);

  // Wrappers for std::sort().
  template <class RandCont>
  void sort_all(RandCont&);
  template <class RandCont, class Pred>
  void sort_all(RandCont&, Pred);

  // Wrappers for std::stable_sort().
  template <class RandCont>
  void stable_sort_all(RandCont&);
  template <class RandCont, class Pred>
  void stable_sort_all(RandCont&, Pred);

  // Wrappers for std::transform().
  // One-container input.
  template <class Container, class OutputIt, class UnaryOp>
  auto transform_all(Container&, OutputIt, UnaryOp);
  template <class Container, class OutputIt, class UnaryOp>
  auto transform_all(Container const&, OutputIt, UnaryOp);
  // Two-container input.
  template <class Container1, class Container2, class OutputIt, class BinaryOp>
  auto transform_all(Container1&, Container2&, OutputIt, BinaryOp);
  template <class Container1, class Container2, class OutputIt, class BinaryOp>
  auto transform_all(Container1 const&, Container2 const&, OutputIt, BinaryOp);

  template <class FwdCont, class Func>
  void for_all_with_index(FwdCont&, Func);
  template <class FwdCont, class Func>
  void for_all_with_index(FwdCont const&, Func);
}

// for_all().
template <class FwdCont, class Func>
inline auto
cet::for_all(FwdCont& s, Func f)
{
  using std::begin;
  using std::end;
  return std::for_each(begin(s), end(s), f);
}

template <class FwdCont, class Func>
inline auto
cet::for_all(FwdCont const& s, Func f)
{
  using std::cbegin;
  using std::cend;
  return std::for_each(cbegin(s), cend(s), f);
}

// for_all_with_index().
template <class FwdCont, class Func>
inline void
cet::for_all_with_index(FwdCont& s, Func f)
{
  std::size_t i{};
  for (auto& e : s) {
    f(i, e);
    ++i;
  }
}

template <class FwdCont, class Func>
inline void
cet::for_all_with_index(FwdCont const& s, Func f)
{
  std::size_t i{};
  for (auto const& e : s) {
    f(i, e);
    ++i;
  }
}

// copy_all().
template <class FwdCont, class FwdIter>
inline auto
cet::copy_all(FwdCont& s, FwdIter it)
{
  using std::begin;
  using std::end;
  return std::copy(begin(s), end(s), it);
}

template <class FwdCont, class FwdIter>
inline auto
cet::copy_all(FwdCont const& s, FwdIter it)
{
  using std::cbegin;
  using std::cend;
  return std::copy(cbegin(s), cend(s), it);
}

// copy_if_all().
template <class FwdCont, class FwdIter, class Pred>
inline auto
cet::copy_if_all(FwdCont& s, FwdIter it, Pred p)
{
  using std::cbegin;
  using std::cend;
  return std::copy_if(begin(s), end(s), it, p);
}

template <class FwdCont, class FwdIter, class Pred>
inline auto
cet::copy_if_all(FwdCont const& s, FwdIter it, Pred p)
{
  using std::cbegin;
  using std::cend;
  return std::copy_if(cbegin(s), cend(s), it, p);
}

// Find_in_all().
template <class FwdCont, class Datum>
inline auto
cet::find_in_all(FwdCont& s, Datum const& d)
{
  using std::begin;
  using std::end;
  return std::find(begin(s), end(s), d);
}

template <class FwdCont, class Datum>
inline auto
cet::find_in_all(FwdCont const& s, Datum const& d)
{
  using std::cbegin;
  using std::cend;
  return std::find(cbegin(s), cend(s), d);
}

// search_all().
template <class FwdCont, class Datum>
inline bool
cet::search_all(FwdCont const& s, Datum const& d)
{
  using std::cbegin;
  using std::cend;
  return std::find(cbegin(s), cend(s), d) != s.end();
}

// binary_search_all().
template <class FwdCont, class Datum>
inline bool
cet::binary_search_all(FwdCont const& s, Datum const& d)
{
  using std::cbegin;
  using std::cend;
  return std::binary_search(cbegin(s), cend(s), d);
}

// lower_bound_all().
template <class FwdCont, class Datum>
inline auto
cet::lower_bound_all(FwdCont& s, Datum const& d)
{
  using std::begin;
  using std::end;
  return std::lower_bound(begin(s), end(s), d);
}

template <class FwdCont, class Datum>
inline auto
cet::lower_bound_all(FwdCont const& s, Datum const& d)
{
  using std::cbegin;
  using std::cend;
  return std::lower_bound(cbegin(s), cend(s), d);
}

template <class FwdCont, class Datum, class Pred>
inline auto
cet::lower_bound_all(FwdCont& s, Datum const& d, Pred p)
{
  using std::begin;
  using std::end;
  return std::lower_bound(begin(s), end(s), d, p);
}

template <class FwdCont, class Datum, class Pred>
inline auto
cet::lower_bound_all(FwdCont const& s, Datum const& d, Pred p)
{
  using std::cbegin;
  using std::cend;
  return std::lower_bound(cbegin(s), cend(s), d, p);
}

// sort_all().
template <class RandCont>
inline void
cet::sort_all(RandCont& s)
{
  using std::begin;
  using std::end;
  std::sort(begin(s), end(s));
}

template <class RandCont, class Pred>
inline void
cet::sort_all(RandCont& s, Pred p)
{
  using std::begin;
  using std::end;
  std::sort(begin(s), end(s), p);
}

// stable_sort_all().
template <class RandCont>
inline void
cet::stable_sort_all(RandCont& s)
{
  using std::begin;
  using std::end;
  std::stable_sort(begin(s), end(s));
}

template <class RandCont, class Pred>
inline void
cet::stable_sort_all(RandCont& s, Pred p)
{
  using std::begin;
  using std::end;
  std::stable_sort(begin(s), end(s), p);
}

// transform_all().
template <class Container, class OutputIt, class UnaryOp>
inline auto
cet::transform_all(Container& in, OutputIt out, UnaryOp unary_op)
{
  using std::begin;
  using std::end;
  return std::transform(begin(in), end(in), out, unary_op);
}

template <class Container, class OutputIt, class UnaryOp>
inline auto
cet::transform_all(Container const& in, OutputIt out, UnaryOp unary_op)
{
  using std::cbegin;
  using std::cend;
  return std::transform(cbegin(in), cend(in), out, unary_op);
}

template <class Container1, class Container2, class OutputIt, class BinaryOp>
inline auto
cet::transform_all(Container1& in1,
                   Container2& in2,
                   OutputIt out,
                   BinaryOp binary_op)
{
  using std::begin;
  using std::end;
  return std::transform(begin(in1), end(in1), begin(in2), out, binary_op);
}

template <class Container1, class Container2, class OutputIt, class BinaryOp>
inline auto
cet::transform_all(Container1 const& in1,
                   Container2 const& in2,
                   OutputIt out,
                   BinaryOp binary_op)
{
  using std::cbegin;
  using std::cend;
  return std::transform(cbegin(in1), cend(in1), cbegin(in2), out, binary_op);
}

#endif /* cetlib_container_algorithms_h */

// Local variables:
// mode: c++
// End:
