#ifndef fhiclcpp_types_detail_SequenceBase_h
#define fhiclcpp_types_detail_SequenceBase_h

#include "fhiclcpp/type_traits.h"
#include "fhiclcpp/types/detail/ParameterBase.h"

#include <array>
#include <tuple>
#include <vector>

namespace fhicl {
  namespace detail {

    template <tt::const_flavor C>
    class ParameterWalker;

    //========================================================
    class SequenceBase : public ParameterBase {
    public:
      SequenceBase(Name&& name,
                   Comment&& comment,
                   par_style const vt,
                   par_type const type,
                   std::function<bool()> maybeUse)
        : ParameterBase{name, comment, vt, type, maybeUse}
      {}

      bool
      empty() const
      {
        return size() == 0;
      }
      std::size_t
      size() const
      {
        return get_size();
      }

      void
      prepare_elements_for_validation(std::size_t const n)
      {
        do_prepare_elements_for_validation(n);
      }
      void
      walk_elements(ParameterWalker<tt::const_flavor::require_non_const>& pw)
      {
        do_walk_elements(pw);
      }
      void
      walk_elements(ParameterWalker<tt::const_flavor::require_const>& pw) const
      {
        do_walk_elements(pw);
      }

    private:
      virtual std::size_t get_size() const = 0;

      virtual void do_prepare_elements_for_validation(std::size_t) = 0;
      virtual void do_walk_elements(
        ParameterWalker<tt::const_flavor::require_non_const>&) = 0;
      virtual void do_walk_elements(
        ParameterWalker<tt::const_flavor::require_const>&) const = 0;
    };
  }
}

#endif /* fhiclcpp_types_detail_SequenceBase_h */

// Local variables:
// mode: c++
// End:
