#include "cetlib/include.h"
#include <cstdlib>
#include <fstream>
#include <regex>

using cet::include;

namespace {

  void
  ensure(int which, bool claim)
  {
    if (not claim)
      std::exit(which);
  }

  inline void
  ensure(int which, std::istream& os)
  {
    // Force boolean context.
    ensure(which, !!os);
  }

  std::string const file_a = "./a.txt";
  std::string const file_b = "./b.txt";
  std::string const file_c = "./c.txt";
  std::string const file_i = "./i.txt";
  std::string const file_j = "./j.txt";
  std::string const file_k = "./k.txt";

  std::string const contents_a = "abcde\n"
                                 "vwxyz\n";

  std::string const contents_b = "12345\n"
                                 "67890\n";

  std::string const contents_c = "a1b2c3\r"
                                 "d4e5f6\r";

  std::string const contents_i = "begin\n"
                                 "#include \"./a.txt\"\n"
                                 "end\n";

  std::string const contents_j = "begin\n"
                                 "#include \"./a.txt\"\n"
                                 "#include \"./b.txt\"\r\n"
                                 "end\n";

  std::string const contents_k = "begin\n"
                                 "#include \"./a.txt\"\n"
                                 "#include \"./b.txt\"\r\n"
                                 "#include \"./c.txt\"\r"
                                 "end\n";

  inline std::string
  expected_string(std::string const& str)
  {
    return std::regex_replace(str, std::regex("\r"), "\n");
  }

  void
  write_files()
  {
    std::ofstream a(file_a);
    a << contents_a;
    std::ofstream b(file_b);
    b << contents_b;
    std::ofstream c(file_c);
    c << contents_c;
    std::ofstream i(file_i);
    i << contents_i;
    std::ofstream j(file_j);
    j << contents_j;
    std::ofstream k(file_k);
    k << contents_k;
  }
}

int
main()
{
  write_files();

  {
    std::string result;

    std::ifstream a(file_a);
    ensure(1, a);
    result.clear();
    include(a, result);
    ensure(2, result == expected_string(contents_a));

    std::ifstream b(file_b);
    ensure(3, b);
    result.clear();
    include(b, result);
    ensure(4, result == expected_string(contents_b));

    std::ifstream c(file_c);
    ensure(5, c);
    result.clear();
    include(c, result);
    ensure(6, result == expected_string(contents_c));
  }

  {
    std::string result;
    std::ifstream i(file_i);
    ensure(11, i);
    result.clear();
    include(i, result);
    ensure(12,
           result == expected_string("begin\n") + expected_string(contents_a) +
                       expected_string("end\n"));
  }

  {
    std::string result;
    std::ifstream j(file_j);
    ensure(21, j);
    result.clear();
    include(j, result);
    ensure(22,
           result == expected_string("begin\n") + expected_string(contents_a) +
                       expected_string(contents_b) + expected_string("end\n"));
  }

  {
    std::string result;
    std::ifstream k(file_k);
    ensure(31, k);
    result.clear();
    include(k, result);
    ensure(32,
           result == expected_string("begin\n") + expected_string(contents_a) +
                       expected_string(contents_b) +
                       expected_string(contents_c) + expected_string("end\n"));
  }

} // main()
