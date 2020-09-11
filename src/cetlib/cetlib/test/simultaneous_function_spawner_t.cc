#define BOOST_TEST_MODULE (simultaneous_function_spawner test)
#include "cetlib/quiet_unit_test.hpp"

#include "cetlib/SimultaneousFunctionSpawner.h"
#include <iostream>

BOOST_AUTO_TEST_SUITE(simultaneous_function_spawner)

BOOST_AUTO_TEST_CASE(cout)
{
  auto task = [] { std::cout << "Hello concurrent world.\n"; };
  cet::SimultaneousFunctionSpawner sfs{cet::repeated_task(7u, task)};
}

BOOST_AUTO_TEST_CASE(atomic_int)
{
  std::atomic<int> i{1};
  auto task = [&i] { ++i; };
  cet::SimultaneousFunctionSpawner sfs{cet::repeated_task(7u, task)};
  BOOST_TEST(i == 8);
}

BOOST_AUTO_TEST_CASE(assign_different_numbers)
{
  std::vector<int> nums(7); // All 7 numbers initialized to 0.
  std::vector<std::function<void()>> tasks;
  tasks.push_back([&nums] { nums[0] = 1; });
  tasks.push_back([&nums] { nums[1] = 3; });
  tasks.push_back([&nums] { nums[2] = 5; });
  tasks.push_back([&nums] { nums[3] = 7; });
  tasks.push_back([&nums] { nums[4] = 6; });
  tasks.push_back([&nums] { nums[5] = 4; });
  tasks.push_back([&nums] { nums[6] = 2; });
  auto const ref = {1, 3, 5, 7, 6, 4, 2};
  cet::SimultaneousFunctionSpawner sfs{tasks};
  BOOST_TEST(nums == ref, boost::test_tools::per_element{});
}

BOOST_AUTO_TEST_SUITE_END()
