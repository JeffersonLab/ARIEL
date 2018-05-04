#include "canvas/Persistency/Provenance/FileFormatVersion.h"
#include <ostream>

std::ostream&
art::operator<<(std::ostream& os, FileFormatVersion const& ff)
{
  os << (ff.era_.empty() ? "" : (ff.era_ + ": ")) << ff.value_;
  return os;
}
