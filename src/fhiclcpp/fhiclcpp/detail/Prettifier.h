#ifndef fhiclcpp_detail_Prettifier_h
#define fhiclcpp_detail_Prettifier_h

/*
  ======================================================================

  Prettifier

  ======================================================================

  Class used when

    'ParameterSet::to_indented_string(unsigned,print_mode::raw)'

  is called.  This class provides a human-readable string
  representing the entire (nested) contents of a ParameterSet object.

  Currently supported format:
  ===========================

  This will provide a print out that looks like:


     ^<-IND->
     ^       p1: {}$                          p1: {}
     ^       p2: {$                           p2: {
     ^          a: something$                    a: something
     ^       }$                               }
     ^       p3: {$                           p3: {
     ^          a: else$                         a: else
     ^          b: []$                           b: []
     ^          c: [$          Rendered          c: [
     ^             11$         ========>            11
     ^          ]$                               ]
     ^          d: [$                            d: [
     ^             11,$                             11,
     ^             12$                              12
     ^          ]$                               ]
     ^          p4: {$                           p4: {
     ^            d: e$                            d: e
     ^          }$                               }
     ^       }$                               }
     ^<-IND->

  Note that the caret ^ (dollar-sign $) represents the beginning
  (end) of the line and is not printed, nor is the <-IND-> string,
  which represents a user-provided width value for the initial
  indentation level.

  Maintenance notes:
  ==================

  [1] A couple stack objects are used: the Indentation class, as well
      as std::stack<std::size_t>, where the latter represents the
      sizes of the stacked sequences.  Keeping track of the sequence
      is necessary so that the last sequence element does not have a
      ',' character that follows it.

      To use these classes correctly, the Indentation must be updated
      during each {enter,exit}_{table,sequence} call, and the
      sequence-sizes stack must be updated during each
      {enter,exit}_sequence call.

  [2] The 'maybe_{indent,nl}_' functions are provided so that empty
      sequences and tables do not appear as

                seq: [       and        table: {
                ]                       }

      but rather

                seq: []      and        table: {}

*/

#include "fhiclcpp/ParameterSetWalker.h"
#include "fhiclcpp/coding.h"
#include "fhiclcpp/detail/Indentation.h"
#include "fhiclcpp/fwd.h"

#include <sstream>
#include <stack>
#include <string>

namespace fhicl::detail {

  class Prettifier : public ParameterSetWalker {
  public:
    Prettifier(unsigned initial_indent_level = 0);

    std::string
    result() const
    {
      return buffer_.str();
    }

  private:
    void enter_table(key_t const&, any_t const&) override;
    void enter_sequence(key_t const&, any_t const&) override;

    void exit_table(key_t const&, any_t const&) override;
    void exit_sequence(key_t const&, any_t const&) override;

    void atom(key_t const&, any_t const&) override;

    void before_action(key_t const&,
                       any_t const&,
                       ParameterSet const*) override;

    void push_size_(any_t const&);
    void pop_size_();
    std::string maybe_indent_(std::size_t);
    std::string maybe_nl_(std::size_t);

    std::ostringstream buffer_{};
    Indentation indent_;
    std::stack<std::size_t> sequence_sizes_;
    std::size_t seq_size_;
    std::size_t table_size_;
  };
}

#endif /* fhiclcpp_detail_Prettifier_h */

// Local variables:
// mode: c++
// End:
