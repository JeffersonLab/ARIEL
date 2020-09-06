// ======================================================================
//
// test ParameterSet values
//
// ======================================================================

#define BOOST_TEST_MODULE (values test)

#include "cetlib/quiet_unit_test.hpp"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include <complex>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace fhicl;
using namespace std;

typedef unsigned int uint;

typedef double dbl;
typedef long double ldbl;

typedef complex<dbl> cdbl;
typedef complex<ldbl> cldbl;

typedef vector<uint> uvec;

BOOST_AUTO_TEST_SUITE(values_test)

BOOST_AUTO_TEST_CASE(bool_values) // test atoms "true" and "false"
{
  {
    ParameterSet pset;
    pset.put<string>("b11", "true");
    BOOST_TEST(pset.is_key_to_atom("b11"));
    BOOST_TEST(!pset.is_key_to_sequence("b11"));
    BOOST_TEST(!pset.is_key_to_table("b11"));
    BOOST_TEST(pset.to_string() == "b11:\"true\"");
    BOOST_TEST(pset.get<bool>("b11"));
    pset.put<string>("b13", "\"false\"");
    BOOST_TEST(pset.to_string() == "b11:\"true\" b13:\"false\"");
    BOOST_TEST(!pset.get<bool>("b13"));
  }

  {
    ParameterSet pset;
    pset.put<bool>("b21", true);
    BOOST_TEST(pset.get<string>("b21") == "true");
    BOOST_TEST(pset.to_string() == "b21:true");
    BOOST_TEST(pset.get<bool>("b21"));
    pset.put<bool>("b24", false);
    BOOST_TEST(pset.get<string>("b24") == "false");
    BOOST_TEST(pset.to_string() == "b21:true b24:false");
    BOOST_TEST(!pset.get<bool>("b24"));
  }

  {
    ParameterSet pset;
    pset.put<string>("b31", "tru");
    BOOST_TEST(pset.to_string() == "b31:\"tru\"");
    BOOST_CHECK_THROW(pset.get<bool>("b31"), fhicl::exception);
  }
}

BOOST_AUTO_TEST_CASE(nil_value) // test atom "nil"
{
  typedef void* nil_t;
  nil_t nil_value = 0;

  ParameterSet pset;
  BOOST_TEST(pset.is_empty());
  BOOST_TEST(pset.to_string() == "");

  pset.put<nil_t>("n11", 0);
  BOOST_TEST(pset.get<nil_t>("n11") == nil_value);
  BOOST_TEST(pset.to_string() == "n11:@nil");
  BOOST_CHECK_THROW(pset.get<bool>("n11"), fhicl::exception);
  BOOST_CHECK_THROW(pset.get<uint>("n11"), fhicl::exception);
  BOOST_CHECK_THROW(pset.get<int>("n11"), fhicl::exception);
  BOOST_CHECK_THROW(pset.get<double>("n11"), fhicl::exception);
  BOOST_CHECK_THROW(pset.get<string>("n11"), fhicl::exception);
  BOOST_CHECK_THROW(pset.get<uvec>("n11"), fhicl::exception);
  BOOST_CHECK_THROW(pset.get<ParameterSet>("n11"), fhicl::exception);

  pset.put<string>("n21", "nil");
  BOOST_CHECK_THROW(pset.get<nil_t>("n21"), fhicl::exception);
  BOOST_TEST(pset.to_string() == "n11:@nil n21:\"nil\"");

  pset.put<string>("n31", "NIL");
  BOOST_TEST(pset.to_string() == "n11:@nil n21:\"nil\" n31:\"NIL\"");
  BOOST_CHECK_THROW(pset.get<nil_t>("n31"), fhicl::exception);

  ParameterSet pset2;
  make_ParameterSet(pset.to_string(), pset2);
  BOOST_TEST(pset.to_string() == pset2.to_string());

  ParameterSet pset3;
  make_ParameterSet("n11:@nil n21:nil n31:\"@nil\"", pset3);
  BOOST_TEST(pset3.to_string() == "n11:@nil n21:\"nil\" n31:\"@nil\"");
  BOOST_CHECK_THROW(pset3.get<string>("n11"), fhicl::exception);
  BOOST_TEST(pset3.get<string>("n21") == "nil");
  BOOST_TEST(pset3.get<string>("n31") == "@nil");
}

