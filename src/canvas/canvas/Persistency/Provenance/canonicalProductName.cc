#include "canvas/Persistency/Provenance/canonicalProductName.h"

namespace {
  constexpr char underscore{'_'};
  constexpr char period{'.'};
}

std::string
art::canonicalProductName(std::string const& friendlyClassName,
                          std::string const& moduleLabel,
                          std::string const& productInstanceName,
                          std::string const& processName)
{
  std::string result;
  result.reserve(friendlyClassName.size() + moduleLabel.size() +
                 productInstanceName.size() + processName.size() + 4);
  result += friendlyClassName;
  result += underscore;
  result += moduleLabel;
  result += underscore;
  result += productInstanceName;
  result += underscore;
  result += processName;
  result += period;
  // It is *absolutely* needed to have the trailing period on the branch
  // name, as this gives instruction to ROOT to split this branch in the
  // modern (v4+) way vs the old way (v3-).

  return result;
}
