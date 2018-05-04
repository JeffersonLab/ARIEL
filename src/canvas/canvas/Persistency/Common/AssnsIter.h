#ifndef canvas_Persistency_Common_AssnsIter_h
#define canvas_Persistency_Common_AssnsIter_h

/* Assns Iterator for art::Assns<L, R, D> */
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/Ptr.h"

#include <iostream>
#include <iterator>

namespace art {
  // for dereference return type
  template <class L, class R, class D>
  struct AssnsNode {
    art::Ptr<L> first{};
    art::Ptr<R> second{};
    D const* data{nullptr};
    AssnsNode() = default;
    AssnsNode(art::Ptr<L> const& l, art::Ptr<R> const& r, D const& d)
      : first{l}, second{r}, data{&d}
    {}
  };

  enum class Direction : int { Forward = 1, Reverse = -1 };

  template <Direction Dir>
  constexpr int
  signed_one()
  {
    return static_cast<std::underlying_type_t<Direction>>(Dir);
  }

  ////////////////////////////
  // Const Iterator
  template <class L, class R, class D, Direction Dir = Direction::Forward>
  class const_AssnsIter {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = art::AssnsNode<L, R, D>;
    using pointer = value_type const*;
    using reference = value_type const&;
    using difference_type = std::ptrdiff_t;

    const_AssnsIter() = default;
    explicit const_AssnsIter(art::Assns<L, R, D> const& assns)
      : coll_{&assns}, index_{assns.size()}
    {}
    explicit const_AssnsIter(art::Assns<L, R, D> const& assns,
                             std::size_t const i)
      : coll_{&assns}, index_{i}
    {}
    const_AssnsIter<L, R, D, Dir>& operator=(
      art::const_AssnsIter<L, R, D, Dir> const& iter);

    reference operator*() const;
    pointer operator->() const;
    const_AssnsIter<L, R, D, Dir>& operator++();
    const_AssnsIter<L, R, D, Dir> operator++(int);
    const_AssnsIter<L, R, D, Dir>& operator--();
    const_AssnsIter<L, R, D, Dir> operator--(int);
    bool operator==(art::const_AssnsIter<L, R, D, Dir> const& iter) const;
    bool operator!=(art::const_AssnsIter<L, R, D, Dir> const& iter) const;
    const_AssnsIter<L, R, D, Dir>& operator+=(std::size_t i);
    const_AssnsIter<L, R, D, Dir> operator+(std::size_t i) const;
    const_AssnsIter<L, R, D, Dir>& operator-=(std::size_t i);
    const_AssnsIter<L, R, D, Dir> operator-(std::size_t i) const;
    std::size_t operator-(
      art::const_AssnsIter<L, R, D, Dir> const& iter1) const;
    value_type operator[](std::size_t i) const;

    bool operator<(art::const_AssnsIter<L, R, D, Dir> const& iter) const;
    bool operator<=(art::const_AssnsIter<L, R, D, Dir> const& iter) const;
    bool operator>(art::const_AssnsIter<L, R, D, Dir> const& iter) const;
    bool operator>=(art::const_AssnsIter<L, R, D, Dir> const& iter) const;

    std::size_t
    getIndex() const
    {
      return index_;
    };

  private:
    art::Assns<L, R, D> const* coll_{nullptr};
    std::size_t index_{-1ull};
    mutable AssnsNode<L, R, D> node_{};
  };

  // For reverse iterators, we do not shift the underlying index into
  // the collection since this wreaks havoc with the comparison
  // operators.  The shifting happens during dereferencing.  Note that
  // an attempt to dereference rend() will result in an out-of-range
  // error.
  template <Direction Dir>
  constexpr auto
  index_for_dereferencing(std::size_t const i)
  {
    return (Dir == Direction::Forward) ? i : i - 1;
  }

  // Utilities for determining the left and right operands of iterator
  // comparisons based on the direction of the iterator.
  template <class L, class R, class D, Direction Dir>
  constexpr auto const&
  left(const_AssnsIter<L, R, D, Dir> const& a,
       const_AssnsIter<L, R, D, Dir> const& b)
  {
    return (Dir == Direction::Forward) ? a : b;
  }

  template <class L, class R, class D, Direction Dir>
  constexpr auto const&
  right(const_AssnsIter<L, R, D, Dir> const& a,
        const_AssnsIter<L, R, D, Dir> const& b)
  {
    return (Dir == Direction::Forward) ? b : a;
  }

  // Dereference
  template <class L, class R, class D, Direction Dir>
  typename art::const_AssnsIter<L, R, D, Dir>::reference
    const_AssnsIter<L, R, D, Dir>::operator*() const
  {
    auto const index = index_for_dereferencing<Dir>(index_);
    node_.first = (*coll_)[index].first;
    node_.second = (*coll_)[index].second;
    node_.data = &(coll_->data(index));
    return node_;
  }

  // right arrow
  template <class L, class R, class D, Direction Dir>
  typename art::const_AssnsIter<L, R, D, Dir>::pointer
    const_AssnsIter<L, R, D, Dir>::operator->() const
  {
    auto const index = index_for_dereferencing<Dir>(index_);
    node_.first = (*coll_)[index].first;
    node_.second = (*coll_)[index].second;
    node_.data = &(coll_->data(index));
    return &node_;
  }

  // Pre-increment
  template <class L, class R, class D, Direction Dir>
  art::const_AssnsIter<L, R, D, Dir>& const_AssnsIter<L, R, D, Dir>::
  operator++()
  {
    index_ = index_ + signed_one<Dir>();
    return *this;
  }

