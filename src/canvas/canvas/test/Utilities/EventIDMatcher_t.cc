// vim: set sw=2 :

#include "canvas/Persistency/Provenance/EventID.h"
#include "canvas/Utilities/EventIDMatcher.h"

//#include <algorithm>
#include <cstdlib>
//#include <functional>
#include <iostream>
#include <string>
#include <vector>

using namespace std::string_literals;

using namespace art;
// using namespace std::placeholders;
using namespace std;

void
dump_pats(vector<string> const& pats)
{
  int i = 0;
  for (auto const& val : pats) {
    cout << i << ": " << val << endl;
  }
}

void
dump_ids(vector<EventID> const& cont)
{
  int i = 0;
  for (auto const& val : cont) {
    cout << i << ": " << val << endl;
  }
}

void
find_matches(string const& pat,
             vector<EventID> const& eids,
             vector<EventID>& matches)
{
  auto m = EventIDMatcher(pat);
  //
  // The for loop is equivalent to this:
  //
  //   copy_if(eids.cbegin(), eids.cend(),
  //           back_inserter(matches),
  //           bind(&EventIDMatcher::match, &m, _1));
  //
  // For those addicted to functional programming, feel free
  // to replace the for loop.  I find the for loop much
  // easier to maintain and debug.
  //
  for (auto const& val : eids) {
    if (m.match(val)) {
      matches.push_back(val);
    }
  }
}

void
find_list_matches(vector<string> const& pats,
                  vector<EventID> const& eids,
                  vector<EventID>& matches)
{
  auto m = EventIDMatcher(pats);
  //
  // The for loop is equivalent to this:
  //
  //   copy_if(eids.cbegin(), eids.cend(),
  //           back_inserter(matches),
  //           bind(&EventIDMatcher::match, &m, _1));
  //
  // For those addicted to functional programming, feel free
  // to replace the for loop.  I find the for loop much
  // easier to maintain and debug.
  //
  for (auto const& val : eids) {
    if (m.match(val)) {
      matches.push_back(val);
    }
  }
}

void
check_match(string const& pat,
            vector<EventID> const& expected,
            vector<EventID> const& matches)
{
  // cout << "   eids.size(): " << eids.size() << endl;
  // cout << "matches.size(): " << matches.size() << endl;
  if (matches == expected) {
    return;
  }
  cerr << "Match with \"" << pat << "\" failed!" << endl;
  cerr << "expected:" << endl;
  dump_ids(expected);
  cerr << "matches:" << endl;
  dump_ids(matches);
  exit(EXIT_FAILURE);
}

void
check_list_match(vector<string> const& pats,
                 vector<EventID> const& expected,
                 vector<EventID> const& matches)
{
  // cout << "   eids.size(): " << eids.size() << endl;
  // cout << "matches.size(): " << matches.size() << endl;
  if (matches == expected) {
    return;
  }
  cerr << "Match with pattern list failed!" << endl;
  cerr << "patterns:" << endl;
  dump_pats(pats);
  cerr << "expected:" << endl;
  dump_ids(expected);
  cerr << "matches:" << endl;
  dump_ids(matches);
  exit(EXIT_FAILURE);
}

void
run_test(string const& pat,
         vector<EventID> const& eids,
         vector<EventID> const& expected)
{
  vector<EventID> matches;
  find_matches(pat, eids, matches);
  check_match(pat, expected, matches);
}

void
run_list_test(vector<string> const& pats,
              vector<EventID> const& eids,
              vector<EventID> const& expected)
{
  vector<EventID> matches;
  find_list_matches(pats, eids, matches);
  check_list_match(pats, expected, matches);
}

