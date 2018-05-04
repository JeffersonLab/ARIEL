#include "canvas/Utilities/TypeID.h"

#include "canvas/Utilities/Exception.h"
#include "canvas/Utilities/FriendlyName.h"
#include "canvas/Utilities/uniform_type_name.h"
#include "tbb/concurrent_unordered_map.h"

#include <cstddef>
#include <ostream>
#include <string>

using namespace std;

void
art::TypeID::print(ostream& os) const
{
  os << className();
}

string
art::TypeID::className() const
{
  static tbb::concurrent_unordered_map<size_t, string> s_nameMap;
  auto hash_code = typeInfo().hash_code();
  auto entry = s_nameMap.find(hash_code);
  if (entry == s_nameMap.end()) {
    entry = s_nameMap.emplace(hash_code, uniform_type_name(typeInfo())).first;
  }
  return entry->second;
}

string
art::TypeID::friendlyClassName() const
{
  return friendlyname::friendlyName(className());
}

ostream&
art::operator<<(ostream& os, TypeID const& tid)
{
  tid.print(os);
  return os;
}

std::string
art::name_of_template_arg(std::string const& template_instance,
                          size_t desired_arg)
{
  std::string result;
  auto comma_count = 0ul;
  auto template_level = 0ul;
  auto arg_start = string::npos;
  auto pos = 0ul;
  pos = template_instance.find_first_of("<>,", pos);
  while (pos != string::npos) {
    switch (template_instance[pos]) {
      case '<':
        ++template_level;
        if ((desired_arg == 0ul) && (template_level == 1ul)) {
          // Found the begin of the desired template arg.
          arg_start = pos + 1;
        }
        break;
      case '>':
        --template_level;
        if ((desired_arg == comma_count) && (template_level == 0ul)) {
          // Found the end of the desired template arg -- trim trailing
          // whitespace
          auto const arg_end =
            template_instance.find_last_not_of(" \t", pos - 1) + 1;
          result = template_instance.substr(arg_start, arg_end - arg_start);
          return result;
        }
        break;
      case ',':
        if (template_level != 1ul) {
          // Ignore arguments not at the first level.
          break;
        }
        if (comma_count == desired_arg) {
          // Found the end of the desired template arg.
          result = template_instance.substr(arg_start, pos - arg_start);
          return result;
        }
        ++comma_count;
        if (comma_count == desired_arg) {
          // Found the begin of the desired template arg.
          arg_start = pos + 1;
        }
        break;
    }
    ++pos;
    pos = template_instance.find_first_of("<>,", pos);
  }
  return result;
}

std::string
art::name_of_assns_partner(std::string assns_type_name)
{
  std::string result;
  if (!is_assns(assns_type_name)) {
    return result;
  }
  static std::string const assns_start = "art::Assns<"s;
  auto const arg0 = name_of_template_arg(assns_type_name, 0);
  auto const arg1 = name_of_template_arg(assns_type_name, 1);
  auto const arg2 = name_of_template_arg(assns_type_name, 2);
  result = assns_start + arg1 + ',' + arg0 + ',' + arg2 + '>';
  return result;
}

std::string
art::name_of_assns_base(std::string assns_type_name)
{
  std::string result;
  if (!is_assns(assns_type_name)) {
    return result;
  }
  using namespace std::string_literals;
  static std::string const assns_start = "art::Assns<"s;
  if (name_of_template_arg(assns_type_name, 2) == "void"s) {
    // Doesn't have the base we're looking for.
    return result;
  }
  result = assns_start + name_of_template_arg(assns_type_name, 0) + ',' +
           name_of_template_arg(assns_type_name, 1) + ",void>";
  return result;
}

std::string
art::name_of_unwrapped_product(std::string const& wrapped_name)
{
  using namespace std::string_literals;
  if (!is_instantiation_of(wrapped_name, "art::Wrapper"s)) {
    throw Exception(errors::LogicError, "Can't unwrap"s)
      << "-- attempted to get unwrapped product from non-instance of art::Wrapper."s;
  }
  return name_of_template_arg(wrapped_name, 0);
}
