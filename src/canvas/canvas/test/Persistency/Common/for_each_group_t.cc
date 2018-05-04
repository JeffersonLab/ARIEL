#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/AssnsAlgorithms.h"
#include "canvas/Persistency/Common/AssnsIter.h"
#include "canvas/Persistency/Common/Ptr.h"

#include <cassert>
#include <forward_list>
#include <type_traits>

using intvec_t = std::vector<int>;
using shortvec_t = std::vector<short>;
using strvec_t = std::vector<std::string>;
using floatvec_t = std::vector<float>;
using assns_ab_t = art::Assns<int, float>;
using assns_abd_t = art::Assns<int, float, short>;
using ci_t = assns_abd_t::const_iterator;

int
main()
{

  // initialization stuff
  art::ProductID const viid{2};
  art::ProductID const vfid{3};
  intvec_t const vi{1, 2, 3};
  floatvec_t const vf{1.0, 1.1, 2.0, 2.1, 3.0, 3.1};
  shortvec_t const vs{10, 11, 20, 21, 30, 31};
  intvec_t const vtest{1, 1, 2, 2, 3, 3};

  int k{};

  assns_abd_t assns;
  assns_ab_t a1;

  for (long unsigned int i = 0; i < 3; ++i) {
    auto p1 = art::Ptr<int>(viid, &vi[i], i);
    for (long unsigned int j = 0; j < 2; ++j) {
      auto p2 = art::Ptr<float>(vfid, &vf[k], k);
      assns.addSingle(p1, p2, vs[k]);
      a1.addSingle(p1, p2);
      ++k;
    }
  }

  // use of art::for_each_group

  floatvec_t fvec;
  auto floats = [&fvec](auto fs) {
    for (auto f = begin(fs); f != end(fs); ++f) {
      fvec.push_back(**f);
    }
  };

  art::for_each_group(a1, floats);
  // floats should be same as vf
  for (auto i = 0; i < 5; ++i) {
    if (fvec[i] != vf[i]) {
      throw art::Exception(art::errors::LogicError)
        << "Float #" << i << "expected to be '" << vf[i] << "', got '"
        << fvec[i] << "' instead!\n";
    }
  }
  return 0;
}
