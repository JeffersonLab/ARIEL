// ======================================================================
//
// test cet::search_path
//
// ======================================================================

#include "cetlib/search_path.h"
#include "cetlib_except/exception.h"
#include <cstdlib>
#include <string>

using cet::search_path;

void
ensure(int const which, bool const claim)
{
  if (!claim)
    std::exit(which);
}

using namespace std::string_literals;

int
main()
{
  {
    auto const path = "xyzzy"s;
    search_path const xyzzy{path};
    ensure(1, xyzzy.size() == 1);
    ensure(2, xyzzy.showenv() == path);
  }

  {
    search_path const xyzzy{":xyzzy"};
    ensure(3, xyzzy.size() == 1);
    ensure(4, xyzzy.showenv().empty());
  }

  ensure(5, search_path{":xyzzy:"}.size() == 1);
  ensure(6, search_path{"xyzzy:plugh"}.size() == 2);
  ensure(7, search_path{"xyzzy:::plugh"}.size() == 2);
  ensure(8, search_path{"xyzzy:plugh:twisty:grue"}.size() == 4);

  ensure(11, !search_path{""}.empty());
  ensure(12, !search_path{":"}.empty());
  ensure(13, !search_path{"::"}.empty());

  ensure(14, search_path{""}.size() == 1);
  ensure(15, search_path{":"}.size() == 1);
  ensure(16, search_path{"::"}.size() == 1);

  {
    try {
      search_path const sp{":/tmp:"};
      sp.find_file("");
      ensure(21, false);
    }
    catch (cet::exception const& e) {
      ensure(22, e.category() == "search_path"s);
    }
  }

  ensure(23, search_path{"a:bb:c.c"}.to_string() == "a:bb:c.c"s);
}