BOOST_AUTO_TEST_CASE(string_values) // test string atoms
{
  ParameterSet pset;
  BOOST_TEST(pset.is_empty());

  pset.put<string>("n11", "");
  BOOST_TEST(pset.get<string>("n11") == "");
  BOOST_TEST(pset.to_string() == "n11:\"\"");
  pset.put<string>("n13", "a");
  BOOST_TEST(pset.get<string>("n13") == "a");
  BOOST_TEST(pset.to_string() == "n11:\"\" n13:\"a\"");
  pset.put<string>("n15", "\"a\"");
  BOOST_TEST(pset.get<string>("n15") == "a");
  BOOST_TEST(pset.to_string() == "n11:\"\" n13:\"a\" n15:\"a\"");
  pset.put<string>("n17", "'a b'");
  BOOST_TEST(pset.get<string>("n17") == "a b");
  BOOST_TEST(pset.to_string() == "n11:\"\" n13:\"a\" n15:\"a\" n17:\"a b\"");
}

BOOST_AUTO_TEST_CASE(unsigned_values) // test unsigned integral atoms
{
  {
    ParameterSet pset;
    pset.put<uint>("u11", 0u);
    BOOST_TEST(pset.get<string>("u11") == "0");
    BOOST_TEST(pset.to_string() == "u11:0");
    BOOST_TEST(pset.get<uint>("u11") == 0u);
  }

  {
    ParameterSet pset;
    pset.put<string>("u21", "000");
    BOOST_TEST(pset.to_string() == "u21:\"000\"");
    uint u21 = pset.get<uint>("u21");
    BOOST_TEST(u21 == 0u);
    pset.put<uint>("u23", u21);
    BOOST_TEST(pset.get<string>("u23") == "0");
    BOOST_TEST(pset.to_string() == "u21:\"000\" u23:0");
  }

  {
    ParameterSet pset;
    pset.put<string>("u31", "1.2e+1");
    BOOST_TEST(pset.to_string() == "u31:\"1.2e+1\"");
    BOOST_TEST(pset.get<uint>("u31") == 12u);
  }

  {
    ParameterSet pset;
    pset.put<unsigned long>("u41", 123456ul);
    BOOST_TEST(pset.get<string>("u41") == "123456");
    BOOST_TEST(pset.to_string() == "u41:123456");
    BOOST_TEST(pset.get<unsigned long>("u41") == 123456ul);
    pset.put<unsigned long>("u44", 1234567ul);
    BOOST_TEST(pset.get<string>("u44") == "1.234567e+6");
    BOOST_TEST(pset.to_string() == "u41:123456 u44:1.234567e+6");
    BOOST_TEST(pset.get<unsigned long>("u44") == 1234567ul);
  }
}

BOOST_AUTO_TEST_CASE(int_values) // test signed integral atoms
{
  {
    ParameterSet pset;
    pset.put<int>("i1", 0);
    BOOST_TEST(pset.get<string>("i1") == "0");
    BOOST_TEST(pset.to_string() == "i1:0");
    BOOST_TEST(pset.get<int>("i1") == 0);
  }

  {
    ParameterSet pset;
    pset.put<string>("i21", "000");
    BOOST_TEST(pset.to_string() == "i21:\"000\"");
    int i21 = pset.get<int>("i21");
    BOOST_TEST(i21 == 0);
    pset.put<int>("i23", i21);
    BOOST_TEST(pset.get<string>("i23") == "0");
    BOOST_TEST(pset.to_string() == "i21:\"000\" i23:0");
  }

  {
    ParameterSet pset;
    pset.put<string>("i31", "-1.2e+1");
    BOOST_TEST(pset.to_string() == "i31:\"-1.2e+1\"");
    BOOST_TEST(pset.get<int>("i31") == -12);
  }

  {
    ParameterSet pset;
    pset.put<long>("i41", 123456);
    BOOST_TEST(pset.get<string>("i41") == "123456");
    BOOST_TEST(pset.to_string() == "i41:123456");
    BOOST_TEST(pset.get<long>("i41") == 123456);
    pset.put<long>("i44", 1234567);
    BOOST_TEST(pset.get<string>("i44") == "1.234567e+6");
    BOOST_TEST(pset.to_string() == "i41:123456 i44:1.234567e+6");
    BOOST_TEST(pset.get<long>("i44") == 1234567);
  }

  {
    ParameterSet pset;
    pset.put<long>("i51", -123456);
    BOOST_TEST(pset.get<string>("i51") == "-123456");
    BOOST_TEST(pset.to_string() == "i51:-123456");
    BOOST_TEST(pset.get<long>("i51") == -123456);
    pset.put<long>("i54", -1234567);
    BOOST_TEST(pset.get<string>("i54") == "-1.234567e+6");
    BOOST_TEST(pset.to_string() == "i51:-123456 i54:-1.234567e+6");
    BOOST_TEST(pset.get<long>("i54") == -1234567);
  }
}

BOOST_AUTO_TEST_CASE(float_values) // test floating-point atoms
{
  {
    ParameterSet pset;
    pset.put<dbl>("f11", 0.0);
    BOOST_TEST(pset.get<string>("f11") == "0");
    BOOST_TEST(pset.to_string() == "f11:0");
    BOOST_TEST(pset.get<dbl>("f11") == 0.0);
    BOOST_TEST(pset.get<int>("f11") == 0);
    BOOST_TEST(pset.get<uint>("f11") == 0u);
    pset.put<dbl>("f14", 12.0);
    BOOST_TEST(pset.get<string>("f14") == "12");
    BOOST_TEST(pset.to_string() == "f11:0 f14:12");
    BOOST_TEST(pset.get<dbl>("f14") == 12.0);
    BOOST_TEST(pset.get<int>("f14") == 12);
    BOOST_TEST(pset.get<uint>("f14") == 12u);
    pset.put<dbl>("f17", 12.3e1);
    BOOST_TEST(pset.get<string>("f17") == "123");
    BOOST_TEST(pset.to_string() == "f11:0 f14:12 f17:123");
    BOOST_TEST(pset.get<dbl>("f17") == 123.0);
    BOOST_TEST(pset.get<int>("f17") == 123);
    BOOST_TEST(pset.get<uint>("f17") == 123u);
  }

  {
    ParameterSet pset;
    pset.put<dbl>("f21", 12.3456e4);
    BOOST_TEST(pset.get<string>("f21") == "123456");
    BOOST_TEST(pset.to_string() == "f21:123456");
    BOOST_TEST(pset.get<dbl>("f21") == 123456.0);
    BOOST_TEST(pset.get<int>("f21") == 123456);
    BOOST_TEST(pset.get<uint>("f21") == 123456u);
    pset.put<dbl>("f24", 12.34567e5);
    BOOST_TEST(pset.get<string>("f24") == "1.234567e+6");
    BOOST_TEST(pset.to_string() == "f21:123456 f24:1.234567e+6");
    BOOST_TEST(pset.get<dbl>("f24") == 1234567.0);
    BOOST_TEST(pset.get<int>("f24") == 1234567L);
    BOOST_TEST(pset.get<uint>("f24") == 1234567uL);
    pset.put<dbl>("f27", 3.5);
    BOOST_TEST(pset.get<string>("f27") == "3.5");
    BOOST_TEST(pset.to_string() == "f21:123456 f24:1.234567e+6 f27:3.5");
    BOOST_TEST(pset.get<dbl>("f27") == 3.5);
    BOOST_CHECK_THROW(pset.get<int>("f27"), fhicl::exception);
    BOOST_CHECK_THROW(pset.get<uint>("f27"), fhicl::exception);
  }

  {
    ParameterSet pset;
    pset.put<dbl>("f31", -12.3456e4);
    BOOST_TEST(pset.get<string>("f31") == "-123456");
    BOOST_TEST(pset.to_string() == "f31:-123456");
    BOOST_TEST(pset.get<dbl>("f31") == -123456.0);
    pset.put<dbl>("f34", -12.34567e5);
    BOOST_TEST(pset.get<string>("f34") == "-1.234567e+6");
    BOOST_TEST(pset.to_string() == "f31:-123456 f34:-1.234567e+6");
    BOOST_TEST(pset.get<dbl>("f34") == -1234567.0);
    pset.put<dbl>("f37", -12.34567e-5);
  }

  {
    ParameterSet pset;
    pset.put<dbl>("f41", numeric_limits<dbl>::infinity());
    BOOST_TEST(pset.get<string>("f41") == "+infinity");
    BOOST_TEST(pset.to_string() == "f41:+infinity");
    BOOST_TEST(pset.get<dbl>("f41") == numeric_limits<dbl>::infinity());
    pset.put<dbl>("f44", -numeric_limits<dbl>::infinity());
    BOOST_TEST(pset.get<string>("f44") == "-infinity");
    BOOST_TEST(pset.to_string() == "f41:+infinity f44:-infinity");
    BOOST_TEST(pset.get<dbl>("f44") == -numeric_limits<dbl>::infinity());
  }
}

