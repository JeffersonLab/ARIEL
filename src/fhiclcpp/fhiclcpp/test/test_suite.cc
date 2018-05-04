#define BOOST_TEST_MODULE (document test)

#include "cetlib/quiet_unit_test.hpp"

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/parse.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace fhicl;
using namespace std;

void
readFile(string fileName, string& content)
{
  ifstream in(fileName.c_str());
  for (string line; getline(in, line);)
    content.append(line).append("\n");
}

BOOST_AUTO_TEST_SUITE(document_test)

BOOST_AUTO_TEST_CASE(doc)
{
  ParameterSet ps1;
  {
    intermediate_table table;
    cet::filepath_lookup_nonabsolute policy("FHICL_FILE_PATH");
    parse_document(
      boost::unit_test::framework::master_test_suite().argv[1], policy, table);
    make_ParameterSet(table, ps1);
  }
  cout << "==========  Parsed parameter set ==========\n"
       << ps1.to_indented_string()
       << "==========  End of parameter set ==========" << endl;

  ParameterSet ps2;
  {
    ostringstream os;
    os << ps1.to_string() << endl;
    intermediate_table table;
    parse_document(os.str(), table);
    make_ParameterSet(table, ps2);
  }

  BOOST_CHECK(ps1 == ps2);

  // Alternative representation.
  ParameterSet ps3;
  {
    ostringstream os;
    os << ps1.to_compact_string() << endl;
    intermediate_table table;
    parse_document(os.str(), table);
    make_ParameterSet(table, ps3);
  }

  BOOST_CHECK(ps1 == ps3);
}

BOOST_AUTO_TEST_SUITE_END()
