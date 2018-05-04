#define BOOST_TEST_MODULE (equality test)

#include "cetlib/quiet_unit_test.hpp"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/parse.h"
#include <string>

using namespace fhicl;
using namespace std;

BOOST_AUTO_TEST_SUITE(document_test)

/*BOOST_AUTO_TEST_CASE( canonical_form )
{
  std::string doc1 = "x : .5";
  std::string doc2 = "x : 0.5";
  intermediate_table tbl1;
  intermediate_table tbl2;
  BOOST_CHECK_NO_THROW( parse_document(doc1, tbl1) );
  BOOST_CHECK_NO_THROW( parse_document(doc2, tbl2) );
  ParameterSet pset1;
  ParameterSet pset2;
  make_ParameterSet(tbl1, pset1);
  make_ParameterSet(tbl2, pset2);
  BOOST_CHECK_EQUAL( pset1.get<int>("x"), pset2.get<int>("x") );
  BOOST_CHECK_EQUAL( pset1, pset2 );
}*/

BOOST_AUTO_TEST_CASE(leading_zeros)
{
  std::string doc1 = "x : 5";
  std::string doc2 = "x : 005";
  intermediate_table tbl1;
  intermediate_table tbl2;
  BOOST_CHECK_NO_THROW(parse_document(doc1, tbl1));
  BOOST_CHECK_NO_THROW(parse_document(doc2, tbl2));
  ParameterSet pset1;
  ParameterSet pset2;
  make_ParameterSet(tbl1, pset1);
  make_ParameterSet(tbl2, pset2);
  BOOST_CHECK_EQUAL(pset1.get<int>("x"), pset2.get<int>("x"));
  BOOST_CHECK(pset1 == pset2);
}

BOOST_AUTO_TEST_SUITE_END()
