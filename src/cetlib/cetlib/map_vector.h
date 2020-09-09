#ifndef cetlib_map_vector_h
#define cetlib_map_vector_h

// ======================================================================
// map_vector: A map mimicking a sparse vector interface
//
// Integer subscripting is unsupported and yields a compilation failure.
// ======================================================================

#include "cetlib_except/exception.h"

#include <algorithm>
#include <iosfwd>
#include <vector>

namespace cet {
  class map_vector_key;

  bool operator==(map_vector_key const&, map_vector_key const&) noexcept;
  bool operator!=(map_vector_key const&, map_vector_key const&) noexcept;
  bool operator<(map_vector_key const&, map_vector_key const&) noexcept;
  bool operator>(map_vector_key const&, map_vector_key const&) noexcept;
  bool operator<=(map_vector_key const&, map_vector_key const&) noexcept;
  bool operator>=(map_vector_key const&, map_vector_key const&) noexcept;

  std::ostream& operator<<(std::ostream&, map_vector_key const&);

  template <class Value>
  class map_vector;
}

// ======================================================================

class cet::map_vector_key {
public:
  // c'tors:
  constexpr map_vector_key() noexcept : key_{-1ul} {}
  explicit map_vector_key(int key);
  constexpr explicit map_vector_key(unsigned const key) noexcept : key_{key} {}
  constexpr explicit map_vector_key(unsigned long key) noexcept;

  // observers:
  unsigned long
  asInt() const noexcept
  {
    return key_;
  }
  unsigned long
  asUint() const noexcept
  {
    return key_;
  }
  void ensure_valid() const;

private:
  unsigned long key_;

}; // map_vector_key

inline cet::map_vector_key::map_vector_key(int key) : key_(key)
{
  if (key < 0) {
    throw cet::exception("InvalidKey")
      << "Negative key " << key << " not valid for map_vector_key.";
  }
}

constexpr cet::map_vector_key::map_vector_key(unsigned long const key) noexcept
  : key_{key}
{}

inline void
cet::map_vector_key::ensure_valid() const
{
  if (key_ == static_cast<unsigned long>(-1)) { // Invalid key
    throw cet::exception("InvalidKey")
      << "Attempt to use an invalid cet::map_vector_key.";
  }
}

// ======================================================================

template <class Value>
class cet::map_vector {
public:
  // types:
  using key_type = map_vector_key;
  using mapped_type = Value;
  using value_type = std::pair<key_type, mapped_type>;
  using impl_type = std::vector<value_type>;

  using size_type = typename impl_type::size_type;
  using difference_type = typename impl_type::difference_type;

  using iterator = typename impl_type::iterator;
  using const_iterator = typename impl_type::const_iterator;
  using reverse_iterator = typename impl_type::reverse_iterator;
  using const_reverse_iterator = typename impl_type::const_reverse_iterator;

  using allocator_type = typename impl_type::allocator_type;
  using pointer = typename allocator_type::pointer;
  using const_pointer = typename allocator_type::const_pointer;
  using reference = typename allocator_type::reference;
  using const_reference = typename allocator_type::const_reference;

  // c'tors:
  map_vector() = default;

  template <class InIter>
  map_vector(InIter const b, InIter const e)
  {
    insert(b, e);
  }

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  // properties:
  bool
  empty() const noexcept
  {
    return v_.empty();
  }
  size_type
  size() const noexcept
  {
    return v_.size();
  }
  size_type
  max_size() const noexcept
  {
    return v_.max_size();
  }
  size_type
  capacity() const noexcept
  {
    return v_.capacity();
  }

  allocator_type
  get_allocator() const noexcept
  {
    return v_.get_allocator();
  }

  // observers:
  value_type const& front() const;
  value_type const& back() const;

  // To be used in case map_vectors must be concatenated, and not
  // merged.
  size_t
  delta() const
  {
    return v_.empty() ? 0 : 1 + v_.back().first.asInt();
  }

  bool has(key_type key) const;

  iterator find(key_type key);
  const_iterator find(key_type key) const;
  const_iterator findOrThrow(key_type key) const;

  mapped_type* getOrNull(key_type key);
  mapped_type const* getOrNull(key_type key) const;

  mapped_type& getOrThrow(key_type key);
  mapped_type const& getOrThrow(key_type key) const;

  mapped_type& operator[](key_type key);
  mapped_type const& operator[](key_type key) const { return getOrThrow(key); }
  mapped_type const&
  at(key_type key) const
  {
    return getOrThrow(key);
  }

  // iterators:
  iterator
  begin() noexcept
  {
    return v_.begin();
  }
  const_iterator
  begin() const noexcept
  {
    return v_.begin();
  }

  iterator
  end() noexcept
  {
    return v_.end();
  }
  const_iterator
  end() const noexcept
  {
    return v_.end();
  }

  reverse_iterator
  rbegin() noexcept
  {
    return v_.rbegin();
  }
  const_reverse_iterator
  rbegin() const noexcept
  {
    return v_.rbegin();
  }

  reverse_iterator
  rend() noexcept
  {
    return v_.rend();
  }
  const_reverse_iterator
  rend() const noexcept
  {
    return v_.rend();
  }

  const_iterator
  cbegin() const noexcept
  {
    return v_.cbegin();
  }
  const_iterator
  cend() const noexcept
  {
    return v_.cend();
  }
  const_reverse_iterator
  crbegin() const
  {
    return v_.crbegin();
  }
  const_reverse_iterator
  crend() const noexcept
  {
    return v_.crend();
  }

  // mutators:
  void
  clear() noexcept
  {
    v_.clear();
  }

  void
  reserve(size_type const n)
  {
    v_.reserve(n);
  }

  void
  swap(map_vector<mapped_type>& other)
  {
    v_.swap(other.v_);
  }

  std::pair<iterator, bool> insert(value_type const& x);

  template <class InIter>
  void insert(InIter b, InIter e);

  // MUST UPDATE WHEN CLASS IS CHANGED!
  static short
  Class_Version()
  {
    return 10;
  }

private:
  impl_type v_{};

  bool class_invariant() const;

  static bool lt(value_type const&, value_type const&) noexcept;
  static bool eq(value_type const&, value_type const&) noexcept;

}; // map_vector<>

// ======================================================================
// additional map_vector_key implementation

// ----------------------------------------------------------------------
// comparisons:

inline bool
cet::operator==(map_vector_key const& k1, map_vector_key const& k2) noexcept
{
  return k1.asInt() == k2.asInt();
}

inline bool
cet::operator!=(map_vector_key const& k1, map_vector_key const& k2) noexcept
{
  return k1.asInt() != k2.asInt();
}

inline bool
cet::operator<(map_vector_key const& k1, map_vector_key const& k2) noexcept
{
  return k1.asInt() < k2.asInt();
}

inline bool
cet::operator>(map_vector_key const& k1, map_vector_key const& k2) noexcept
{
  return k1.asInt() > k2.asInt();
}

inline bool
cet::operator<=(map_vector_key const& k1, map_vector_key const& k2) noexcept
{
  return k1.asInt() <= k2.asInt();
}

inline bool
cet::operator>=(map_vector_key const& k1, map_vector_key const& k2) noexcept
{
  return k1.asInt() >= k2.asInt();
}

// ----------------------------------------------------------------------
// output:

inline std::ostream&
cet::operator<<(std::ostream& os, cet::map_vector_key const& key)
{
  return os << key.asInt();
}

// ======================================================================
// additional map_vector<> implementation

// ----------------------------------------------------------------------
// observers:

template <class Value>
typename cet::map_vector<Value>::value_type const&
cet::map_vector<Value>::front() const
{
  if (v_.empty())
    throw cet::exception("map_vector::front") << "container is empty!\n";
  return v_.front();
}

template <class Value>
typename cet::map_vector<Value>::value_type const&
cet::map_vector<Value>::back() const
{
  if (v_.empty())
    throw cet::exception("map_vector::back") << "container is empty!\n";
  return v_.back();
}

template <class Value>
bool
cet::map_vector<Value>::has(key_type const key) const
{
  value_type const v{key, mapped_type{}};
  return std::binary_search(v_.begin(), v_.end(), v, lt);
}