BOOST_AUTO_TEST_CASE(complex_values) // test complex atoms
{
  {
    ParameterSet pset;
    pset.put<cldbl>("f11", cldbl());
    BOOST_TEST(pset.get<string>("f11") == "(0,0)");
    BOOST_TEST(pset.to_string() == "f11:(0,0)");
    BOOST_TEST(pset.get<cldbl>("f11") == cldbl(0.0L, 0.0L));
    pset.put<cdbl>("f14", cdbl());
    BOOST_TEST(pset.get<string>("f14") == "(0,0)");
    BOOST_TEST(pset.to_string() == "f11:(0,0) f14:(0,0)");
    BOOST_TEST(pset.get<cdbl>("f14") == cdbl(0.0, 0.0));
  }

  {
    ParameterSet pset;
    pset.put<cldbl>("f21", cldbl(1, 2));
    BOOST_TEST(pset.to_string() == "f21:(1,2)");
    BOOST_TEST(pset.get<cldbl>("f21") == cldbl(1.0L, 2.0L));
    BOOST_TEST(pset.get<cdbl>("f21") == cdbl(1.0, 2.0));
    pset.put<string>("f24", "(3.25 , 4.75 )");
    BOOST_TEST(pset.get<cldbl>("f24") == cldbl(3.25L, 4.75L));
    BOOST_TEST(pset.get<cdbl>("f24") == cdbl(3.25, 4.75));
  }
}

BOOST_AUTO_TEST_CASE(sequence_values) // test sequences
{
  ParameterSet pset;
  uvec uv;

  pset.put<string>("f11", "[]");
  BOOST_TEST(pset.get<string>("f11") == "[]");
  BOOST_TEST(pset.to_string() == "f11:\"[]\"");
  BOOST_TEST(pset.get<uvec>("f11").empty());
  BOOST_TEST(pset.get<uvec>("f11") == uvec());
  BOOST_TEST(pset.get<uvec>("f11") == uv);
  pset.put<uvec>("f16", uv);
  // BOOST_TEST( pset.get<string>("f16", "NO") ==  "NO" );
  BOOST_TEST(pset.to_string() == "f11:\"[]\" f16:[]");
  BOOST_TEST(pset.get<uvec>("f16") == uv);

  uv.push_back(1);
  pset.put<string>("f21", "[1]");
  BOOST_TEST(pset.get<string>("f21") == "[1]");
  BOOST_TEST(pset.to_string() == "f11:\"[]\" f16:[] f21:\"[1]\"");
  BOOST_TEST(pset.get<uvec>("f21").size() == 1u);
  BOOST_TEST(pset.get<uvec>("f21") == uv);
  pset.put<uvec>("f25", uv);
  // BOOST_TEST( pset.get<string>("f25", "NO") ==  "NO" );
  BOOST_TEST(pset.to_string() == "f11:\"[]\" f16:[] f21:\"[1]\" f25:[1]");
  BOOST_TEST(pset.get<uvec>("f25").size() == 1u);
  BOOST_TEST(pset.get<uvec>("f25") == uv);

  uv.push_back(2);
  pset.put<string>("f31", "[1,2]");
  BOOST_TEST(pset.get<string>("f31") == "[1,2]");
  BOOST_TEST(pset.get<uvec>("f31").size() == 2u);
  BOOST_TEST(pset.get<uvec>("f31") == uv);
  uv.push_back(3);
  pset.put<uvec>("f34", uv);
  BOOST_TEST(pset.to_string() ==
             "f11:\"[]\" f16:[] f21:\"[1]\" f25:[1] f31:\"[1,2]\" f34:[1,2,3]");
  // BOOST_TEST( pset.get<string>("f34", "NO") ==  "NO" );
  BOOST_TEST(pset.get<uvec>("f34").size() == 3u);
  BOOST_TEST(pset.get<uvec>("f34") == uv);

  uv.push_back(4);
  pset.put<string>("f41", "[1,2,3,4]");
  BOOST_TEST(pset.get<string>("f41") == "[1,2,3,4]");
  BOOST_TEST(pset.get<uvec>("f41").size() == 4u);
  BOOST_TEST(pset.get<uvec>("f41") == uv);
  uv.push_back(5);
  pset.put<uvec>("f43", uv);
  BOOST_TEST(pset.is_key_to_sequence("f43"));
  BOOST_TEST(!pset.is_key_to_table("f43"));
  BOOST_TEST(!pset.is_key_to_atom("f43"));
  // BOOST_TEST( pset.get<string>("f43", "NO") ==  "NO" );
  BOOST_TEST(pset.get<uvec>("f43").size() == 5u);
  BOOST_TEST(pset.get<uvec>("f43") == uv);
}

BOOST_AUTO_TEST_CASE(table_values) // test tables
{
  ParameterSet p0;
  BOOST_TEST(p0.to_string() == "");
  BOOST_CHECK_THROW(p0.get<string>("a"), fhicl::exception);

  ParameterSet p1;
  p1.put<string>("b", "B");
  p1.put<string>("a", "A");
  BOOST_TEST(p1.get<string>("a") == "A");
  BOOST_TEST(p1.get<string>("b") == "B");
  BOOST_TEST(p1.to_string() == "a:\"A\" b:\"B\"");

  ParameterSet p2;
  p2.put<string>("y", "Y");
  p2.put<string>("z", "Z");
  p2.put<string>("x", "X");
  BOOST_TEST(p2.get<string>("x") == "X");
  BOOST_TEST(p2.get<string>("y") == "Y");
  BOOST_TEST(p2.get<string>("z") == "Z");
  BOOST_TEST(p2.to_string() == "x:\"X\" y:\"Y\" z:\"Z\"");

  ParameterSet p3;
  p3.put<ParameterSet>("p2", p2);
  p3.put<ParameterSet>("p1", p1);
  BOOST_TEST(p3.is_key_to_table("p2"));
  BOOST_TEST(!p3.is_key_to_sequence("p2"));
  BOOST_TEST(!p3.is_key_to_atom("p2"));
  BOOST_TEST(p3.get<ParameterSet>("p1").get<string>("a") == "A");
  BOOST_TEST(p3.get<ParameterSet>("p1").get<string>("b") == "B");
  BOOST_TEST(p3.get<ParameterSet>("p2").get<string>("x") == "X");
  BOOST_TEST(p3.get<ParameterSet>("p2").get<string>("y") == "Y");
  BOOST_TEST(p3.get<ParameterSet>("p2").get<string>("z") == "Z");
  BOOST_TEST(p3.to_string() ==
             "p1:{a:\"A\" b:\"B\"} p2:{x:\"X\" y:\"Y\" z:\"Z\"}");
}

BOOST_AUTO_TEST_SUITE_END()
