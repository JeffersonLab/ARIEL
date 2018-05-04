#ifndef fhiclcpp_types_Name_h
#define fhiclcpp_types_Name_h

#include <regex>
#include <string>

namespace fhicl {

  std::string const get_regex_replace_str(std::string const& str);

  struct Name {

    explicit Name(std::string const& name = "") : value(name) {}
    std::string value;

    static Name
    sequence_element(std::size_t const i)
    {
      return Name{index(i)};
    }

    static Name
    sequence_element(std::string const& key_stem, std::size_t const i)
    {
      return Name{key_stem + index(i)};
    }

    // Sometimes the key has '[' and ']' literals in it.  Those must be
    // padded with an escape character so the regex library doesn't
    // think we're trying to specify a character set.
    static std::string
    regex_safe(std::string const& key)
    {
      return fhicl::get_regex_replace_str(key);
    }

  private:
    static std::string
    index(std::size_t const i)
    {
      return "[" + std::to_string(i) + "]";
    }
  };
}

#endif /* fhiclcpp_types_Name_h */

// Local variables:
// mode: c++
// End:
