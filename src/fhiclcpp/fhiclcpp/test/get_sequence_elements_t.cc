#define BOOST_TEST_MODULE (get sequence elements test)

#include "cetlib/quiet_unit_test.hpp"
#include "cetlib/test_macros.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/parse.h"

#include <iostream>
#include <string>
#include <vector>

using namespace fhicl;
using namespace std;

struct SampleConfigFixture {
  SampleConfigFixture();

  ParameterSet pset;
};

SampleConfigFixture::SampleConfigFixture()
{
  putenv(const_cast<char*>("FHICL_FILE_PATH=./test:."));
  cet::filepath_lookup policy("FHICL_FILE_PATH");
  intermediate_table tbl;
  std::string cfg_in("Sample.cfg");
  parse_document(cfg_in, policy, tbl);
  make_ParameterSet(tbl, pset);
}

BOOST_FIXTURE_TEST_SUITE(sampleConfig, SampleConfigFixture)

BOOST_AUTO_TEST_CASE(element_lookup)
{
  BOOST_CHECK_EQUAL(pset.get<int>("g[0]"), 1);
  BOOST_CHECK_EQUAL(pset.get<int>("h[0].h1"), 12);
  BOOST_CHECK_EQUAL(pset.get<int>("m[0]"), -1);
  BOOST_CHECK_EQUAL(pset.get<int>("vv[0][1]"), 2);

  BOOST_CHECK_EQUAL(pset.get<std::string>("h[1].h2"), "h2");
}

BOOST_AUTO_TEST_CASE(element_container_lookup)
{
  auto vec0 = pset.get<std::vector<int>>("vv[0]");
  auto ref0 = {1, 2, 3};
  CET_CHECK_EQUAL_COLLECTIONS(vec0, ref0);

  auto vec1 = pset.get<std::vector<int>>("vv[1]");
  auto ref1 = {2, 3, 4};
  CET_CHECK_EQUAL_COLLECTIONS(vec1, ref1);
}

BOOST_AUTO_TEST_SUITE_END()
