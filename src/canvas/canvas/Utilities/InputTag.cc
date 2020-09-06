#include "canvas/Utilities/InputTag.h"
// vim: set sw=2 expandtab :

#include "boost/algorithm/string/classification.hpp"
#include "boost/algorithm/string/split.hpp"
#include "canvas/Utilities/Exception.h"
#include "fhiclcpp/coding.h"

#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace art {

  InputTag::~InputTag() = default;
  InputTag::InputTag() = default;

  InputTag::InputTag(string const& label,
                     string const& instance,
                     string const& processName)
    : label_{label}, instance_{instance}, process_{processName}
  {}

  InputTag::InputTag(char const* label,
                     char const* instance,
                     char const* processName)
    : label_{label}, instance_{instance}, process_{processName}
  {}

  InputTag::InputTag(string const& s)
  {
    vector<string> tokens;
    boost::split(tokens, s, boost::is_any_of(":"), boost::token_compress_off);
    auto const nwords = tokens.size();
    if (nwords > 3u) {
      throw Exception(errors::Configuration,
                      "An error occurred while creating an input tag.\n")
        << "The string '" << s
        << "' has more than three colon-delimited tokens.\n"
           "The supported syntax is '<module_label>:<optional instance "
           "name>:<optional process name>'.";
    }
    if (nwords > 0) {
      label_ = tokens[0];
    }
    if (nwords > 1) {
      instance_ = tokens[1];
    }
    if (nwords > 2) {
      process_ = tokens[2];
    }
  }

  InputTag::InputTag(char const* s) : InputTag{string{s}} {}

  InputTag::InputTag(InputTag const& rhs) = default;
  InputTag::InputTag(InputTag&& rhs) = default;

  InputTag& InputTag::operator=(InputTag const& rhs) = default;
  InputTag& InputTag::operator=(InputTag&& rhs) = default;

  bool
  InputTag::operator==(InputTag const& tag) const noexcept
  {
    return (label_ == tag.label_) && (instance_ == tag.instance_) &&
           (process_ == tag.process_);
  }

  bool
  InputTag::empty() const noexcept
  {
    return label_.empty() && instance_.empty() && process_.empty();
  }

  string const&
  InputTag::label() const noexcept
  {
    return label_;
  }

  string const&
  InputTag::instance() const noexcept
  {
    return instance_;
  }

  string const&
  InputTag::process() const noexcept
  {
    return process_;
  }

  string
  InputTag::encode() const
  {
    static string const separator{":"};
    string result = label_;
    if (!instance_.empty() || !process_.empty()) {
      result += separator + instance_;
    }
    if (!process_.empty()) {
      result += separator + process_;
    }
    return result;
  }

  bool
  operator!=(InputTag const& left, InputTag const& right)
  {
    return !(left == right);
  }

  void
  decode(std::any const& a, InputTag& tag)
  {
    if (fhicl::detail::is_sequence(a)) {
      vector<string> tmp;
      fhicl::detail::decode(a, tmp);
      if (tmp.size() == 2) {
        tag = {tmp[0], tmp[1]};
      } else if (tmp.size() == 3) {
        tag = {tmp[0], tmp[1], tmp[2]};
      } else {
        ostringstream errmsg;
        errmsg << "When converting to InputTag by a sequence, FHiCL entries "
                  "must follow the convention:\n\n"
               << "  [ label, instance ], or\n"
               << "  [ label, instance, process_name ].\n\n";
        errmsg << "FHiCL entries provided: [ ";
        for (auto ca = tmp.begin(); ca != tmp.cend(); ++ca) {
          errmsg << *ca;
          if (ca != tmp.cend() - 1) {
            errmsg << ", ";
          }
        }
        errmsg << " ]";
        throw length_error(errmsg.str());
      }
    } else {
      string tmp;
      fhicl::detail::decode(a, tmp);
      tag = tmp;
    }
  }

  ostream&
  operator<<(ostream& os, InputTag const& tag)
  {
    static string const process("', process = '");
    os << "InputTag: label = '" << tag.label() << "', instance = '"
       << tag.instance()
       << (tag.process().empty() ? string() : (process + tag.process())) << "'";
    return os;
  }

} // namespace art
