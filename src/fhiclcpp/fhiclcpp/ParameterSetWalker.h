#ifndef fhiclcpp_ParameterSetWalker_h
#define fhiclcpp_ParameterSetWalker_h

/*

  ======================================================================

  ParameterSetWalker

  ======================================================================

  There are cases where we must walk over the entire ParameterSet
  tree, including descending into its nested tables and looping over
  its sequence elements.  The 'ParameterSet::walk_' function and the
  'ParameterSetWalker' interface below make this possible in a way
  that allows for flexibility in what kind of actions we want
  performed for each table, sequence or atom.

  The tree-walking functionality is provided by

      'ParameterSet::walk_( ParameterSetWalker& psw)'

  Heuristically, it looks like:

      for (auto const& param : pset_parameters) {

         psw.before_action()

         if (is_table(param)) {
         *** psw.enter_table()
             descend_into_table ...
             psw.exit_table()
         }
         else if (is_sequence(param)) {
         *** psw.enter_sequence()
             loop_through_sequence ...
             psw.exit_sequence()
         }
         else {
         *** psw.atom()
         }

         psw.after_action()

      }

  The actions that are to be taken per parameter category (table,
  sequence, or atom) are defined entirely by overrides to the
  ParameterSetWalker virtual functions that 'psw' calls (as shown
  above).  In other words,

  <<  NO ParameterSet TREE WALKING SHOULD BE DONE BY >>
  <<  ANY CONCRETE ParameterSetWalker INSTANCE.      >>

  The function calls prefaced with '***' correspond to pure virtual
  functions, which must have corresponding overrides in any derived
  classes.

  The 'exit_{table,sequence}' functions are provided if (e.g.) the
  derived-class state needs to be restored after the table or sequence
  traversal.  The '{before,after}_action' virtual functions are
  provided so that category-agnostic instructions can be executed
  before or after the category-specific ones.

*/

#include "boost/any.hpp"

#include <string>

namespace fhicl {

  class ParameterSet;

  class ParameterSetWalker {
  public:
    virtual ~ParameterSetWalker() noexcept = default;

    using key_t = std::string;
    using any_t = boost::any;

    void
    do_enter_table(key_t const& k, any_t const& a)
    {
      enter_table(k, a);
    }
    void
    do_enter_sequence(key_t const& k, any_t const& a)
    {
      enter_sequence(k, a);
    }
    void
    do_atom(key_t const& k, any_t const& a)
    {
      atom(k, a);
    }

    void
    do_before_action(key_t const& k, any_t const& a, ParameterSet const* ps)
    {
      before_action(k, a, ps);
    }
    void
    do_after_action()
    {
      after_action();
    }

    void
    do_exit_table(key_t const& k, any_t const& a)
    {
      exit_table(k, a);
    }
    void
    do_exit_sequence(key_t const& k, any_t const& a)
    {
      exit_sequence(k, a);
    }

  private:
    virtual void enter_table(key_t const&, any_t const&) = 0;
    virtual void enter_sequence(key_t const&, any_t const&) = 0;
    virtual void atom(key_t const&, any_t const&) = 0;

    virtual void
    exit_table(key_t const&, any_t const&)
    {}
    virtual void
    exit_sequence(key_t const&, any_t const&)
    {}

    // Pointer to enclosing ParameterSet object provided
    virtual void
    before_action(key_t const&, any_t const&, ParameterSet const*)
    {}
    virtual void
    after_action()
    {}
  };

} // fhicl

#endif /* fhiclcpp_ParameterSetWalker_h */

// Local variables:
// mode: c++
// End:
