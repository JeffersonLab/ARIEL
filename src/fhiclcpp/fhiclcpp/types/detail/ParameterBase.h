#ifndef fhiclcpp_types_detail_ParameterBase_h
#define fhiclcpp_types_detail_ParameterBase_h

/*
  ParameterBase is the most fundamental base class for all fhiclcpp
  types.  The taxonomy is:


          ParameterBase
         /      |    \ \____________________________________
        /       |     \___________________                  \
       /        |                         \                  \
      /         |                          \                  \
  AtomBase   TableBase                  SequenceBase         DelegateBase
     |          |                      /      |     \             \
     |          |                     /       |      \             \
  Atom<T>    Table<T>   Sequence<T,SZ>  Sequence<T>   Tuple<T...>
  DelegatedParameter


  All concrete Optional* fhiclcpp types also inherit from the
  corresponding base classes (e.g. OptionalAtom<T> inherits from
  AtomBase, not OptionalAtomBase).  The design is meant to closely
  follow the classification of FHiCL values, as described in the FHiCL
  language quick start guide.
*/

#include "fhiclcpp/types/ConfigPredicate.h"
#include "fhiclcpp/types/detail/ParameterArgumentTypes.h"
#include "fhiclcpp/types/detail/ParameterMetadata.h"

#include <string>

namespace fhicl {

  class ParameterSet;

  namespace detail {

    //========================================================
    class ParameterBase {
    public:
      std::string const&
      key() const
      {
        return mdata_.key();
      }
      std::string const&
      name() const
      {
        return mdata_.name();
      }
      std::string const&
      comment() const
      {
        return mdata_.comment();
      }
      bool
      has_default() const
      {
        return mdata_.has_default();
      }
      bool
      is_optional() const
      {
        return mdata_.is_optional();
      }
      bool
      is_conditional() const
      {
        return mdata_.is_conditional();
      }
      par_type
      parameter_type() const
      {
        return mdata_.type();
      }
      bool
      should_use() const
      {
        return maybeUse_();
      }

      ParameterBase(Name const& name,
                    Comment const& comment,
                    par_style const vt,
                    par_type const type,
                    std::function<bool()> maybeUse = AlwaysUse())
        : mdata_{name, comment, vt, type}, maybeUse_{maybeUse}
      {}

      virtual ~ParameterBase() = default;

      // Modifiers
      void
      set_value(fhicl::ParameterSet const& ps, bool trimParents)
      {
        do_set_value(ps, trimParents);
      }
      void
      set_par_style(par_style const vt)
      {
        mdata_.set_par_style(vt);
      }

    private:
      virtual void do_set_value(fhicl::ParameterSet const&,
                                bool trimParents) = 0;

      ParameterMetadata mdata_;
      std::function<bool()> maybeUse_;
    };
  }
}

#endif /* fhiclcpp_types_detail_ParameterBase_h */

// Local variables:
// mode: c++
// End:
