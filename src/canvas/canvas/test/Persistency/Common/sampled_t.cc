#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "canvas/Persistency/Common/Sampled.h"
#include "canvas/Utilities/Exception.h"

#include <string>
#include <vector>

using namespace art;

namespace {
  InputTag const invalid_tag{};

  template <typename T>
  void
  assert_sampled_allowed()
  {
    Sampled<T>{invalid_tag};
  }

  template <typename T>
  void
  assert_sampled_forbidden() try {
    Sampled<T>{invalid_tag};
    assert(false && "Successful construction not expected.");
  }
  catch (Exception const& e) {
    std::string const actual_msg{e.what()};
    std::string const expected_msg{"An attempt was made to create the type"};
    assert(actual_msg.find(expected_msg) != std::string::npos);
  }
}

int
main()
{
  assert_sampled_allowed<int>();
  assert_sampled_allowed<std::vector<int>>();
  assert_sampled_forbidden<Assns<int, double>>();
  assert_sampled_forbidden<Assns<int, double, std::string>>();
  assert_sampled_forbidden<Ptr<int>>();
  assert_sampled_forbidden<PtrVector<int>>();
  assert_sampled_forbidden<std::vector<Ptr<int>>>();
  assert_sampled_forbidden<std::map<Ptr<int>, Ptr<double>>>();
  assert_sampled_forbidden<std::list<Ptr<int>>>();
  assert_sampled_forbidden<std::vector<std::vector<std::vector<Ptr<int>>>>>();
}
