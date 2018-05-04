#include "canvas/Persistency/Provenance/ProductID.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <ostream>

namespace art {

  ProductID::ProductID(value_type const value) : value_{value} {}

  ProductID::ProductID(std::string const& canonicalProductName)
    : ProductID{toID(canonicalProductName)}
  {}

  void
  ProductID::setID(std::string const& canonicalProductName)
  {
    value_ = toID(canonicalProductName);
  }

  ProductID::value_type
  ProductID::toID(std::string const& canonicalProductName)
  {
    auto const& check = cet::crc32{canonicalProductName}.digest();
    mf::LogDebug("ProductID") << "Product created with id: "
                              << "[" << check << "] "
                              << "from canonical product name: "
                              << "\"" << canonicalProductName << "\"";
    return check;
  }

  std::ostream&
  operator<<(std::ostream& os, ProductID const id)
  {
    os << id.value();
    return os;
  }
}