template <class Value>
typename cet::map_vector<Value>::iterator
cet::map_vector<Value>::find(key_type const key)
{
  value_type const v{key, mapped_type{}};
  auto const begin = v_.begin(), end = v_.end();
  auto it = std::lower_bound(begin, end, v, lt);
  if (it != end && it->first != key)
    it = end;
  return it;
}

template <class Value>
typename cet::map_vector<Value>::const_iterator
cet::map_vector<Value>::find(key_type const key) const
{
  value_type const v{key, mapped_type{}};
  auto const begin = v_.cbegin(), end = v_.cend();
  auto it = std::lower_bound(begin, end, v, lt);
  if (it != end && it->first != key)
    it = end;
  return it;
}

template <class Value>
typename cet::map_vector<Value>::const_iterator
cet::map_vector<Value>::findOrThrow(key_type const key) const
{
  auto p = find(key);
  if (p == v_.cend())
    throw cet::exception("map_vector::getOrThrow")
      << "out of range (no such key): " << key.asInt() << std::endl;

  return p;
}

template <class Value>
Value*
cet::map_vector<Value>::getOrNull(key_type const key)
{
  auto it = find(key);
  return it == v_.end() ? nullptr : &it->second;
}

template <class Value>
Value const*
cet::map_vector<Value>::getOrNull(key_type const key) const
{
  auto it = find(key);
  return it == v_.cend() ? nullptr : &it->second;
}

template <class Value>
Value&
cet::map_vector<Value>::getOrThrow(key_type const key)
{
  auto* p = getOrNull(key);
  if (p == nullptr)
    throw cet::exception("map_vector::getOrThrow")
      << "out of range (no such key): " << key.asInt() << std::endl;

  return *p;
}

template <class Value>
Value const&
cet::map_vector<Value>::getOrThrow(key_type const key) const
{
  auto const* p = getOrNull(key);
  if (p == nullptr)
    throw cet::exception("map_vector::getOrThrow")
      << "out of range (no such key): " << key.asInt() << std::endl;

  return *p;
}

template <class Value>
Value& cet::map_vector<Value>::operator[](key_type const key)
{
  value_type const v{key, mapped_type{}};
  auto const begin = v_.begin(), end = v_.end();
  auto it = std::lower_bound(begin, end, v, lt);
  if (it == end || it->first != key)
    it = v_.insert(it, std::move(v));
  return it->second;
}

// ----------------------------------------------------------------------
// mutators:

template <class Value>
std::pair<typename cet::map_vector<Value>::iterator, bool>
cet::map_vector<Value>::insert(value_type const& v)
{
  v.first.ensure_valid();
  auto const begin = v_.begin(), end = v_.end();
  auto it = std::lower_bound(begin, end, v, lt);
  if (it == end || it->first != v.first)
    return std::make_pair(v_.insert(it, v), true);
  return std::make_pair(it, false);
}

template <class Value>
template <class InIter>
void
cet::map_vector<Value>::insert(InIter const b, InIter const e)
{
  std::for_each(b, e, [](auto const& pr) { return pr.first.ensure_valid(); });
  impl_type result;
  std::merge(v_.cbegin(), v_.cend(), b, e, back_inserter(result), lt);
  auto new_end = std::unique(result.begin(), result.end(), eq);
  result.erase(new_end, result.end());
  v_.swap(result);
}

// ----------------------------------------------------------------------
// helpers:

template <class Value>
bool
cet::map_vector<Value>::class_invariant() const
{
  return std::is_sorted(v_.begin(), v_.end(), lt);
}

// ======================================================================

template <class Value>
bool
cet::map_vector<Value>::lt(value_type const& v1, value_type const& v2) noexcept
{
  return v1.first < v2.first;
}

template <class Value>
bool
cet::map_vector<Value>::eq(value_type const& v1, value_type const& v2) noexcept
{
  return v1.first == v2.first;
}

// ======================================================================
#endif /* cetlib_map_vector_h */

// Local variables:
// mode: c++
// End:
