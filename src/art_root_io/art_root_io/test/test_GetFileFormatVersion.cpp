#include "art_root_io/GetFileFormatVersion.h"
#include <cassert>
#include <iostream>

int
main(int /*argc*/, char* argv[])
{
  assert(art::getFileFormatVersion() == 13);
  std::cout << "Running " << argv[0] << std::endl;
}
