#include "fhiclcpp/detail/KeyAssembler.h"
#include "cetlib/container_algorithms.h"

#include <sstream>

using namespace fhicl;
using namespace fhicl::detail;

std::string
KeyAssembler::full_key_(name_t const& name) const
{
  std::ostringstream os;
  cet::copy_all(name_stack_, std::ostream_iterator<std::string>{os, "."});
  os << name;
  return os.str();
}

void
KeyAssembler::enter_table(key_t const& key, any_t const&)
{
  keys_.emplace_back(full_key_(key));
  name_stack_.emplace_back(key);
}

void
KeyAssembler::exit_table(key_t const&, any_t const&)
{
  name_stack_.pop_back();
}

void
KeyAssembler::enter_sequence(key_t const& key, any_t const&)
{
  keys_.emplace_back(full_key_(key));
}

void
KeyAssembler::atom(key_t const& key, any_t const&)
{
  keys_.emplace_back(full_key_(key));
}
