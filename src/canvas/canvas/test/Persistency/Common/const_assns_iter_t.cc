#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/AssnsIter.h"
#include "canvas/Persistency/Common/Ptr.h"

#include <cassert>
#include <forward_list>
#include <type_traits>

using namespace art;

using intvec_t = std::vector<int>;
using shortvec_t = std::vector<short>;
using strvec_t = std::vector<std::string>;
using floatvec_t = std::vector<float>;
using assns_ab_t = art::Assns<int, float>;
using assns_abd_t = art::Assns<int, float, short>;
using ci_t = assns_abd_t::const_iterator;

using ab_node_t = assns_ab_t::const_iterator::value_type;
using ab_node_first_t = ab_node_t::first_type;
using ab_node_second_t = ab_node_t::second_type;
using abd_node_t = ci_t::value_type;
using abd_node_first_t = abd_node_t::first_type;
using abd_node_second_t = abd_node_t::second_type;
using abd_node_data_t = abd_node_t::data_type;

static_assert(std::is_same_v<ab_node_first_t, abd_node_first_t>);
static_assert(std::is_same_v<ab_node_second_t, abd_node_second_t>);
static_assert(std::is_same_v<abd_node_first_t, tuple_element_t<0, abd_node_t>>);
static_assert(
  std::is_same_v<abd_node_second_t, tuple_element_t<1, abd_node_t>>);
static_assert(std::is_same_v<abd_node_data_t, tuple_element_t<2, abd_node_t>>);

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

  // iterator increment and dereference test
  auto my_begin = assns.begin();
  auto const my_end = assns.end();

  auto check_values =
    [](auto const& r, short const s, float const f, int const j) {
      auto const& first = get<abd_node_first_t>(r);
      auto const& second = get<abd_node_second_t>(r);
      auto const& data = get<abd_node_data_t>(r);

      auto const& firstA = get<0>(r);
      auto const& secondA = get<1>(r);
      auto const& dataA = get<2>(r);

      auto const& firstB = get<Ptr<int>>(r);
      auto const& secondB = get<Ptr<float>>(r);
      auto const& dataB = get<short>(r);

      assert(first == firstA);
      assert(second == secondA);
      assert(data == dataA);

      assert(first == firstB);
      assert(second == secondB);
      assert(data == dataB);

      assert(data == s);
      assert(*second == f);
      assert(*first == j);
    };

  k = 0;
  for (auto p = my_begin; p != my_end; ++p) {
    auto const& r = *p;
    check_values(r, vs[k], vf[k], vtest[k]);
    ++k;
  }

  // range-for loop test
  k = 0;
  for (auto const& p : assns) {
    check_values(p, vs[k], vf[k], vtest[k]);
    ++k;
  }

  // std::for_each test
  auto print = [](auto const& r) { std::cout << " " << *(r.data); };
  std::for_each(assns.begin(), assns.end(), print);
  std::cout << std::endl;

  static_assert(std::is_copy_constructible_v<ci_t>);
  static_assert(std::is_copy_assignable_v<ci_t>);
  static_assert(std::is_move_assignable_v<ci_t>);

  assert(*((*(my_begin + 4)).data) == 30);
  my_begin += 4;
  assert(*((*my_begin).data) == 30);
  assert(my_begin - my_end == 2);
  assert(*(my_begin[0].data) == 30);
  assert(*(my_begin->data) == 30);
  assert((my_begin < my_end) == true);
  assert((my_begin <= my_end) == true);
  assert((my_begin > my_end) == false);
  assert((my_begin >= my_end) == false);

  // reverse iterator test
  assns_abd_t empty_assns;
  assert(empty_assns.rbegin() == empty_assns.rend());

  k = assns.size() - 1;
  auto const my_rbegin = assns.rbegin();
  auto const my_rend = assns.rend();
  for (auto it = my_rbegin; it != my_rend; ++it) {
    check_values(*it, vs[k], vf[k], vtest[k]);
    --k;
  }

  assert((my_rbegin < my_rend) == true);
  assert((my_rbegin <= my_rend) == true);
  assert((my_rbegin > my_rend) == false);
  assert((my_rbegin >= my_rend) == false);

  return 0;
}
