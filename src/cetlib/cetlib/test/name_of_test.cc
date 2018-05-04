#define BOOST_TEST_MODULE (name_of test)
#include "cetlib/quiet_unit_test.hpp"

#include "cetlib/name_of.h"

using cet::name_of;

struct sometype;

BOOST_AUTO_TEST_SUITE(name_of_test)

BOOST_AUTO_TEST_CASE(unknown_type_test)
{
  BOOST_CHECK_EQUAL(name_of<sometype>::is(), "unknown-type");
  BOOST_CHECK_EQUAL(name_of<sometype*>::is(), "ptr-to_unknown-type");
  BOOST_CHECK_EQUAL(name_of<sometype&>::is(), "ref-to_unknown-type");
  BOOST_CHECK_EQUAL(name_of<sometype[1]>::is(), "array[1]-of_unknown-type");
}

BOOST_AUTO_TEST_CASE(fundamental_types_test)
{
  // signed ints:
  BOOST_CHECK_EQUAL(name_of<int>::is(), "int");
  BOOST_CHECK_EQUAL(name_of<long>::is(), "long");
  BOOST_CHECK_EQUAL(name_of<long long>::is(), "llong");
  BOOST_CHECK_EQUAL(name_of<short>::is(), "short");
  BOOST_CHECK_EQUAL(name_of<signed char>::is(), "schar");

  // unsigned ints:
  BOOST_CHECK_EQUAL(name_of<unsigned char>::is(), "uchar");
  BOOST_CHECK_EQUAL(name_of<unsigned int>::is(), "uint");
  BOOST_CHECK_EQUAL(name_of<unsigned long>::is(), "ulong");
  BOOST_CHECK_EQUAL(name_of<unsigned long long>::is(), "ullong");
  BOOST_CHECK_EQUAL(name_of<unsigned short>::is(), "ushort");

  // floating:
  BOOST_CHECK_EQUAL(name_of<double>::is(), "double");
  BOOST_CHECK_EQUAL(name_of<float>::is(), "float");
  BOOST_CHECK_EQUAL(name_of<long double>::is(), "ldouble");

  // other:
  BOOST_CHECK_EQUAL(name_of<bool>::is(), "bool");
  BOOST_CHECK_EQUAL(name_of<char>::is(), "char");
  BOOST_CHECK_EQUAL(name_of<void>::is(), "void");
}

BOOST_AUTO_TEST_CASE(cv_types_test)
{
  // const:
  BOOST_CHECK_EQUAL(name_of<int const>::is(), "c_int");
  BOOST_CHECK_EQUAL(name_of<long const>::is(), "c_long");
  BOOST_CHECK_EQUAL(name_of<long long const>::is(), "c_llong");
  BOOST_CHECK_EQUAL(name_of<short const>::is(), "c_short");
  BOOST_CHECK_EQUAL(name_of<signed char const>::is(), "c_schar");

  // volatile:
  BOOST_CHECK_EQUAL(name_of<unsigned char volatile>::is(), "v_uchar");
  BOOST_CHECK_EQUAL(name_of<unsigned int volatile>::is(), "v_uint");
  BOOST_CHECK_EQUAL(name_of<unsigned long volatile>::is(), "v_ulong");
  BOOST_CHECK_EQUAL(name_of<unsigned long long volatile>::is(), "v_ullong");
  BOOST_CHECK_EQUAL(name_of<unsigned short volatile>::is(), "v_ushort");

  // const-volatile:
  BOOST_CHECK_EQUAL(name_of<double const volatile>::is(), "c-v_double");
  BOOST_CHECK_EQUAL(name_of<float const volatile>::is(), "c-v_float");
  BOOST_CHECK_EQUAL(name_of<long double const volatile>::is(), "c-v_ldouble");
  BOOST_CHECK_EQUAL(name_of<bool const volatile>::is(), "c-v_bool");
  BOOST_CHECK_EQUAL(name_of<char const volatile>::is(), "c-v_char");
  BOOST_CHECK_EQUAL(name_of<void const volatile>::is(), "c-v_void");
}

BOOST_AUTO_TEST_CASE(array_types_test)
{
  BOOST_CHECK_EQUAL(name_of<sometype[1]>::is(), "array[1]-of_unknown-type");

  BOOST_CHECK_EQUAL(name_of<int[9]>::is(), "array[9]-of_int");
  // BOOST_CHECK_EQUAL( name_of<int const   [9]>::is(), "array[9]-of_c_int" );
  BOOST_CHECK_EQUAL(name_of<int * [9]>::is(), "array[9]-of_ptr-to_int");
  BOOST_CHECK_EQUAL(name_of<int const * [9]>::is(), "array[9]-of_ptr-to_c_int");
  // BOOST_CHECK_EQUAL( name_of<int * const [9]>::is(),
  // "array[9]-of_c_ptr-to_int" );
}

BOOST_AUTO_TEST_CASE(library_types_test)
{
  BOOST_CHECK_EQUAL(name_of<std::nullptr_t>::is(), "std::nullptr_t");
  BOOST_CHECK_EQUAL(name_of<std::string>::is(), "std::string");
  BOOST_CHECK_EQUAL(name_of<std::istream>::is(), "std::istream");
  BOOST_CHECK_EQUAL(name_of<std::ostream>::is(), "std::ostream");
}

BOOST_AUTO_TEST_SUITE_END()
