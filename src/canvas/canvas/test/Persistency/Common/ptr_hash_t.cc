#include "canvas/Persistency/Common/Ptr.h"
#include <cassert>
#include <unordered_set>

void
unordered_set_test()
{
  art::Ptr<int> def;
  std::unordered_set<art::Ptr<int>> s;
  // Make sure insertion works
  s.insert(def);
  assert(s.size() == 1);
  // Make sure insertion of a second item doesn't change the set
  s.insert(def);
  assert(s.size() == 1);
  // Make sure finding works.
  assert(s.find(def) == s.begin());

  // Don't do this at home, kids. This is a wildly unsafe Ptr! It does
  // suffice for testing the hashing ability.
  art::Ptr<int> p1(art::ProductID(234), nullptr, 7);
  art::Ptr<int> p2(art::ProductID(234), nullptr, 7);
  assert(def != p1);
  assert(p1 == p2);
  std::hash<art::Ptr<int>> hasher;
  assert(hasher(def) != hasher(p1));
  assert(hasher(p1) == hasher(p2));

  s.insert(p1);
  assert(s.size() == 2);
  assert(s.find(def) != s.end());
  assert(s.find(p1) != s.end());
}

int
main()
{
  unordered_set_test();
}
