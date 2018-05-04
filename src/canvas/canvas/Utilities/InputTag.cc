#include "canvas/Utilities/InputTag.h"

#include "boost/algorithm/string/classification.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/any.hpp"
#include "canvas/Utilities/Exception.h"
#include "fhiclcpp/coding.h"

#include <sstream>
#include <stdexcept>
#include <vector>

namespace art {
  void
  InputTag::set_from_string_(std::string const& s)
  {
    // string is delimited by colons
    std::vector<std::string> tokens;
    // cet::split(s, ':', std::back_inserter(tokens));
    boost::split(tokens, s, boost::is_any_of(":"), boost::token_compress_off);

    int nwords = tokens.size();
    if (nwords > 3) {
      throw art::Exception(errors::Configuration, "InputTag")
        << "Input tag " << s << " has " << nwords << " tokens";
    }
    if (nwords > 0)
      label_ = tokens[0];
    if (nwords > 1)
      instance_ = tokens[1];
    if (nwords > 2)
      process_ = tokens[2];
  }

  std::string
  InputTag::encode() const
  {
    // NOTE: since the encoding gets used to form the configuration hash I did
    // not want
    // to change it so that not specifying a process would cause two colons to
    // appear in the encoding and thus not being backwards compatible
    static std::string const separator(":");
    std::string result = label_;
    if (!instance_.empty() || !process_.empty()) {
      result += separator + instance_;
    }
    if (!process_.empty()) {
      result += separator + process_;
    }
    return result;
  }

  std::ostream&
  operator<<(std::ostream& ost, art::InputTag const& tag)
  {
    static std::string const process(", process = ");
    ost << "InputTag:  label = " << tag.label()
        << ", instance = " << tag.instance()
        << (tag.process().empty() ? std::string() : (process + tag.process()));
    return ost;
  }

  void
  decode(boost::any const& a, InputTag& tag)
  {

    if (fhicl::detail::is_sequence(a)) {
      std::vector<std::string> tmp;
      fhicl::detail::decode(a, tmp);
      if (tmp.size() == 2)
        tag = {tmp.at(0), tmp.at(1)};
      else if (tmp.size() == 3)
        tag = {tmp.at(0), tmp.at(1), tmp.at(2)};
      else {
        std::ostringstream errmsg;
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
        throw std::length_error(errmsg.str());
      }
    } else {
      std::string tmp;
      fhicl::detail::decode(a, tmp);
      tag = tmp;
    }
  }
}
