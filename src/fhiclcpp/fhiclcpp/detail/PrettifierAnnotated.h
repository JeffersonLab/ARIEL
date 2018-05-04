#ifndef fhiclcpp_detail_PrettifierAnnotated_h
#define fhiclcpp_detail_PrettifierAnnotated_h

/*
  ======================================================================

  PrettifierAnnotated

  ======================================================================

  Class used when

    'ParameterSet::to_indented_string(unsigned,print_mode::annotated)'

  is called.  This class provides a human-readable string representing
  the entire (nested) contents of a ParameterSet object, as well as
  annotations that describe in what FHiCL configuration file, and in
  what line of that file, the particular parameter was set or
  reassigned.

  Currently supported format:
  ===========================

  This will provide a print out that looks like:


     ^<-IND->
     ^       p1: {  # file:1$                         p1: {  # file:1
     ^       }$                                       }
     ^       p2: {  # file:2$                         p2: {  # file:2
     ^          a: something  # ""$                      a: something  # ""
     ^       }$                                       }
     ^       p3: {  # new_file:14$                    p3: {  # new_file:14
     ^          a: else  # new_file:15$                  a: else  # new_file:15
     ^          b: [  # file:7$                          b: [  # file:7
     ^          ]$                                       ]
     ^          c: [  # file:9$          Rendered        c: [  # file:9
     ^             11  # ""$             ========>          11  # ""
     ^          ]$                                       ]
     ^          d: [  # file:14$                         d: [  # file:14
     ^             11,  # ""$                               11,  # ""
     ^             12  # file:28$                           12  # file:28
     ^          ]$                                       ]
     ^          p4: {  # new_file:16$                    p4: {  # new_file:16
     ^            d: e  # new_file:17$                     d: e  # new_file:17
     ^          }$                                       }
     ^       }$                                       }
     ^<-IND->

  Note that the caret ^ (dollar-sign $) represents the beginning (end)
  of the line and is not printed, nor is the <-IND-> string, which
  represents a user-provided width value for the initial indentation
  level.  The double quotes "" are printed whenever the annotation
  from the previous line is the same as the current.

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

  [2] There are cases where the annotation information is not
      available:

        (a) The parameter was introduced in the C++ code (either by the
            user, or by the system) and, therefore, has no file source.

        (b) The parameter was introduced through a substitution:

              p1: { a: b }
              p1.p2.c: d

            In this case, 'p1', 'a', and 'c' would each have source
            annotations, but 'p2' would not.

        (c) The parameter originates from an external source (e.g. a
            database), and no file source exists.

      In these cases, no annotation is provided.

  [3] To determine if the source annotation is the same as what has
      been previously inserted into the buffer, we cache the source
      information during the 'after_action' step.

*/

#include "fhiclcpp/ParameterSetWalker.h"
#include "fhiclcpp/coding.h"
#include "fhiclcpp/detail/Indentation.h"
#include "fhiclcpp/fwd.h"

#include <sstream>
#include <stack>
#include <string>

namespace fhicl {
  namespace detail {

    class PrettifierAnnotated : public ParameterSetWalker {
    public:
      PrettifierAnnotated(unsigned initial_indent_level = 0);

      std::string
      result() const
      {
        return buffer_.str();
      }

    private:
      void before_action(key_t const&,
                         any_t const&,
                         ParameterSet const*) override;
      void after_action() override;

      void enter_table(key_t const&, any_t const&) override;
      void enter_sequence(key_t const&, any_t const&) override;

      void exit_table(key_t const&, any_t const&) override;
      void exit_sequence(key_t const&, any_t const&) override;

      void atom(key_t const&, any_t const&) override;

      void push_size_(any_t const&);
      void pop_size_();

      std::ostringstream buffer_;
      Indentation indent_;
      std::string curr_info_;
      std::string cached_info_;
      std::stack<std::size_t> sequence_sizes_;
      std::size_t curr_size_;
    };
  }
}

#endif /* fhiclcpp_detail_PrettifierAnnotated_h */

// Local variables:
// mode: c++
// End:
