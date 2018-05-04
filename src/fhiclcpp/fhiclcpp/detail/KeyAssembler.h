#ifndef fhiclcpp_detail_KeyAssembler_h
#define fhiclcpp_detail_KeyAssembler_h

/*
  ======================================================================

  KeyAssembler

  ======================================================================

  Class used when

    'ParameterSet::get_all_keys()'

  is called.  This class provides an 'std::vector<std::string>' representing
  the entire (nested) list of keys corresponding to a ParameterSet object.

  For a FHiCL configuration of the type:

         p1: {}
         p3: {
            a: else
            b: []
            d: [ 11, 12 ]
            p4: { e: f }
            g: [ {h1: i1}, {h2: i2} ]
         }

  The following list of keys will be returned (in some order):

        p1
        p3
        p3.a
        p3.b
        p3.d
        p3.d[0]
        p3.d[1]
        p3.p4
        p3.p4.e
        p3.g
        p3.g[0]
        p3.g[0].h1
        p3.g[1]
        p3.g[1].h2

  Maintenance notes:
  ==================

  [1] The std::vector<name_t> object is used to keep track of the
      stacked FHiCL names.  This series of names is stitched together
      to form the full key.  The reason this object is not of type
      std::stack is that we need to be able to iterate through each of
      the stack elements to create the full key, which is not doable
      with an std::stack object.

      To use this class correctly, the name stack must be updated
      during each {enter,exit}_table call.

*/

#include "boost/any.hpp"
#include "fhiclcpp/ParameterSetWalker.h"
#include "fhiclcpp/coding.h"

#include <string>
#include <vector>

namespace fhicl {
  namespace detail {

    using key_t = std::string;
    using name_t = std::string;

    class KeyAssembler : public ParameterSetWalker {
    public:
      std::vector<key_t> const&
      result()
      {
        return keys_;
      }

    private:
      void enter_table(key_t const&, any_t const&) override;
      void exit_table(key_t const&, any_t const&) override;

      void enter_sequence(key_t const&, any_t const&) override;
      void atom(key_t const&, any_t const&) override;

      std::string full_key_(name_t const&) const;

      std::vector<key_t> keys_{};
      std::vector<name_t> name_stack_{};
    };
  }
}

#endif /* fhiclcpp_detail_KeyAssembler_h */

// Local variables:
// mode: c++
// End:
