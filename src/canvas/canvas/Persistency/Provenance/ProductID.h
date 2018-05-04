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

    ProductID() = default;
    explicit ProductID(std::string const& canonicalProductName);
    explicit ProductID(value_type const value);

    static ProductID
    invalid()
    {
      return ProductID{};
    }

    void setID(std::string const& canonicalProductName);

    bool
    isValid() const
    {
      return value_ != 0u;
    }
    auto
    value() const
    {
      return value_;
    }

    bool
    operator<(ProductID const rh) const
    {
      return value_ < rh.value_;
    }
    bool
    operator>(ProductID const rh) const
    {
      return rh < *this;
    }
    bool
    operator==(ProductID const rh) const
    {
      return value_ == rh.value_;
    }
    bool
    operator!=(ProductID const rh) const
    {
      return !(*this == rh);
    }

    struct Hash {
      std::size_t
      operator()(ProductID const pid) const
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
    std::size_t
    operator()(art::ProductID id) const
    {
      return id.value();
    }
  };
}

#endif /* canvas_Persistency_Provenance_ProductID_h */

// Local Variables:
// mode: c++
// End:
