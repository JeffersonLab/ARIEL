#include "fhiclcpp/ParameterSet.h"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

int
main()
{
  fhicl::ParameterSet pset;

  pset.put<int>("a", -1);
  assert(pset.get<int>("a") == -1);
  try {
    pset.get<unsigned int>("a", 0u);
    assert(false);
  }
  catch (fhicl::exception const& e) {
    assert(true);
  }

  pset.put<std::string>("b", "string");
  assert(pset.get<std::string>("b") == "string");

  pset.put<double>("c", 2.3);
  assert(pset.get<double>("c") == 2.3);

  pset.put<bool>("d", true);
  assert(pset.get<bool>("d") == true);

  std::vector<int> vi;
  vi.push_back(-1);
  vi.push_back(-2);
  vi.push_back(-3);
  pset.put<std::vector<int>>("e", vi);
  assert(pset.get<std::vector<int>>("e")[1] == -2);

  try {
    std::vector<unsigned int> vui_def;
    pset.get<std::vector<unsigned int>>("e", vui_def);
    assert(false);
  }
  catch (fhicl::exception const& e) {
    assert(true);
  }

  std::vector<double> vd;
  vd.push_back(0.1);
  vd.push_back(0.2);
  vd.push_back(0.3);
  pset.put<std::vector<double>>("f", vd);
  assert(pset.get<std::vector<double>>("f")[1] == 0.2);

  std::vector<std::string> vs;
  vs.push_back("str1");
  vs.push_back("str2");
  vs.push_back("str3");
  pset.put<std::vector<std::string>>("g", vs);

  fhicl::ParameterSet pset2;
  pset2.put<int>("a2", 1);
  pset2.put<std::string>("b2", "bstring");
  pset.put<fhicl::ParameterSet>("h", pset2);
  assert(pset.get<fhicl::ParameterSet>("h").get<std::string>("b2") ==
         "bstring");
  assert(pset2.to_string() == "a2:1 b2:\"bstring\"");

  pset.put<unsigned int>("u1", 12);
  assert(pset.get<unsigned int>("u1") == 12);

  pset.put<unsigned int>("u2", 0);
  assert(pset.get<unsigned int>("u2") == 0);

  std::vector<unsigned int> vui;
  vui.push_back(1);
  vui.push_back(2);
  vui.push_back(3);
  pset.put<std::vector<unsigned int>>("vu", vui);
  assert(pset.get<std::vector<int>>("vu")[1] == 2);
  assert(pset.get<std::vector<unsigned int>>("vu")[1] == 2);

  pset.put<std::string>("int_str", "012");
  assert(pset.get<int>("int_str") == 12);

  pset.put<std::string>("float_str", "003e2");
  assert(pset.get<double>("float_str") == 3e2);

  pset.put<std::string>("float_str2", "003.200");
  assert(pset.get<double>("float_str2") == 3.2);

  pset.put<bool>("b1", true);
  assert(pset.get<bool>("b1") == true);

  pset.put<bool>("b2", false);
  assert(pset.get<bool>("b2") == false);

  std::vector<std::string> names = pset.get_pset_names();
  assert(!names.empty());
  assert(names[0].compare("h") == 0);

  pset.put<std::string>("b3", "true");
  assert(pset.get<bool>("b3") == true);
  pset.put<std::string>("b4", "false");
  assert(pset.get<bool>("b4") == false);

  return 0;
}
