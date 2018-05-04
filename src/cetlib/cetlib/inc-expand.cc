// ======================================================================
//
// inc-expand: read/write specified text, replacing each #include
//             directive with the corresponding file's contents
//
// ======================================================================

#include "cetlib/filepath_maker.h"
#include "cetlib/includer.h"
#include "cetlib_except/exception.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>

cet::filepath_maker identity_filepath;

// ----------------------------------------------------------------------

int
do_including(std::string const& from, std::ostream& to) try {
  cet::includer inc(from, identity_filepath);

  cet::includer::const_iterator it = inc.begin();
  cet::includer::const_iterator end = inc.end();

  std::ostream_iterator<char> out(to);
  std::copy(it, end, out);

  return 0;
}
catch (cet::exception const& e) {
  return 1;
} // do_including()

// ----------------------------------------------------------------------

int
main(int argc, char* argv[])
{
  int nfailures = 0;

  for (int k = 1; k != argc; ++k)
    nfailures += do_including(argv[k], std::cout);

  return nfailures;

} // main()
