#ifndef fhiclcpp_types_detail_ParameterWalker_h
#define fhiclcpp_types_detail_ParameterWalker_h

/*
  ======================================================================

  ParameterWalker

  ======================================================================

  The ParameterWalker class makes it possible to traverse the nested
  configuration for a given parameter.  This class is very similar in
  functionality to the 'ParameterSet::walk_' function.

  The tree-walking functionality is provided via derivation:

      class MyWalker : ParameterWalker<tt::const_flavor::require(_non)_const>
      {
        // members, functions, etc.
      };

  where a provided template argument of
  'tt::const_flavor::require_const ' makes all virtual functions
  accept "T const*" arguments, whereas require_non_const makes all
  virtual functions accept "T*" arguments.

  Heuristically, it looks like:

      for ( auto p : parameters ) {

         psw.before_action(p)

         if ( is_table(param) ) {
             cast_from(p,t)
         *** psw.enter_table(t)
             descend_into_table ...
             psw.exit_table(t)
         }
         else if ( is_sequence(param) ) {
             cast_from(p,s)
         *** psw.enter_sequence(s)
             loop_through_sequence ...
             psw.exit_sequence(s)
         }
         else if (is_atom(param)) {
             cast_from(p,a)
         *** psw.atom(a)
         }
         else {
             cast_from(p,dp)
         *** psw.delegated_parameter(dp)
         }

         psw.after_action(p)

      }

  The actions that are to be taken per parameter category (table,
  sequence, or atom) are defined entirely by overrides to the
  ParameterWalker virtual functions that 'psw' calls (as shown above).

  The function calls prefaced with '***' correspond to pure virtual
  functions, which must have corresponding overrides in any derived
  classes.

  The 'exit_{table,sequence}' functions are provided if (e.g.) the
  derived-class state needs to be restored after the table or sequence
  traversal.  The '{before,after}_action' virtual functions are
  provided so that category-agnostic instructions can be executed
  before or after the category-specific ones.
*/

#include "cetlib/container_algorithms.h"
#include "cetlib_except/demangle.h"
#include "fhiclcpp/type_traits.h"
#include "fhiclcpp/types/detail/AtomBase.h"
#include "fhiclcpp/types/detail/DelegateBase.h"
#include "fhiclcpp/types/detail/SequenceBase.h"
#include "fhiclcpp/types/detail/TableBase.h"

namespace fhicl {
  namespace detail {

    template <tt::const_flavor C>
    class ParameterWalker {
    public:
      ParameterWalker() = default;
      virtual ~ParameterWalker() = default;

      void walk_over(tt::maybe_const_t<ParameterBase, C>&);

      bool
      do_before_action(tt::maybe_const_t<ParameterBase, C>& p)
      {
        return before_action(p);
      }
      void
      do_after_action(tt::maybe_const_t<ParameterBase, C>& p)
      {
        after_action(p);
      }

      void
      do_enter_table(tt::maybe_const_t<TableBase, C>& t)
      {
        enter_table(t);
      }
      void
      do_exit_table(tt::maybe_const_t<TableBase, C>& t)
      {
        exit_table(t);
      }

      void
      do_enter_sequence(tt::maybe_const_t<SequenceBase, C>& s)
      {
        enter_sequence(s);
      }
      void
      do_exit_sequence(tt::maybe_const_t<SequenceBase, C>& s)
      {
        exit_sequence(s);
      }

      void
      do_atom(tt::maybe_const_t<AtomBase, C>& a)
      {
        atom(a);
      }

      void
      do_delegated_parameter(tt::maybe_const_t<DelegateBase, C>& dp)
      {
        delegated_parameter(dp);
      }

    private:
      virtual void enter_table(tt::maybe_const_t<TableBase, C>&) = 0;
      virtual void enter_sequence(tt::maybe_const_t<SequenceBase, C>&) = 0;
      virtual void atom(tt::maybe_const_t<AtomBase, C>&) = 0;
      virtual void delegated_parameter(tt::maybe_const_t<DelegateBase, C>&) = 0;

      virtual bool
      before_action(tt::maybe_const_t<ParameterBase, C>&)
      {
        return true;
      }
      virtual void
      after_action(tt::maybe_const_t<ParameterBase, C>&)
      {}
      virtual void
      exit_table(tt::maybe_const_t<TableBase, C>&)
      {}
      virtual void
      exit_sequence(tt::maybe_const_t<SequenceBase, C>&)
      {}
    };

    //=============================================================================
    // IMPLEMENTATION BELOW

    template <tt::const_flavor C>
    void
    ParameterWalker<C>::walk_over(tt::maybe_const_t<ParameterBase, C>& p)
    {
      if (!do_before_action(p))
        return;

      fhicl::par_type const pt = p.parameter_type();

      auto& tw = *this;

      if (is_table(pt)) {
        using maybe_const_table = tt::maybe_const_t<TableBase, C>;
        maybe_const_table& t = dynamic_cast<maybe_const_table&>(p);
        do_enter_table(t);
        cet::for_all(t.members(), [&tw](auto m) { tw.walk_over(*m); });
        do_exit_table(t);
      } else if (is_sequence(pt)) {
        using maybe_const_sequence = tt::maybe_const_t<SequenceBase, C>;
        maybe_const_sequence& s = dynamic_cast<maybe_const_sequence&>(p);
        do_enter_sequence(s);
        s.walk_elements(tw);
        do_exit_sequence(s);
      } else if (is_atom(pt)) {
        using maybe_const_atom = tt::maybe_const_t<AtomBase, C>;
        maybe_const_atom& a = dynamic_cast<maybe_const_atom&>(p);
        do_atom(a);
      } else {
        using maybe_const_delegate = tt::maybe_const_t<DelegateBase, C>;
        maybe_const_delegate& dp = dynamic_cast<maybe_const_delegate&>(p);
        do_delegated_parameter(dp);
      }

      do_after_action(p);
    }
  }
}

#endif /* fhiclcpp_types_detail_ParameterWalker_h */

// Local variables:
// mode: c++
// End:
