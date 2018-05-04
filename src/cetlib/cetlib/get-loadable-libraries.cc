#include "cetlib/LibraryManager.h"

#include <iostream>
#include <regex>
#include <string>
#include <vector>

namespace {
  std::string const regex_prefix{"([-A-Za-z0-9]*_)*"};
  std::regex const slash{"/"};

  std::string
  pattern(std::string const& spec)
  {
    std::string const canonSpec = std::regex_replace(spec, slash, "_");
    return regex_prefix + canonSpec + "_";
  }

  inline std::vector<std::string>
  getLibraries(cet::LibraryManager const& lm)
  {
    std::vector<std::string> result;
    lm.getLoadableLibraries(result);
    return result;
  }
}

int
main(int argc, char** argv)
{
  if (argc < 2 || argc > 3) {
    return 1;
  }
  std::string const suffix{argv[1]};
  std::string const spec{argc == 2 ? "" : argv[2]};
  cet::LibraryManager const lm{suffix, pattern(spec + ".*")};
  auto const& libs = getLibraries(lm);

  std::string str{};
  for (auto const& lib : libs) {
    auto const& libspecs = lm.getSpecsByPath(lib);
    if (libspecs.first.find(spec) == 0) {
      str += libspecs.first;
      str += '\n';
    }
    // Not all libraries have a long spec.
    if (!libspecs.second.empty() && libspecs.second.find(spec) == 0) {
      str += libspecs.second;
      str += '\n';
    }
  }
  std::cout << str;
}
