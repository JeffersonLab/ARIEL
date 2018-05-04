// ======================================================================
//
// test search_path
//
// ======================================================================

#include "cetlib/search_path.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using cet::search_path;

typedef std::vector<std::string> strings_t;

/*
  example regular expression = "^lib([A-Za-z0-9_]+)_plugin.so$"

  This program needs to be driven by a shell script to make
  management of files easier, such as creating and destroying
  temporary directories and files.
*/

int
main(int argc, char* argv[])
{
  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << " regex_pattern directory_name\n";
    return -1;
  }

  strings_t results;
  std::size_t count =
    search_path(argv[2]).find_files(argv[1], std::back_inserter(results));

  std::copy(results.begin(),
            results.end(),
            std::ostream_iterator<std::string>(std::cout, "\n"));

  return count;
}
