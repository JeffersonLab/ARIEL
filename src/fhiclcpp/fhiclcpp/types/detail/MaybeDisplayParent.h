#ifndef fhiclcpp_types_detail_MaybeDisplayParent_h
#define fhiclcpp_types_detail_MaybeDisplayParent_h

#include "cetlib/exempt_ptr.h"
#include "fhiclcpp/detail/Indentation.h"
#include "fhiclcpp/types/detail/ParameterBase.h"

#include <cctype>
#include <sstream>
#include <vector>

//===================================================================================
namespace fhicl {
  namespace detail {

    using fhicl::detail::ParameterBase;
    using base_ptr = cet::exempt_ptr<ParameterBase>;

    class MaybeDisplayParent {
    public:
      MaybeDisplayParent(ParameterBase const& p,
                         bool const showParents,
                         Indentation& ind)
        : show_{showParents}
        , names_{showParents ? get_parents(p.key()) :
                               std::vector<std::string>{}}
        , indent_{ind}
      {
        std::ostringstream os;

        if (show_) {

          for (auto it = names_.begin(), e = names_.end() - 1; it != e; ++it) {
            std::string const& name = *it;
            std::string const& next_name = *std::next(it);

            // We use the indent_ member so that we can properly
            // update the indentation level.  However, the literal
            // indented string should not include any characters in it
            // for the parents of the parameter in question.
            std::string const indent(indent_().size(), ' ');
            if (!std::isdigit(name[0])) {
              if (std::isdigit(next_name[0])) {
                os << indent << name << ": [  # index: " << next_name << '\n';
                closingBraces_.push_back(indent + "]\n");
              } else {
                os << indent << name << ": {\n";
                closingBraces_.push_back(indent + "}\n");
              }
            } else {
              if (!std::isdigit(next_name[0])) {
                os << indent << "{\n";
                closingBraces_.push_back(indent + "}\n");
              } else {
                os << indent << "[  # index: " << next_name << '\n';
                closingBraces_.push_back(indent + "]\n");
              }
            }
            indent_.push();
          }
        } else {
        }

        fullPayload_ = os.str();
      }

      std::string const&
      parent_names() const
      {
        return fullPayload_;
      }

      std::string
      closing_braces() const
      {
        if (!show_)
          return "";

        std::string result;
        for (auto it = closingBraces_.crbegin(), re = closingBraces_.crend();
             it != re;
             ++it) {
          result.append(*it);
          indent_.pop();
        }
        return result;
      }

    private:
      bool show_;
      std::string fullPayload_{};
      std::vector<std::string> names_;
      std::vector<std::string> closingBraces_{};
      Indentation& indent_;

      std::vector<std::string> get_parents(std::string const& k);

      bool
      is_sequence_element(ParameterBase const& p)
      {
        auto pos = p.key().find_last_of("]");
        return pos != std::string::npos && pos == p.key().size() - 1;
      }
    };
  }
}

#endif /* fhiclcpp_types_detail_MaybeDisplayParent_h */

// Local variables:
// mode: c++
// End:
