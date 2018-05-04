#ifndef canvas_Persistency_Common_PtrVectorBase_h
#define canvas_Persistency_Common_PtrVectorBase_h

#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/RefCore.h"

#include <typeinfo>
#include <utility>
#include <vector>

class TBuffer;

namespace art {
  class PtrVectorBase;

  namespace detail {
    class PtrVectorBaseStreamer;
  }
}

class art::PtrVectorBase {
public:
  typedef unsigned long key_type;
  typedef std::vector<key_type> indices_t;

public:
  typedef indices_t::size_type size_type;

  virtual ~PtrVectorBase() = default;

  // Observers
  bool isNonnull() const;
  bool isNull() const;
  bool isAvailable() const;
  ProductID id() const;
  EDProductGetter const* productGetter() const;

  // Mutators
  void setProductGetter(EDProductGetter* g) const;

protected:
  PtrVectorBase() = default;

  void clear();
  void swap(PtrVectorBase&);
  void updateCore(RefCore const& core);
  bool operator==(PtrVectorBase const&) const;

private:
  void reserve(size_type n);
  void fillPtrs() const;
  template <typename T>
  typename Ptr<T>::key_type key(Ptr<T> const& ptr) const;

  RefCore core_;
  mutable indices_t indicies_; // Will be zeroed-out by fillPtrs();

  virtual void fill_offsets(indices_t& indices) = 0;
  virtual void fill_from_offsets(indices_t const& indices) const = 0;
  virtual void zeroTransients() = 0;

  friend class art::detail::PtrVectorBaseStreamer;
}; // PtrVectorBase

inline bool
art::PtrVectorBase::isNonnull() const
{
  return core_.isNonnull();
}

inline bool
art::PtrVectorBase::isNull() const
{
  return !isNonnull();
}

inline bool
art::PtrVectorBase::isAvailable() const
{
  return core_.isAvailable();
}

inline art::ProductID
art::PtrVectorBase::id() const
{
  return core_.id();
}

inline art::EDProductGetter const*
art::PtrVectorBase::productGetter() const
{
  return core_.productGetter();
}

inline void
art::PtrVectorBase::setProductGetter(EDProductGetter* g) const
{
  core_.setProductGetter(g);
}

inline void
art::PtrVectorBase::clear()
{
  core_ = RefCore();
  indices_t tmp;
  indicies_.swap(tmp); // Free up memory
}

inline void
art::PtrVectorBase::reserve(size_type n)
{
  indicies_.reserve(n);
}

inline void
art::PtrVectorBase::swap(PtrVectorBase& other)
{
  core_.swap(other.core_);
}

template <typename T>
inline typename art::Ptr<T>::key_type
art::PtrVectorBase::key(Ptr<T> const& ptr) const
{
  return ptr.key();
}

inline bool
art::PtrVectorBase::operator==(PtrVectorBase const& other) const
{
  return core_ == other.core_;
}

#endif /* canvas_Persistency_Common_PtrVectorBase_h */

// Local Variables:
// mode: c++
// End:
