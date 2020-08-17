#ifndef canvas_Persistency_Provenance_Hash_h
#define canvas_Persistency_Provenance_Hash_h
// vim: set sw=2 expandtab :

// ======================================================================
//
// Hash:
//
// Note: The call to 'fixup' in every member function is a temporary
// measure for backwards compatibility. It is necessary in every function
// because Root creates instances of the class *without* using the
// interface of the class, thus making it insufficient to assure that all
// constructors make corrected instances.
//
// ======================================================================

#include "canvas/Utilities/Exception.h"
#include "cetlib/MD5Digest.h"
#include "cetlib/container_algorithms.h"
#include "hep_concurrency/tsan.h"

#include <ostream>
#include <string>
#include <utility>

namespace art {

  namespace detail {
    // This string is in the 16 byte (non-printable) representation.
    std::string const& InvalidHash();
  } // namespace detail

  template <int I>
  class Hash {
  public:
    using value_type = std::string;

    Hash();
    explicit Hash(std::string const&);
    Hash(Hash<I> const&);
    Hash(Hash<I>&&);
    Hash<I>& operator=(Hash<I> const&);
    Hash<I>& operator=(Hash<I>&&);

    // For ROOT
    static short Class_Version() noexcept;

    // For now, just check the most basic: a default constructed
    // ParameterSetID is not valid. This is very crude: we are
    // assuming that nobody created a ParameterSetID from an empty
    // string, nor from any string that is not a valid string
    // representation of an MD5 checksum.
    bool isValid() const;
    bool isCompactForm() const noexcept;
    // Return the 16 byte (non-printable) string form.
    std::string compactForm() const;
    bool operator<(Hash<I> const&) const;
    bool operator>(Hash<I> const&) const;
    bool operator==(Hash<I> const&) const;
    bool operator!=(Hash<I> const&) const;
    std::ostream& print(std::ostream&) const;
    void swap(Hash<I>&);

  private:
    // If hash_ is in the hexified 32 byte representation,
    // make it be in the 16 byte unhexified representation.
    void fixup();
    std::string hash_{};
  };

  // MUST UPDATE WHEN CLASS IS CHANGED!
  template <int I>
  short
  Hash<I>::Class_Version() noexcept
  {
    return 10;
  }

  template <int I>
  Hash<I>::Hash()
  {
    fixup();
  }

  template <int I>
  Hash<I>::Hash(std::string const& s) : hash_{s}
  {
    fixup();
  }

  template <int I>
  Hash<I>::Hash(Hash<I> const& rhs) : hash_{rhs.hash_}
  {
    fixup();
  }

  template <int I>
  Hash<I>::Hash(Hash<I>&& rhs) : hash_{std::move(rhs.hash_)}
  {
    fixup();
  }

  template <int I>
  Hash<I>&
  Hash<I>::operator=(Hash<I> const& rhs)
  {
    if (this != &rhs) {
      hash_ = rhs.hash_;
      fixup();
    }
    return *this;
  }

  template <int I>
  Hash<I>&
  Hash<I>::operator=(Hash<I>&& rhs)
  {
    hash_ = std::move(rhs.hash_);
    fixup();
    return *this;
  }

  template <int I>
  bool
  Hash<I>::isValid() const
  {
    if (isCompactForm()) {
      auto ret = hash_ != art::detail::InvalidHash();
      return ret;
    }
    return !hash_.empty();
  }

  template <int I>
  bool
  Hash<I>::operator<(Hash<I> const& rhs) const
  {
    if (isCompactForm() == rhs.isCompactForm()) {
      return hash_ < rhs.hash_;
    }
    // Force both into compact form.
    return Hash<I>{*this} < Hash<I>{rhs};
  }

  template <int I>
  bool
  Hash<I>::operator>(Hash<I> const& rhs) const
  {
    if (isCompactForm() == rhs.isCompactForm()) {
      return hash_ > rhs.hash_;
    }
    // Force both into compact form.
    return Hash<I>{*this} > Hash<I>{rhs};
  }

  template <int I>
  bool
  Hash<I>::operator==(Hash<I> const& rhs) const
  {
    if (isCompactForm() == rhs.isCompactForm()) {
      return hash_ == rhs.hash_;
    }
    // Force both into compact form.
    return Hash<I>{*this} == Hash<I>{rhs};
  }

  template <int I>
  bool
  Hash<I>::operator!=(Hash<I> const& rhs) const
  {
    if (isCompactForm() == rhs.isCompactForm()) {
      return hash_ != rhs.hash_;
    }
    // Force both into compact form.
    return Hash<I>{*this} != Hash<I>{rhs};
  }

  template <int I>
  std::ostream&
  Hash<I>::print(std::ostream& os) const
  {
    Hash<I> tMe{*this};
    cet::MD5Result temp;
    cet::copy_all(tMe.hash_, temp.bytes);
    os << temp.toString();
    return os;
  }

  template <int I>
  void
  Hash<I>::swap(Hash<I>& rhs)
  {
    fixup();
    hash_.swap(rhs.hash_);
    fixup();
  }

  template <int I>
  std::string
  Hash<I>::compactForm() const
  {
    if (isCompactForm()) {
      return hash_;
    }
    Hash<I> tMe(*this);
    return tMe.compactForm();
  }

  template <int I>
  void
  Hash<I>::fixup()
  {
    if (hash_.size() == 16) {
      // Already in compact form.
      return;
    }
    if (hash_.size() == 0) {
      hash_ = art::detail::InvalidHash();
      return;
    }
    if (hash_.size() == 32) {
      cet::MD5Result md5;
      md5.fromHexifiedString(hash_);
      hash_ = md5.compactForm();
      return;
    }
    throw art::Exception(art::errors::LogicError)
      << "art::Hash<> instance with data in illegal state:\n"
      << hash_ << "\nPlease report this to the core framework developers";
  }

  template <int I>
  bool
  Hash<I>::isCompactForm() const noexcept
  {
    return hash_.size() == 16;
  }

  template <int I>
  void
  swap(Hash<I>& a, Hash<I>& b)
  {
    a.swap(b);
  }

  template <int I>
  std::ostream&
  operator<<(std::ostream& os, Hash<I> const& h)
  {
    return h.print(os);
  }

} // namespace art

#endif /* canvas_Persistency_Provenance_Hash_h */

// Local Variables:
// mode: c++
// End:
