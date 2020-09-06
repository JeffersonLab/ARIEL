#include "canvas/Persistency/Provenance/detail/createViewLookups.h"
// vim: set sw=2:

art::ViewLookup_t
art::detail::createViewLookups(ProductDescriptionsByID const& descriptions)
{
  // This version stores the list of products that support views.
  ViewLookup_t result;
  for (auto const& pr : descriptions) {
    auto const& pd = pr.second;
    if (!pd.supportsView())
      continue;

    auto const& procName = pd.processName();
    auto const pid = pr.first;

    result[procName].emplace_back(pid);
  }
  return result;
}