int
main()
{
  //
  //  Generate a list of EventIDs to test with.
  //
  vector<EventID> eids;
  for (auto run = 1U; run < 3U; ++run) {
    for (auto subrun = 0U; subrun < 2U; ++subrun) {
      for (auto event = 1U; event < 3U; ++event) {
        eids.emplace_back(run, subrun, event);
      }
    }
  }
  //
  //  Check wildcards.
  //
  if (1) {
    // Match everything.
    auto pat = "*:*:*"s;
    vector<EventID> expected = {
      {1, 0, 1},
      {1, 0, 2},
      {1, 1, 1},
      {1, 1, 2},
      {2, 0, 1},
      {2, 0, 2},
      {2, 1, 1},
      {2, 1, 2},
    };
    run_test(pat, eids, expected);
  }
  if (1) {
    // Match any run, any subRun.
    auto pat = "*:*:1"s;
    vector<EventID> expected = {
      {1, 0, 1},
      {1, 1, 1},
      {2, 0, 1},
      {2, 1, 1},
    };
    run_test(pat, eids, expected);
  }
  if (1) {
    // Match any run.
    auto pat = "*:0:1"s;
    vector<EventID> expected = {
      {1, 0, 1},
      {2, 0, 1},
    };
    run_test(pat, eids, expected);
  }
  if (1) {
    // Match entry 0.
    auto pat = "1:0:1"s;
    vector<EventID> expected = {
      {1, 0, 1},
    };
    run_test(pat, eids, expected);
  }
  if (1) {
    // Match entry 1.
    auto pat = "1:0:2"s;
    vector<EventID> expected = {
      {1, 0, 2},
    };
    run_test(pat, eids, expected);
  }
  if (1) {
    // Match entry 2.
    auto pat = "1:1:1"s;
    vector<EventID> expected = {
      {1, 1, 1},
    };
    run_test(pat, eids, expected);
  }
  if (1) {
    // Match entry 3.
    auto pat = "1:1:2"s;
    vector<EventID> expected = {
      {1, 1, 2},
    };
    run_test(pat, eids, expected);
  }
  if (1) {
    // Match entry 4.
    auto pat = "2:0:1"s;
    vector<EventID> expected = {
      {2, 0, 1},
    };
    run_test(pat, eids, expected);
  }
  if (1) {
    // Match entry 5.
    auto pat = "2:0:2"s;
    vector<EventID> expected = {
      {2, 0, 2},
    };
    run_test(pat, eids, expected);
  }
  if (1) {
    // Match entry 6.
    auto pat = "2:1:1"s;
    vector<EventID> expected = {
      {2, 1, 1},
    };
    run_test(pat, eids, expected);
  }
  if (1) {
    // Match entry 7.
    auto pat = "2:1:2"s;
    vector<EventID> expected = {
      {2, 1, 2},
    };
    run_test(pat, eids, expected);
  }
  if (1) {
    // Match run range.
    auto pat = "1-2:0:1"s;
    vector<EventID> expected = {
      {1, 0, 1},
      {2, 0, 1},
    };
    run_test(pat, eids, expected);
  }
  if (1) {
    // Match subrun range.
    auto pat = "1:0-1:1"s;
    vector<EventID> expected = {
      {1, 0, 1},
      {1, 1, 1},
    };
    run_test(pat, eids, expected);
  }
  if (1) {
    // Match event range.
    auto pat = "1:0:1-2"s;
    vector<EventID> expected = {
      {1, 0, 1},
      {1, 0, 2},
    };
    run_test(pat, eids, expected);
  }
  {
    vector<EventID> empty;
    eids.swap(empty);
  }
  for (auto run = 1U; run < 11U; ++run) {
    for (auto subrun = 0U; subrun < 10U; ++subrun) {
      for (auto event = 1U; event < 11U; ++event) {
        eids.emplace_back(run, subrun, event);
      }
    }
  }
  if (1) {
    // Match run range list.
    auto pat = "1-2, 4, 6-9 : 0 : 1"s;
    vector<EventID> expected = {
      {1, 0, 1},
      {2, 0, 1},
      {4, 0, 1},
      {6, 0, 1},
      {7, 0, 1},
      {8, 0, 1},
      {9, 0, 1},
    };
    run_test(pat, eids, expected);
  }
  if (1) {
    // Match subrun range list.
    auto pat = "3: 1-3, 6, 9 : 5"s;
    vector<EventID> expected = {
      {3, 1, 5},
      {3, 2, 5},
      {3, 3, 5},
      {3, 6, 5},
      {3, 9, 5},
    };
    run_test(pat, eids, expected);
  }
  if (1) {
    // Match event range list.
    auto pat = "7: 5 : 3, 5-5, 6-7, 9-10"s;
    vector<EventID> expected = {
      {7, 5, 3},
      {7, 5, 5},
      {7, 5, 6},
      {7, 5, 7},
      {7, 5, 9},
      {7, 5, 10},
    };
    run_test(pat, eids, expected);
  }
  if (1) {
    // Match complex pattern.
    auto pat = "1, 6 : 0, 2, 5-6  : 3, 6-7"s;
    vector<EventID> expected = {
      {1, 0, 3}, {1, 0, 6}, {1, 0, 7}, {1, 2, 3}, {1, 2, 6}, {1, 2, 7},
      {1, 5, 3}, {1, 5, 6}, {1, 5, 7}, {1, 6, 3}, {1, 6, 6}, {1, 6, 7},
      {6, 0, 3}, {6, 0, 6}, {6, 0, 7}, {6, 2, 3}, {6, 2, 6}, {6, 2, 7},
      {6, 5, 3}, {6, 5, 6}, {6, 5, 7}, {6, 6, 3}, {6, 6, 6}, {6, 6, 7},
    };
    run_test(pat, eids, expected);
  }
  if (1) {
    // Simple pattern list.
    vector<string> pats = {
      "1:0:1"s,
      "5:3:10"s,
    };
    vector<EventID> expected = {
      {1, 0, 1},
      {5, 3, 10},
    };
    run_list_test(pats, eids, expected);
  }
  if (1) {
    // Complex pattern list.
    vector<string> pats = {
      "7: 5 : 3, 5-5, 6-7, 9-10"s,
      "1, 6 : 0, 2, 5-6  : 3, 6-7"s,
      "1-2: 5 : 3, 5-5, 6-7, 9-10"s,
    };
    vector<EventID> expected = {
      {1, 0, 3}, {1, 0, 6}, {1, 0, 7},  {1, 2, 3}, {1, 2, 6}, {1, 2, 7},
      {1, 5, 3}, {1, 5, 5}, {1, 5, 6},  {1, 5, 7}, {1, 5, 9}, {1, 5, 10},
      {1, 6, 3}, {1, 6, 6}, {1, 6, 7},  {2, 5, 3}, {2, 5, 5}, {2, 5, 6},
      {2, 5, 7}, {2, 5, 9}, {2, 5, 10}, {6, 0, 3}, {6, 0, 6}, {6, 0, 7},
      {6, 2, 3}, {6, 2, 6}, {6, 2, 7},  {6, 5, 3}, {6, 5, 6}, {6, 5, 7},
      {6, 6, 3}, {6, 6, 6}, {6, 6, 7},  {7, 5, 3}, {7, 5, 5}, {7, 5, 6},
      {7, 5, 7}, {7, 5, 9}, {7, 5, 10},
    };
    run_list_test(pats, eids, expected);
  }
}
