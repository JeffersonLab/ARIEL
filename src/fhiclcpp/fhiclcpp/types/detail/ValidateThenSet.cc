#include "fhiclcpp/types/detail/ValidateThenSet.h"
#include "cetlib/container_algorithms.h"
#include "fhiclcpp/exception.h"
#include "fhiclcpp/types/detail/AtomBase.h"
#include "fhiclcpp/types/detail/NameStackRegistry.h"
#include "fhiclcpp/types/detail/ParameterBase.h"
#include "fhiclcpp/types/detail/PrintAllowedConfiguration.h"
#include "fhiclcpp/types/detail/SequenceBase.h"
#include "fhiclcpp/types/detail/TableBase.h"
#include "fhiclcpp/types/detail/optional_parameter_message.h"
#include "fhiclcpp/types/detail/strip_containing_names.h"
#include "fhiclcpp/types/detail/validationException.h"

#include <iomanip>
#include <regex>

//====================================================================

bool
fhicl::detail::ValidateThenSet::before_action(ParameterBase& p)
{
  // 'ConfigPredicate' condition must be satisfied to continue.
  if (!p.should_use())
    return false;

  // Check that key exists; allow defaulted or optional keys to be
  // absent.
  std::string const& k = strip_first_containing_name(p.key());
  if (!pset_.has_key(k) && !cet::search_all(ignorableKeys_, k)) {
    if (!p.has_default() && !p.is_optional()) {
      missingParameters_.emplace_back(&p);
    }
    return false;
  }

  auto erase_from = std::remove(userKeys_.begin(), userKeys_.end(), k);
  userKeys_.erase(erase_from, userKeys_.cend());
  return true;
}

void
fhicl::detail::ValidateThenSet::after_action(ParameterBase& p)
{
  p.set_value(pset_, true);
}

//====================================================================

void
fhicl::detail::ValidateThenSet::enter_sequence(SequenceBase& s)
{
  // Ensure that the supplied parameter represents a sequence.
  auto const& key = strip_first_containing_name(s.key());
  if (!pset_.is_key_to_sequence(key)) {
    throw fhicl::exception(type_mismatch, "error converting to sequence:\n")
      << "The supplied value of the parameter:\n"
      << "    " << s.key() << '\n'
      << "does not represent a sequence.\n";
  }

  std::regex const r{fhicl::Name::regex_safe(key) + "\\[\\d+\\]"};
  std::size_t const nElems =
    std::count_if(userKeys_.begin(), userKeys_.end(), [&r](auto const& k) {
      return std::regex_match(k, r);
    });
  s.prepare_elements_for_validation(nElems);
}

//====================================================================

void
fhicl::detail::ValidateThenSet::delegated_parameter(DelegateBase& dp)
{
  // A delegated parameter must itself be present, but any nested
  // parameters do not need to be present since the nested parameters
  // are potentially validated elsewhere.
  auto const& name = dp.name();
  std::string const pattern{fhicl::Name::regex_safe(name) + R"((\.|\[))"};
  std::regex const r{pattern};
  auto erase_from =
    std::remove_if(userKeys_.begin(), userKeys_.end(), [&r](auto const& k) {
      return std::regex_search(k, r);
    });
  userKeys_.erase(erase_from, userKeys_.end());
}

//====================================================================

namespace {

  using fhicl::detail::ParameterBase;
  using fhicl::detail::PrintAllowedConfiguration;
  using fhicl::detail::strip_first_containing_name;
  using key_set = std::set<std::string>;

  void
  removeIgnorableKeys(key_set const& ignorable,
                      std::vector<std::string>& extra,
                      std::vector<cet::exempt_ptr<ParameterBase>>& missing)
  {
    for (auto const& key : ignorable) {

      auto const& subkey = strip_first_containing_name(key);

      // Allow erasure globbing for extra keys (if "parameter" is an
      // ignorable key, then "parameter.a" is also ignorable)
      auto it = cet::find_in_all(extra, subkey);
      if (it != extra.cend()) {
        auto match = [&subkey](std::string const& key) {
          return key.find(subkey) == 0ul;
        };
        auto const end = std::find_if_not(it, extra.end(), match);
        extra.erase(it, end);
      }

      // Since all ignorable missing keys are set explicitly, we do
      // not glob erasures.
      auto mit =
        std::remove_if(missing.begin(), missing.end(), [&subkey](auto p) {
          return p->key() == subkey;
        });

      missing.erase(mit, missing.end());
    }
  }

  inline bool
  show_parents(std::string const& k)
  {
    std::size_t const freq =
      std::count(k.begin(), k.end(), '.') + std::count(k.begin(), k.end(), '[');
    return freq > 1;
  }

  std::string
  fillMissingKeysMsg(
    std::vector<cet::exempt_ptr<ParameterBase>> const& missingParams)
  {
    if (missingParams.empty())
      return "";

    std::string const prefix{" - " + std::string(3, ' ')};

    std::ostringstream oss;
    oss << "Missing parameters:\n";
    for (auto p : missingParams) {

      // If the key is nested (e.g. pset1.pset2[0] ), show the
      // parents
      PrintAllowedConfiguration pc{oss, show_parents(p->key()), prefix, true};
      pc.walk_over(*p);
    }
    oss << "\n";

    return oss.str();
  }

  std::string
  fillExtraKeysMsg(fhicl::ParameterSet const& pset,
                   std::vector<std::string> const& extraKeys)
  {
    if (extraKeys.empty())
      return "";

    std::ostringstream oss;
    oss << "Unsupported parameters:\n\n";
    for (auto const& key : extraKeys) {
      oss << " + " << std::setw(30) << std::left << key << " [ "
          << pset.get_src_info(key) << " ]\n";
    }
    oss << '\n';

    return oss.str();
  }
}

void
fhicl::detail::ValidateThenSet::check_keys()
{
  removeIgnorableKeys(ignorableKeys_, userKeys_, missingParameters_);
  std::string errmsg;
  errmsg += fillMissingKeysMsg(missingParameters_);
  errmsg += fillExtraKeysMsg(pset_, userKeys_);
  if (!errmsg.empty()) {
    std::string fullmsg{detail::optional_parameter_message(false)};
    fullmsg += "\n";
    fullmsg += errmsg;
    throw validationException{fullmsg.c_str()};
  }
}
