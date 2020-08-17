#ifndef canvas_Persistency_Provenance_ProductID_h
#define canvas_Persistency_Provenance_ProductID_h

//=====================================================================
// ProductID: A unique identifier for each EDProduct within a process.
//=====================================================================

#include "cetlib/crc32.h"

#include <functional> // for std::hash
#include <iosfwd>
#include <string>

namespace art {

  class ProductID {
  public:
    using value_type = unsigned int;

    constexpr ProductID() = default;
    explicit ProductID(std::string const& canonicalProductName);
    constexpr explicit ProductID(value_type const value) noexcept
      : value_{value}
    {}

    static constexpr ProductID
    invalid() noexcept
    {
      return ProductID{};
    }

    void setID(std::string const& canonicalProductName);

    constexpr bool
    isValid() const noexcept
    {
      return value_ != 0u;
    }

    constexpr auto
    value() const noexcept
    {
      return value_;
    }

    constexpr operator std::size_t() const noexcept
    {
      return static_cast<std::size_t>(value_);
    }

    constexpr bool
    operator<(ProductID const rh) const noexcept
    {
      return value_ < rh.value_;
    }
    constexpr bool
    operator>(ProductID const rh) const noexcept
    {
      return rh < *this;
    }
    constexpr bool
    operator==(ProductID const rh) const noexcept
    {
      return value_ == rh.value_;
    }
    constexpr bool
    operator!=(ProductID const rh) const noexcept
    {
      return !(*this == rh);
    }

    struct Hash {
      constexpr std::size_t
      operator()(ProductID const pid) const noexcept
      {
        return pid.value(); // since the ID is already a checksum, don't
                            // worry about further hashing
      }
    };

  private:
    static value_type toID(std::string const& branchName);
    friend class ProductIDStreamer;

    // Conveniently, the CRC32 value associated with an empty string
    // is 0.
    value_type value_{0u};
  };

  std::ostream& operator<<(std::ostream& os, ProductID const id);
}

namespace std {
  template <>
  struct hash<art::ProductID> {
    constexpr std::size_t
    operator()(art::ProductID const id) const noexcept
    {
      return id.value();
    }
  };
}

#endif /* canvas_Persistency_Provenance_ProductID_h */

// Local Variables:
// mode: c++
// End:
