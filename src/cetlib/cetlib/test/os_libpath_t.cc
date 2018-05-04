#include "cetlib/os_libpath.h"

#include <assert.h>
#include <iostream>
#include <string.h>

#include <sys/utsname.h>

int
main()
{
  utsname name;
  if (uname(&name) != 0) {
    std::cerr << "uname call failed\n";
    return 1;
  }
  std::string osname(name.sysname);
  std::string expected;
  if (osname == "Darwin") {
    expected = "DYLD_LIBRARY_PATH";
  } else if (osname == "Linux") {
    expected = "LD_LIBRARY_PATH";
  } else {
    std::cerr << "Unrecognized operating system name from uname\n";
    return 1;
  }

  char const* lpath = cet::os_libpath();
  if (expected != lpath) {
    std::cerr << "expected: " << expected << '\n'
              << "result:   " << lpath << '\n';
    return 2;
  }
}
