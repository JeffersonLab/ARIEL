#ifndef fhiclcpp_detail_printing_helpers_h
#define fhiclcpp_detail_printing_helpers_h

// ===================================================
//
// Various printing helpers
//
// These helpers are used by the 'Prettifier' classes.
//
// ===================================================

#include "fhiclcpp/detail/print_mode.h"

#include <any>
#include <string>

namespace fhicl {
  namespace detail {

    inline std::string
    nl(std::size_t i = 1)
    {
      return std::string(i, '\n');
    }

    std::string printed_suffix(std::string const& key, std::size_t const sz);

    std::string print_prefix_annotated_info(std::string const& curr_info);

    std::string print_annotated_info(std::string const& curr_info,
                                     std::string const& cached_info);

    namespace table {
      inline std::string
      opening_brace()
      {
        return "{";
      }
      inline std::string
      closing_brace()
      {
        return "}";
      }
      std::string printed_prefix(std::string const& key);
    }

    namespace sequence {
      inline std::string
      opening_brace()
      {
        return "[";
      }
      inline std::string
      closing_brace()
      {
        return "]";
      }
      std::string printed_prefix(std::string const& key);
    }

    namespace atom {
      std::string printed_prefix(std::string const& key);
      std::string value(std::any const&);
    }

    inline bool
    is_sequence_element(std::string const& key)
    {
      auto pos = key.find_last_of(sequence::closing_brace());
      return pos != std::string::npos && pos == key.size() - 1;
    }

    inline bool
    is_table_member(std::string const& key)
    {
      auto pos1 = key.find_last_of(".");
      if (pos1 == std::string::npos)
        return false;

      return is_sequence_element(key) ? false : true;
    }

    std::size_t index_for_sequence_element(std::string const& name);
  }
}

#endif /* fhiclcpp_detail_printing_helpers_h */

// Local variables:
// mode: c++
// End:
