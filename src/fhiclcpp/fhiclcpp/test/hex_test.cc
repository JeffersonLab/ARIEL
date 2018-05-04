#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include <cassert>
#include <string>

int
main()
{
  fhicl::ParameterSet ps;
  std::string text(
    "small: 0xF large: 0X1000 very_large: 0xabcdefabcdefabcdef ");
  text.append("small_bin: 0b01 large_bin: 0b01010101");
  fhicl::make_ParameterSet(text, ps);
  assert(ps.get<int>("small") == 15);
  assert(ps.get<unsigned long>("large") == 0x1000);
  assert(ps.get<std::string>("very_large") == "3.169232504802520059375e21");
  assert(ps.get<int>("small_bin") == 1);
  assert(ps.get<unsigned long>("large_bin") == 85);
}
