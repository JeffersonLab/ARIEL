#define BOOST_TEST_MODULE (keymap test)

#include "cetlib/quiet_unit_test.hpp"
#include "cetlib/test_macros.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/test/types/CondConfig.h"
#include "fhiclcpp/types/Table.h"

#include <iostream>
#include <string>

namespace {

  using namespace fhicl;

  fhicl::ParameterSet
  create_PSet(std::string const& file)
  {
    cet::filepath_maker fpm;
    ParameterSet pset;
    make_ParameterSet(file, fpm, pset);
    return pset;
  }
}

BOOST_AUTO_TEST_SUITE(types_keymap_test)

BOOST_AUTO_TEST_CASE(cond_01_t)
{
  std::string const file{"cond_01_t.fcl"};
  Table<CondConfig> table{Name("pset")};
  try {
    table.validate_ParameterSet(
      create_PSet(file).get<fhicl::ParameterSet>("pset"));
  }
  catch (fhicl::detail::validationException const& e) {
    std::cout << e.what() << std::endl;
  }
}

BOOST_AUTO_TEST_CASE(cond_02_t)
{
  std::string const file{"cond_02_t.fcl"};
  Table<CondConfig> table{Name("pset")};
  table.validate_ParameterSet(
    create_PSet(file).get<fhicl::ParameterSet>("pset"));
  BOOST_CHECK_EQUAL(table().flag(), false);
  BOOST_CHECK_EQUAL(table().num2(), 4);
  BOOST_CHECK_EQUAL(table().shape(), "sphere");
  BOOST_CHECK_EQUAL(table().sphereParams().radius(), 7);
}

BOOST_AUTO_TEST_CASE(cond_03_t)
{
  std::string const file{"cond_03_t.fcl"};
  Table<CondConfig> table{Name("pset")};
  table.validate_ParameterSet(
    create_PSet(file).get<fhicl::ParameterSet>("pset"));
  BOOST_CHECK_EQUAL(table().flag(), true);
  int number{16};
  BOOST_CHECK_EQUAL(table().num1(number), false);
  BOOST_CHECK_EQUAL(table().shape(), "box");
  auto ref = {4, 9, 2};
  auto hls = table().boxParams().halfLengths();
  CET_CHECK_EQUAL_COLLECTIONS(hls(), ref);
  BOOST_CHECK_EQUAL(table().boxParams().material(), "Wood");
  std::string name;
  BOOST_CHECK_EQUAL(table().boxParams().boxName(name), true);
  BOOST_CHECK_EQUAL(name, "Stan");
}

BOOST_AUTO_TEST_SUITE_END()
