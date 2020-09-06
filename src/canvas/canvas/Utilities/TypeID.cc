#include "canvas/Utilities/TypeID.h"
// vim: set sw=2 expandtab :

#include "canvas/Utilities/Exception.h"
#include "canvas/Utilities/FriendlyName.h"
#include "canvas/Utilities/uniform_type_name.h"
#include "hep_concurrency/RecursiveMutex.h"

#include <cstddef>
#include <map>
#include <ostream>
#include <string>
#include <typeinfo>
#include <utility>

using namespace hep::concurrency;
using namespace std;

namespace art {

  RecursiveMutex* TypeID::s_mutex{nullptr};
  map<size_t, string>* TypeID::s_nameMap{nullptr};

  void
  TypeID::startup()
  {
    if (s_mutex == nullptr) {
      s_mutex = new RecursiveMutex{"TypeID::s_mutex"};
      s_nameMap = new map<size_t, string>;
    }
  }

  void
  TypeID::shutdown()
  {
    delete s_nameMap;
    s_nameMap = nullptr;
    delete s_mutex;
    s_mutex = nullptr;
  }

  struct TypeIDCleanup {
    ~TypeIDCleanup() { TypeID::shutdown(); }
    TypeIDCleanup() { TypeID::startup(); }
  } typeIDCleaner_;

  TypeID::~TypeID() noexcept = default;
  TypeID::TypeID() noexcept = default;
  TypeID::TypeID(TypeID const&) noexcept = default;
  TypeID::TypeID(TypeID&) noexcept = default;
  TypeID& TypeID::operator=(TypeID const&) noexcept = default;
  TypeID& TypeID::operator=(TypeID&) noexcept = default;

  TypeID::TypeID(type_info const& ti) noexcept : ti_{&ti} {}

  TypeID::TypeID(type_info const* ti) noexcept : ti_{ti} {}

  type_info const&
  TypeID::typeInfo() const
  {
    return *ti_;
  }

  char const*
  TypeID::name() const
  {
    return ti_->name();
  }

  string
  TypeID::className() const
  {
    RecursiveMutexSentry sentry{*s_mutex, __func__};
    auto hash_code = typeInfo().hash_code();
    auto entry = s_nameMap->find(hash_code);
    if (entry == s_nameMap->end()) {
      entry =
        s_nameMap->emplace(hash_code, uniform_type_name(typeInfo())).first;
    }
    return entry->second;
  }

  string
  TypeID::friendlyClassName() const
  {
    return friendlyname::friendlyName(className());
  }

  bool
  TypeID::operator<(TypeID const& rhs) const
  {
    return ti_->before(*rhs.ti_);
  }

  bool
  TypeID::operator==(TypeID const& rhs) const
  {
    return *ti_ == *rhs.ti_;
  }

  TypeID::operator bool() const { return ti_ != nullptr; }

  void
  TypeID::swap(TypeID& other)
  {
    std::swap(ti_, other.ti_);
  }

  void
  TypeID::print(ostream& os) const
  {
    os << className();
  }

  bool
  operator>(TypeID const& a, TypeID const& b)
  {
    return b < a;
  }

  bool
  operator!=(TypeID const& a, TypeID const& b)
  {
    return !(a == b);
  }

  void
  swap(TypeID& left, TypeID& right)
  {
    left.swap(right);
  }

  ostream&
  operator<<(ostream& os, TypeID const& tid)
  {
    tid.print(os);
    return os;
  }

  string
  name_of_template_arg(string const& template_instance, size_t desired_arg)
  {
    string result;
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

  string
  name_of_assns_partner(string assns_type_name)
  {
    string result;
    if (!is_assns(assns_type_name)) {
      return result;
    }
    static string const assns_start = "art::Assns<"s;
    auto const arg0 = name_of_template_arg(assns_type_name, 0);
    auto const arg1 = name_of_template_arg(assns_type_name, 1);
    auto const arg2 = name_of_template_arg(assns_type_name, 2);
    result = assns_start + arg1 + ',' + arg0 + ',' + arg2 + '>';
    return result;
  }

  string
  name_of_assns_base(string assns_type_name)
  {
    string result;
    if (!is_assns(assns_type_name)) {
      return result;
    }
    using namespace std::string_literals;
    static string const assns_start = "art::Assns<"s;
    if (name_of_template_arg(assns_type_name, 2) == "void"s) {
      // Doesn't have the base we're looking for.
      return result;
    }
    result = assns_start + name_of_template_arg(assns_type_name, 0) + ',' +
             name_of_template_arg(assns_type_name, 1) + ",void>";
    return result;
  }

  string
  name_of_unwrapped_product(string const& wrapped_name)
  {
    using namespace std::string_literals;
    if (!is_instantiation_of(wrapped_name, "art::Wrapper"s)) {
      throw Exception(errors::LogicError, "Can't unwrap"s)
        << "-- attempted to get unwrapped product from non-instance of art::Wrapper."s;
    }
    return name_of_template_arg(wrapped_name, 0);
  }

} // namespace art
