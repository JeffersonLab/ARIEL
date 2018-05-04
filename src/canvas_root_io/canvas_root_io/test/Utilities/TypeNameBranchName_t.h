#ifndef canvas_root_io_test_Utilities_TypeNameBranchName_t_h
#define canvas_root_io_test_Utilities_TypeNameBranchName_t_h

#include "cetlib/map_vector.h"

#include <map>
#include <string>
#include <vector>

namespace arttest {
  typedef std::string MyString;
  typedef std::vector<MyString> MyStrings;

  struct MyBigPOD {
    MyString s;
    MyStrings ss;
    std::map<std::string, std::string> ms;
    MyStrings mms;
  };
}

#endif /* canvas_root_io_test_Utilities_TypeNameBranchName_t_h */

// Local Variables:
// mode: c++
// End:
