#ifndef fhiclcpp_detail_ParameterSetImplHelpers_h
#define fhiclcpp_detail_ParameterSetImplHelpers_h

#include "fhiclcpp/coding.h"

#include <algorithm>
#include <string>
#include <vector>

namespace fhicl {
  namespace detail {

    //===============================================================
    // get_names

    class Keys {
    public:
      Keys(std::vector<std::string> const& keys, std::string const& last)
        : tables_{keys}, last_{last}
      {}

      auto const&
      tables() const
      {
        return tables_;
      }
      auto const&
      last() const
      {
        return last_;
      }

    private:
      std::vector<std::string> tables_;
      std::string last_;
    };

    Keys get_names(std::string const& key);

    //===============================================================
    // get_sequence_indices

    class SequenceKey {
    public:
      SequenceKey(std::string const& name,
                  std::vector<std::size_t> const& indices)
        : name_{name}, indices_{indices}
      {}

      auto const&
      name() const
      {
        return name_;
      }
      auto const&
      indices() const
      {
        return indices_;
      }

    private:
      std::string name_;
      std::vector<std::size_t> indices_;
    };

    SequenceKey get_sequence_indices(std::string const& key);

    //===============================================================
    // find_an_any

    using cit_size_t = std::vector<std::size_t>::const_iterator;

    inline bool
    find_an_any(cit_size_t it, cit_size_t const cend, std::any& a)
    {
      if (it == cend) {
        // If we got this far, that means the element must exist,
        // otherwise the previous recursive 'find_parameter' call would
        // have returned false.
        return true;
      }

      auto const seq = std::any_cast<ps_sequence_t>(a);

      if (*it >= seq.size())
        return false;

      a = seq[*it];

      return find_an_any(++it, cend, a);
    }
  }
}

#endif /* fhiclcpp_detail_ParameterSetImplHelpers_h */

// Local variables:
// mode: c++
// End:
