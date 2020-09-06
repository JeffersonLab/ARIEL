#ifndef cetlib_split_by_regex_h
#define cetlib_split_by_regex_h

// ======================================================================
//
// split_by_regex: Obtain substrings at a string's specified boundaries,
//                 defined according to a regex delimiter set.
//
//                 This is a high-powered method of tokenizing that uses
//                 the C++ regex library to split into substrings.
//
// Examples of splitting with ":" as the separator:
//    input     result
//     ""       [""]         # Different behavior than split.h
//     "a"      ["a"]
//     "a:"     ["a"]
//     ":boo"   ["","boo"]   # Different behavior than split.h
//     "a:b"    ["a","b"]
//     "a::b"   ["a","","b"] # Different behavior than split.h
//
// More complicated example splitting by "::" and "().":
//
//    split_by_regex("namespace::class::static_function().value",
//    "(::|\\(\\)\\.)")
//
// returns ["namespace","class","static_function","value"]
//
// ======================================================================

#include <regex>
#include <string>
#include <vector>

namespace cet {
  std::vector<std::string> split_by_regex(std::string const& str,
                                          std::regex const& re);

  // Forms regular expression given pattern via a string.
  std::vector<std::string> split_by_regex(std::string const& str,
                                          std::string const& delimSet);
}

#endif /* cetlib_split_by_regex_h */

// Local variables:
// mode: c++
// End:
