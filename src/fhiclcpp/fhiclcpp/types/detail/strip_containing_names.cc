#include "fhiclcpp/types/detail/strip_containing_names.h"

// If 'find_{first,last}_of' returns std::string::npos, then 'pos' will
// be 0 and the call to substr will return the full key.

std::string
fhicl::detail::strip_first_containing_name(std::string const& key)
{
  auto const pos = key.find_first_of(".") + 1;
  return key.substr(pos);
}

std::string
fhicl::detail::strip_all_containing_names(std::string const& key)
{
  auto const pos = key.find_last_of(".") + 1;
  return key.substr(pos);
}