  // Post-increment
  template <class L, class R, class D, Direction Dir>
  art::const_AssnsIter<L, R, D, Dir> const_AssnsIter<L, R, D, Dir>::operator++(
    int)
  {
    art::const_AssnsIter<L, R, D, Dir> tmp{*this};
    index_ = index_ + signed_one<Dir>();
    return tmp;
  }

  // Pre-decrement
  template <class L, class R, class D, Direction Dir>
  art::const_AssnsIter<L, R, D, Dir>& const_AssnsIter<L, R, D, Dir>::
  operator--()
  {
    index_ = index_ - signed_one<Dir>();
    return *this;
  }
  // post-decrement
  template <class L, class R, class D, Direction Dir>
  art::const_AssnsIter<L, R, D, Dir> const_AssnsIter<L, R, D, Dir>::operator--(
    int)
  {
    art::const_AssnsIter<L, R, D, Dir> tmp{*this};
    index_ = index_ - signed_one<Dir>();
    return tmp;
  }

  // equality
  template <class L, class R, class D, Direction Dir>
  bool
  const_AssnsIter<L, R, D, Dir>::operator==(
    art::const_AssnsIter<L, R, D, Dir> const& iter) const
  {
    return index_ == iter.index_;
  }

  // in-equality
  template <class L, class R, class D, Direction Dir>
  bool
  const_AssnsIter<L, R, D, Dir>::operator!=(
    art::const_AssnsIter<L, R, D, Dir> const& iter) const
  {
    return !(index_ == iter.index_);
  }

  // increment by a given value ...
  template <class L, class R, class D, Direction Dir>
  art::const_AssnsIter<L, R, D, Dir>&
  const_AssnsIter<L, R, D, Dir>::operator+=(std::size_t const i)
  {
    // to do add check for index bounds and make sure it works for both positive
    // and negative values
    index_ = index_ + signed_one<Dir>() * i;
    return *this;
  }

  // random access
  template <class L, class R, class D, Direction Dir>
  art::const_AssnsIter<L, R, D, Dir>
  const_AssnsIter<L, R, D, Dir>::operator+(std::size_t const i) const
  {
    art::const_AssnsIter<L, R, D, Dir> tmp{*this};
    tmp.index_ = tmp.index_ + signed_one<Dir>() * i;
    return tmp;
  }

  // decrement by a given value ...
  template <class L, class R, class D, Direction Dir>
  art::const_AssnsIter<L, R, D, Dir>&
  const_AssnsIter<L, R, D, Dir>::operator-=(std::size_t const i)
  {
    // to do add check for index bounds and make sure it works for both positive
    // and negative values
    index_ = index_ - signed_one<Dir>() * i;
    return *this;
  }

  // random access
  template <class L, class R, class D, Direction Dir>
  art::const_AssnsIter<L, R, D, Dir>
  const_AssnsIter<L, R, D, Dir>::operator-(std::size_t const i) const
  {
    art::const_AssnsIter<L, R, D, Dir> tmp{*this};
    tmp.index_ = tmp.index_ - signed_one<Dir>() * i;
    return tmp;
  }

  // difference between two iterators to return an index
  template <class L, class R, class D, Direction Dir>
  std::size_t
  const_AssnsIter<L, R, D, Dir>::operator-(
    art::const_AssnsIter<L, R, D, Dir> const& iter1) const
  {
    return (iter1.index_ - index_);
  }

  // Dereference
  template <class L, class R, class D, Direction Dir>
  typename art::const_AssnsIter<L, R, D, Dir>::value_type
    const_AssnsIter<L, R, D, Dir>::operator[](std::size_t const i) const
  {
    art::const_AssnsIter<L, R, D, Dir> tmp{*this};
    tmp.index_ = tmp.index_ + signed_one<Dir>() * i;
    return tmp.node_;
  }

  // less than
  template <class L, class R, class D, Direction Dir>
  bool
  const_AssnsIter<L, R, D, Dir>::operator<(
    art::const_AssnsIter<L, R, D, Dir> const& iter) const
  {
    auto const& l = left(*this, iter);
    auto const& r = right(*this, iter);
    return l.index_ < r.index_;
  }

  // less than equal to
  template <class L, class R, class D, Direction Dir>
  bool
  const_AssnsIter<L, R, D, Dir>::operator<=(
    art::const_AssnsIter<L, R, D, Dir> const& iter) const
  {
    auto const& l = left(*this, iter);
    auto const& r = right(*this, iter);
    return l.index_ <= r.index_;
  }

  // less than equal to
  template <class L, class R, class D, Direction Dir>
  bool
  const_AssnsIter<L, R, D, Dir>::operator>(
    art::const_AssnsIter<L, R, D, Dir> const& iter) const
  {
    auto const& l = left(*this, iter);
    auto const& r = right(*this, iter);
    return l.index_ > r.index_;
  }

  // greater than equal to
  template <class L, class R, class D, Direction Dir>
  bool
  const_AssnsIter<L, R, D, Dir>::operator>=(
    art::const_AssnsIter<L, R, D, Dir> const& iter) const
  {
    auto const& l = left(*this, iter);
    auto const& r = right(*this, iter);
    return l.index_ >= r.index_;
  }

  template <class L, class R, class D, Direction Dir>
  art::const_AssnsIter<L, R, D, Dir>&
  const_AssnsIter<L, R, D, Dir>::operator=(
    art::const_AssnsIter<L, R, D, Dir> const& iter)
  {
    node_ = iter.node_;
    return *this;
  }
}

#endif /* canvas_Persistency_Common_AssnsIter_h */

// Local Variables:
// mode: c++
// End:
