#define BOOST_TEST_MODULE (ParentageRegistry_t)
#include "cetlib/quiet_unit_test.hpp"

#include "canvas/Persistency/Provenance/Parentage.h"
#include "canvas/Persistency/Provenance/ParentageRegistry.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "cetlib/SimultaneousFunctionSpawner.h"
#include "cetlib/container_algorithms.h"

#include <string>
#include <vector>

using namespace art;
using namespace std::string_literals;

BOOST_AUTO_TEST_SUITE(ParentageTest)

BOOST_AUTO_TEST_CASE(concurrent_insertion_reading)
{
  using ParentNames = std::vector<std::string>;
  using ParentProductIDs = std::vector<ProductID>;

  std::vector<ParentProductIDs> setsOfParents;
  {
    std::vector<ParentNames> setsOfParentNames{
      {"p1a", "p1b", "p1c"}, {"p2a", "p2b", "p2c", "p2d"}, {"p3a", "p3b"}};
    cet::transform_all(setsOfParentNames,
                       std::back_inserter(setsOfParents),
                       [](auto const& parentNames) {
                         ParentProductIDs pids;
                         cet::transform_all(
                           parentNames,
                           std::back_inserter(pids),
                           [](auto const& name) { return ProductID{name}; });
                         return pids;
                       });
  }

  // From branch-names to parentages
  std::vector<Parentage> parentages;
  cet::transform_all(setsOfParents,
                     std::back_inserter(parentages),
                     [](auto const& parents) { return Parentage{parents}; });

  // Insert parentages in parallel
  {
    std::vector<std::function<void()>> tasks;
    cet::transform_all(
      parentages, std::back_inserter(tasks), [](auto const& p) {
        return [&p] { ParentageRegistry::emplace(p.id(), p); };
      });
    cet::SimultaneousFunctionSpawner sfs{tasks};
  }

  BOOST_REQUIRE(ParentageRegistry::get().size() == parentages.size());

  // Retrieve histories in parallel
  {
    std::vector<Parentage> retrievedParentages(parentages.size());
    std::vector<std::function<void()>> tasks;
    cet::for_all_with_index(
      parentages,
      [&retrievedParentages, &tasks](std::size_t const i, auto const& h) {
        auto& entry = retrievedParentages[i];
        tasks.push_back([&h, &entry] {
          Parentage retrievedParentage;
          ParentageRegistry::get(h.id(), retrievedParentage);
          entry = std::move(retrievedParentage);
        });
      });
    cet::SimultaneousFunctionSpawner sfs{tasks};
    BOOST_TEST(parentages == retrievedParentages);
  }
}

BOOST_AUTO_TEST_SUITE_END()
