#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/exception.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {
  constexpr bool should_fail{false};

  void
  retrieve(fhicl::ParameterSet const& pset,
           std::string const& parm,
           bool const should_succeed = true) try {
    pset.get<art::InputTag>(parm);
    assert(should_succeed);
  }
  catch (fhicl::exception const& e) {
    assert(not should_succeed);
  }
}

int
main()
{
  //===========================================================
  // Test 1-argument InputTag c'tor
  //===========================================================

  fhicl::ParameterSet pset;
  pset.put<std::string>("first_key", "label:instance:processName");

  std::vector<std::string> vs;
  for (unsigned i(0); i != 10; ++i) {
    std::ostringstream value;
    value << "label" << i << ":"
          << "instance" << i << ":"
          << "processName" << i;
    vs.emplace_back(value.str());
  }
  pset.put("second_key", vs);

  auto const tag = pset.get<art::InputTag>("first_key");
  auto const vec_tags = pset.get<std::vector<art::InputTag>>("second_key");

  auto const vec_tags_empty =
    pset.get("third_key", std::vector<art::InputTag>{});

  assert(vec_tags_empty.empty());

  std::vector<art::InputTag> vtag;
  assert(pset.get_if_present("second_key", vtag));

  //===========================================================
  // Test multi-argument constructors
  //===========================================================

  pset.put<std::vector<std::string>>("multi1", {"label", "instance"});
  pset.put<std::vector<std::string>>("multi2",
                                     {"label", "instance", "process"});
  pset.put<std::vector<std::string>>("multierr1", {"label:something:else"});
  pset.put<std::vector<std::string>>(
    "multierr2", {"label", "instance", "process", "something", "else"});

  retrieve(pset, "multi1");
  retrieve(pset, "multi2");
  retrieve(pset, "multierr1", should_fail);
  retrieve(pset, "multierr2", should_fail);
}
