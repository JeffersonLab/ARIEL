// Test of the TypeTools functions.

#define BOOST_TEST_MODULE (TypeTools_t)
#include "cetlib/quiet_unit_test.hpp"

#include <iostream>
#include <string>
#include <typeinfo>
#include <vector>

#include "canvas/Persistency/Common/Wrapper.h"
#include "canvas_root_io/Utilities/TypeTools.h"
#include "canvas_root_io/Utilities/TypeWithDict.h"

using namespace std::string_literals;
using art::root::TypeWithDict;

BOOST_AUTO_TEST_SUITE(TypeTools_t)

BOOST_AUTO_TEST_CASE(no_dictionary_is_invalid)
{
  BOOST_REQUIRE(!TClass::GetClass("ThereIsNoTypeWithThisName"));
}

BOOST_AUTO_TEST_CASE(find_nested)
{
  TClass* intvec{TClass::GetClass("std::vector<int>")};
  BOOST_REQUIRE(intvec);

  BOOST_REQUIRE(
    !art::root::find_nested_type_named("WankelRotaryEngine", intvec));
  BOOST_REQUIRE(art::root::find_nested_type_named("const_iterator", intvec));
}

BOOST_AUTO_TEST_CASE(burrowing)
{
  TClass* wrapper_type{TClass::GetClass(typeid(std::vector<int>))};
  BOOST_REQUIRE(wrapper_type);
  TypeWithDict const v_type{
    art::root::find_nested_type_named("value_type", wrapper_type)};
  BOOST_REQUIRE(v_type);
  BOOST_REQUIRE_EQUAL(v_type.className(), "int"s);
}

BOOST_AUTO_TEST_CASE(type_of_template_arg)
{
  TClass* wrapper{TClass::GetClass("art::Wrapper<std::vector<int>>")};
  BOOST_REQUIRE(wrapper);
  TypeWithDict const ty{art::root::type_of_template_arg(wrapper, 0)};
  BOOST_REQUIRE(ty);
  TClass* arg(ty.tClass());
  BOOST_REQUIRE(arg);
  BOOST_REQUIRE_EQUAL(arg->GetName(), "vector<int>"s);
}

BOOST_AUTO_TEST_CASE(is_instantiation_of)
{
  TClass* wrapper{TClass::GetClass("art::Wrapper<int>")};
  BOOST_REQUIRE(wrapper);
  BOOST_REQUIRE(art::root::is_instantiation_of(wrapper, "art::Wrapper"));
}

BOOST_AUTO_TEST_SUITE_END()
