#ifndef fhiclcpp_types_detail_ParameterMetadata_h
#define fhiclcpp_types_detail_ParameterMetadata_h

#include "fhiclcpp/types/Comment.h"
#include "fhiclcpp/types/Name.h"
#include "fhiclcpp/types/detail/NameStackRegistry.h"
#include "fhiclcpp/types/detail/ParameterArgumentTypes.h"
#include "fhiclcpp/types/detail/strip_containing_names.h"

#include <string>

namespace fhicl {
  namespace detail {

    class ParameterBase;

    class ParameterMetadata {
    public:
      ParameterMetadata() = default;

      ParameterMetadata(Name const& name = Name(),
                        Comment const& comment = Comment(""),
                        par_style const parStyle = par_style::NTYPES,
                        par_type const parType = par_type::NTYPES)
        : key_{NameStackRegistry::instance().full_key(name.value)}
        , name_{name.value}
        , comment_{comment.value}
        , parStyle_{parStyle}
        , parType_{parType}
      {}

      std::string
      key() const
      {
        return key_;
      }
      std::string
      name() const
      {
        return name_;
      }
      std::string
      comment() const
      {
        return comment_;
      }

      bool
      has_default() const
      {
        return parStyle_ == par_style::DEFAULT ||
               parStyle_ == par_style::DEFAULT_CONDITIONAL;
      }

      bool
      is_optional() const
      {
        return parStyle_ == par_style::OPTIONAL ||
               parStyle_ == par_style::OPTIONAL_CONDITIONAL;
      }

      bool
      is_conditional() const
      {
        return parStyle_ == par_style::REQUIRED_CONDITIONAL ||
               parStyle_ == par_style::OPTIONAL_CONDITIONAL ||
               parStyle_ == par_style::DEFAULT_CONDITIONAL;
      }

      par_type
      type() const
      {
        return parType_;
      }

      void
      set_key(std::string const& key)
      {
        key_ = key;
        name_ = detail::strip_all_containing_names(key);
      }

      void
      set_par_style(par_style const vt)
      {
        parStyle_ = vt;
      }

    private:
      std::string key_{};
      std::string name_{};
      std::string comment_{};
      par_style parStyle_{par_style::NTYPES};
      par_type parType_{par_type::NTYPES};
    };
  }
}

#endif /* fhiclcpp_types_detail_ParameterMetadata_h */

// Local variables:
// mode: c++
// End:
