#ifndef canvas_Persistency_Common_PtrVectorBase_h
#define canvas_Persistency_Common_PtrVectorBase_h

#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/RefCore.h"

#include <typeinfo>
#include <utility>
#include <vector>

namespace art {
  class PtrVectorBase;

  namespace detail {
    class PtrVectorBaseStreamer;
  }
}

class art::PtrVectorBase {
public:
  using key_type = unsigned long;
  using indices_t = std::vector<key_type>;
  using size_type = indices_t::size_type;

  virtual ~PtrVectorBase() = default;

  // Observers
  bool isNonnull() const noexcept;
  bool isNull() const noexcept;
  bool isAvailable() const;
  ProductID id() const noexcept;
  EDProductGetter const* productGetter() const noexcept;

  // Mutators
  void setProductGetter(EDProductGetter const*) noexcept;

protected:
  PtrVectorBase() = default;

  void clear();
  void swap(PtrVectorBase&);
  void updateCore(RefCore const& core);
  bool operator==(PtrVectorBase const&) const noexcept;

private:
  void reserve(size_type n);
  void fillPtrs() const;
  template <typename T>
  typename Ptr<T>::key_type key(Ptr<T> const& ptr) const noexcept;

  RefCore core_;
  mutable indices_t indicies_; // Will be zeroed-out by fillPtrs();

  virtual void fill_offsets(indices_t& indices) = 0;
  virtual void fill_from_offsets(indices_t const& indices) const = 0;
  virtual void zeroTransients() = 0;

  friend class art::detail::PtrVectorBaseStreamer;
}; // PtrVectorBase

inline bool
art::PtrVectorBase::isNonnull() const noexcept
{
  return core_.isNonnull();
}

inline bool
art::PtrVectorBase::isNull() const noexcept
{
  return !isNonnull();
}

inline bool
art::PtrVectorBase::isAvailable() const
{
  return core_.isAvailable();
}

inline art::ProductID
art::PtrVectorBase::id() const noexcept
{
  return core_.id();
}

inline art::EDProductGetter const*
art::PtrVectorBase::productGetter() const noexcept
{
  return core_.productGetter();
}

inline void
art::PtrVectorBase::setProductGetter(EDProductGetter const* g) noexcept
{
  core_.setProductGetter(g);
}

inline void
art::PtrVectorBase::clear()
{
  core_ = RefCore{};
  indices_t tmp;
  indicies_.swap(tmp); // Free up memory
}

inline void
art::PtrVectorBase::reserve(size_type const n)
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
art::PtrVectorBase::key(Ptr<T> const& ptr) const noexcept
{
  return ptr.key();
}

inline bool
art::PtrVectorBase::operator==(PtrVectorBase const& other) const noexcept
{
  return core_ == other.core_;
}

#endif /* canvas_Persistency_Common_PtrVectorBase_h */

// Local Variables:
// mode: c++
// End:
