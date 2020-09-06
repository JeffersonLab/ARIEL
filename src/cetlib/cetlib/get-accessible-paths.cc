#include "boost/filesystem.hpp"
#include "cetlib/getenv.h"
#include "cetlib/split.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std::string_literals;
namespace bfs = boost::filesystem;

namespace {

  void
  print_paths(std::string const& path_name,
              std::string pattern_to_match,
              bool const match_full_path)
  {
    bfs::path path{path_name};
    if (exists(path) && is_directory(path)) {
      std::vector<std::string> v;
      for (auto const& x : bfs::directory_iterator(path)) {
        if (match_full_path) {
          auto const str = canonical(x.path()).string();
          if (str.find(pattern_to_match) != 0) {
            continue;
          }
          std::cout << str << ' ';
        } else {
          auto const str = canonical(x.path()).filename().string();
          if (str.find(pattern_to_match) != 0) {
            continue;
          }
          std::cout << str << ' ';
        }
      }
    }
  }
}

int
main(int argc, char** argv)
{
  if (argc != 3) {
    return 1;
  }

  std::string const envname{argv[1]};
  std::string spec{argv[2]};
  std::string to_match{spec};
  auto const last_slash = to_match.find("/");
  bool const slash_found{last_slash != std::string::npos};
  std::string const prefix = slash_found ? to_match.substr(0, last_slash) : ""s;
  if (slash_found) {
    to_match.erase(0, last_slash + 1);
  }

  if (spec.empty() || spec[0] == '/') {
    print_paths("/"s + prefix, "/"s + to_match, true);
  }

  if (spec[0] != '/') {
    print_paths("./"s + prefix, to_match, false);
  }

  if (spec[0] != '/' && spec.find("./") != 0) {
    // Find paths on environment variable
    auto const env = cet::getenv(envname);
    std::vector<std::string> paths;
    cet::split(env, ':', back_inserter(paths));
    for (auto const& path : paths) {
      if (path == "."s)
        continue;
      print_paths(path + prefix, to_match, false);
    }
  }
}
