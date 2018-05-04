#include "fhiclcpp/types/detail/SearchAllowedConfiguration.h"

using namespace fhicl::detail;
using fhicl::detail::SearchAllowedConfiguration;

bool
SearchAllowedConfiguration::supports_key(ParameterBase const& pb,
                                         std::string const& searched_for_key)
{
  SearchAllowedConfiguration sac{pb, searched_for_key};
  sac.walk_over(pb);
  return sac.result();
}

SearchAllowedConfiguration::SearchAllowedConfiguration(
  ParameterBase const& pb,
  std::string const& searched_for_key)
  : key_{form_absolute(pb, searched_for_key)}
{}

bool
SearchAllowedConfiguration::before_action(ParameterBase const& pb)
{
  supportsKey_ = supportsKey_ || pb.key() == key_;
  // If supportsKey_ is true, then a match has been found, and no
  // extra tree-walking is required--so this function should
  // return false.  If supportsKey_ is false, then continue
  // looking through the tree to see if another parameter
  // matches--i.e. it should return true.
  return !supportsKey_;
}

std::string
SearchAllowedConfiguration::form_absolute(ParameterBase const& pb,
                                          std::string const& searched_for_key)
{
  if (searched_for_key.empty()) {
    throw exception{error::other, "SearchAllowedConfiguration::form_absolute"}
      << "The specified searched-for key is empty.\n";
  }

  auto const pt = pb.parameter_type();
  if (!is_table(pt) && !is_sequence(pt)) {
    throw exception{error::other, "SearchAllowedConfiguration::form_absolute"}
      << "Attempt to search for '" << searched_for_key
      << "' as supported by a\n"
      << "parameter that is not a table or sequence.\n";
  }

  // assume searched-for key is meant to be relative to the
  // top_level_key.
  std::string const appending_character{is_table(pt) ? "." : ""};
  std::string absolute_key{pb.key()};
  absolute_key += appending_character;
  absolute_key += searched_for_key;
  return absolute_key;
}
