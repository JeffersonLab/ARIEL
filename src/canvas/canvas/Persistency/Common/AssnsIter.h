#ifndef canvas_Persistency_Common_AssnsIter_h
#define canvas_Persistency_Common_AssnsIter_h

/* Assns Iterator for art::Assns<L, R, D> */

#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/AssnsNode.h"

#include <iostream>
#include <iterator>

namespace art {
  enum class Direction : int { Forward = 1, Reverse = -1 };

  template <Direction Dir>
  constexpr int
  signed_one()
  {
    return static_cast<std::underlying_type_t<Direction>>(Dir);
  }

  ////////////////////////////
  // Const Iterator
  template <typename L,
            typename R,
            typename D,
            Direction Dir = Direction::Forward>
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
  template <typename L, typename R, typename D, Direction Dir>
  constexpr auto const&
  left(const_AssnsIter<L, R, D, Dir> const& a,
       const_AssnsIter<L, R, D, Dir> const& b)
  {
    return (Dir == Direction::Forward) ? a : b;
  }

  template <typename L, typename R, typename D, Direction Dir>
  constexpr auto const&
  right(const_AssnsIter<L, R, D, Dir> const& a,
        const_AssnsIter<L, R, D, Dir> const& b)
  {
    return (Dir == Direction::Forward) ? b : a;
  }

  // Dereference
  template <typename L, typename R, typename D, Direction Dir>
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
  template <typename L, typename R, typename D, Direction Dir>
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
  template <typename L, typename R, typename D, Direction Dir>
  art::const_AssnsIter<L, R, D, Dir>&
  const_AssnsIter<L, R, D, Dir>::operator++()
  {
    index_ = index_ + signed_one<Dir>();
    return *this;
  }

  // Post-increment
  template <typename L, typename R, typename D, Direction Dir>
  art::const_AssnsIter<L, R, D, Dir>
  const_AssnsIter<L, R, D, Dir>::operator++(int)
  {
    art::const_AssnsIter<L, R, D, Dir> tmp{*this};
    index_ = index_ + signed_one<Dir>();
    return tmp;
  }

  // Pre-decrement
  template <typename L, typename R, typename D, Direction Dir>
  art::const_AssnsIter<L, R, D, Dir>&
  const_AssnsIter<L, R, D, Dir>::operator--()
  {
    index_ = index_ - signed_one<Dir>();
    return *this;
  }
  // post-decrement
  template <typename L, typename R, typename D, Direction Dir>
  art::const_AssnsIter<L, R, D, Dir>
  const_AssnsIter<L, R, D, Dir>::operator--(int)
  {
    art::const_AssnsIter<L, R, D, Dir> tmp{*this};
    index_ = index_ - signed_one<Dir>();
    return tmp;
  }

  // equality
  template <typename L, typename R, typename D, Direction Dir>
  bool
  const_AssnsIter<L, R, D, Dir>::operator==(
    art::const_AssnsIter<L, R, D, Dir> const& iter) const
  {
    return index_ == iter.index_;
  }

  // in-equality
  template <typename L, typename R, typename D, Direction Dir>
  bool
  const_AssnsIter<L, R, D, Dir>::operator!=(
    art::const_AssnsIter<L, R, D, Dir> const& iter) const
  {
    return !(index_ == iter.index_);
  }

  // increment by a given value ...
  template <typename L, typename R, typename D, Direction Dir>
  art::const_AssnsIter<L, R, D, Dir>&
  const_AssnsIter<L, R, D, Dir>::operator+=(std::size_t const i)
  {
    // to do add check for index bounds and make sure it works for both positive
    // and negative values
    index_ = index_ + signed_one<Dir>() * i;
    return *this;
  }

  // random access
  template <typename L, typename R, typename D, Direction Dir>
  art::const_AssnsIter<L, R, D, Dir>
  const_AssnsIter<L, R, D, Dir>::operator+(std::size_t const i) const
  {
    art::const_AssnsIter<L, R, D, Dir> tmp{*this};
    tmp.index_ = tmp.index_ + signed_one<Dir>() * i;
    return tmp;
  }

  // decrement by a given value ...
  template <typename L, typename R, typename D, Direction Dir>
  art::const_AssnsIter<L, R, D, Dir>&
  const_AssnsIter<L, R, D, Dir>::operator-=(std::size_t const i)
  {
    // to do add check for index bounds and make sure it works for both positive
    // and negative values
    index_ = index_ - signed_one<Dir>() * i;
    return *this;
  }

  // random access
  template <typename L, typename R, typename D, Direction Dir>
  art::const_AssnsIter<L, R, D, Dir>
  const_AssnsIter<L, R, D, Dir>::operator-(std::size_t const i) const
  {
    art::const_AssnsIter<L, R, D, Dir> tmp{*this};
    tmp.index_ = tmp.index_ - signed_one<Dir>() * i;
    return tmp;
  }

  // difference between two iterators to return an index
  template <typename L, typename R, typename D, Direction Dir>
  std::size_t
  const_AssnsIter<L, R, D, Dir>::operator-(
    art::const_AssnsIter<L, R, D, Dir> const& iter1) const
  {
    return (iter1.index_ - index_);
  }

  // Dereference
  template <typename L, typename R, typename D, Direction Dir>
  typename art::const_AssnsIter<L, R, D, Dir>::value_type
    const_AssnsIter<L, R, D, Dir>::operator[](std::size_t const i) const
  {
    art::const_AssnsIter<L, R, D, Dir> tmp{*this};
    tmp.index_ = tmp.index_ + signed_one<Dir>() * i;
    return tmp.node_;
  }

  // less than
  template <typename L, typename R, typename D, Direction Dir>
  bool
  const_AssnsIter<L, R, D, Dir>::operator<(
    art::const_AssnsIter<L, R, D, Dir> const& iter) const
  {
    auto const& l = left(*this, iter);
    auto const& r = right(*this, iter);
    return l.index_ < r.index_;
  }

  // less than equal to
  template <typename L, typename R, typename D, Direction Dir>
  bool
  const_AssnsIter<L, R, D, Dir>::operator<=(
    art::const_AssnsIter<L, R, D, Dir> const& iter) const
  {
    auto const& l = left(*this, iter);
    auto const& r = right(*this, iter);
    return l.index_ <= r.index_;
  }

  // less than equal to
  template <typename L, typename R, typename D, Direction Dir>
  bool
  const_AssnsIter<L, R, D, Dir>::operator>(
    art::const_AssnsIter<L, R, D, Dir> const& iter) const
  {
    auto const& l = left(*this, iter);
    auto const& r = right(*this, iter);
    return l.index_ > r.index_;
  }

  // greater than equal to
  template <typename L, typename R, typename D, Direction Dir>
  bool
  const_AssnsIter<L, R, D, Dir>::operator>=(
    art::const_AssnsIter<L, R, D, Dir> const& iter) const
  {
    auto const& l = left(*this, iter);
    auto const& r = right(*this, iter);
    return l.index_ >= r.index_;
  }
}

#endif /* canvas_Persistency_Common_AssnsIter_h */

// Local Variables:
// mode: c++
// End:
