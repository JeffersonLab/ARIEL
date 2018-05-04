#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include <cassert>
#include <string>

int
main()
{
  fhicl::ParameterSet ps;
  std::string text("m1: { val: 3 name: boo} ");
  fhicl::make_ParameterSet(text, ps);

  assert(ps.get<int>("m1.val") == 3);
  assert(ps.get<std::string>("m1.name") == "boo");
}
