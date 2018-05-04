#ifndef fhiclcpp_detail_PrettifierPrefixAnnotated_h
#define fhiclcpp_detail_PrettifierPrefixAnnotated_h

/*
  ======================================================================

  PrettifierPrefixAnnotated

  ======================================================================

  Class used when

    'ParameterSet::to_indented_string(unsigned,print_mode::prefix_annotated)'

  is called.  This class provides a string representing the entire
  (nested) contents of a ParameterSet object, as well as annotations
  that describe in what FHiCL configuration file, and in what line of
  that file, the particular parameter was set or reassigned.  This
  version provides annotations on the line preceding the parameter
  assignment, which can help users who wish to parse the stringified
  representation with their own scripts.

  Currently supported format:
  ===========================

  This will provide a print out that looks like:

     ^<-IND->
     ^#KEY|p1|$                                    #KEY|p1|
     ^#SRC|file:1|$                                #SRC|file:1|
     ^       p1: {$                                       p1: {
     ^       }$                                           }
     ^#KEY|p2|$                                    #KEY|p2|
     ^#SRC|file:2|$                                #SRC|file:2|
     ^       p2: {$                                       p2: {
     ^#KEY|p2.a|$                                  #KEY|p2.a|
     ^#SRC|file:2|$                                #SRC|file:2|
     ^          a: something$                                a: something
     ^       }$                                           }
     ^#KEY|p3|$                                    #KEY|p3|
     ^#SRC|new_file:14|$                           #SRC|new_file:14|
     ^       p3: {$                                       p3: {
     ^#KEY|p3.a|$                                  #KEY|p3.a|
     ^#SRC|new_file:15|$                           #SRC|new_file:15|
     ^          a: else$                                     a: else
     ^#KEY|p3.b|$                                  #KEY|p3.b|
     ^#SRC|file:7|$                                #SRC|file:7|
     ^          b: [$                                        b: [
     ^          ]$                                           ]
     ^#KEY|p3.c|$                 Rendered         #KEY|p3.c|
     ^#SRC|file:9|$               ========>        #SRC|file:9|
     ^          c: [$                                        c: [
     ^#KEY|p3.c[0]|$                               #KEY|p3.c[0]|
     ^#SRC|file:9|$                                #SRC|file:9|
     ^             11$                                          11
     ^          ]$                                           ]
     ^#KEY|p3.d|$                                  #KEY|p3.d|
     ^#SRC|file:14|$                               #SRC|file:14|
     ^          d: [$                                        d: [
     ^#KEY|p3.d[0]|$                               #KEY|p3.d[0]|
     ^#SRC|file:14|$                               #SRC|file:14|
     ^             11,$                                         11,
     ^#KEY|p3.d[1]|$                               #KEY|p3.d[1]|
     ^#SRC|file:28|$                               #SRC|file:28|
     ^             12$                                          12
     ^          ]$                                           ]
     ^#KEY|p4|$                                    #KEY|p3.p4|
     ^#SRC|new_file:16|$                           #SRC|new_file:16
     ^          p4: {$                                       p4: {
     ^#KEY|p4.d|$                                  #KEY|p3.p4.d|
     ^#SRC|new_file:17|$                           #SRC|new_file:17|
     ^            d: e$                                        d: e
     ^          }$                                           }
     ^       }$                                           }
     ^<-IND->


  Note that the caret ^ (dollar-sign $) represents the beginning (end)
  of the line and is not printed, nor is the <-IND-> string, which
  represents a user-provided width value for the initial indentation
  level.

  The '#KEY' tag includes the fully-qualified key of the parameter two
  lines below it.  The '#SRC|*|' tag specifies the annotation
  associated with the parameter one line below it.  The vertical bars
  are provided so that empty source annotations are still parsable by
  a user's script.

  Maintenance notes:
  ==================

  [1] A few stack objects are used: the Indentation class, as well as
      std::stack<std::size_t>, and std::vector<name_t>.

      The std::stack<std::size_t> object is used to keep track of the
      sizes of the stacked sequences.  Keeping track of the sequence
      is necessary so that the last sequence element does not have a
      ',' character that follows it.

      The std::vector<name_t> object is used to keep track of the
      stacked FHiCL names.  This series of names is stitched together
      to form the full key.  The reason this object is not of type
      std::stack is that we need to be able to iterate through each of
      the stack elements to create the full key, which is not doable
      with an std::stack object.

      To use these classes correctly, the Indentation must be updated
      during each {enter,exit}_{table,sequence} call, the
      sequence-sizes stack must be updated during each
      {enter,exit}_sequence call, and the name stack must be updated
      during each {enter,exit}_table call.

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

      In these cases, an empty annotation is provided in the '#SRC'
      tag.

*/

#include "fhiclcpp/ParameterSetWalker.h"
#include "fhiclcpp/coding.h"
#include "fhiclcpp/detail/Indentation.h"
#include "fhiclcpp/fwd.h"

#include <sstream>
#include <stack>
#include <string>
#include <vector>

namespace fhicl {
  namespace detail {

    class PrettifierPrefixAnnotated : public ParameterSetWalker {
    public:
      using name_t = std::string;

      PrettifierPrefixAnnotated();

      std::string
      result() const
      {
        return buffer_.str();
      }

    private:
      void before_action(key_t const&,
                         any_t const&,
                         ParameterSet const*) override;

      void enter_table(key_t const&, any_t const&) override;
      void enter_sequence(key_t const&, any_t const&) override;

      void exit_table(key_t const&, any_t const&) override;
      void exit_sequence(key_t const&, any_t const&) override;

      void atom(key_t const&, any_t const&) override;

      void push_size_(any_t const&);
      void pop_size_();
      std::string print_full_key_(name_t const& k) const;

      std::stringstream buffer_;
      Indentation indent_;
      std::string info_;
      std::stack<std::size_t> sequence_sizes_;
      std::size_t curr_size_;
      std::vector<name_t> name_stack_;
    };
  }
}

#endif /* fhiclcpp_detail_PrettifierPrefixAnnotated_h */

// Local variables:
// mode: c++
// End:
