#include "canvas/Persistency/Provenance/Hash.h"
// vim: set sw=2 expandtab :

namespace art::detail {
  // This string is the 16-byte, non-printable version.
  std::string const&
  InvalidHash()
  {
    static std::string const invalid{cet::MD5Result().compactForm()};
    return invalid;
  }
}
