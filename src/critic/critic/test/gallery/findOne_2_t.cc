#define BOOST_TEST_MODULE (findOne_2_t)
#include "cetlib/quiet_unit_test.hpp"

#include "canvas/Persistency/Common/FindMany.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "canvas/Persistency/Common/FindOne.h"
#include "canvas/Persistency/Common/FindOneP.h"
#include "canvas/Persistency/Provenance/EventAuxiliary.h"
#include "critic/test/gallery/TestFindOne.h"
#include "gallery/Event.h"

#include <iostream>
#include <string>
#include <vector>

BOOST_AUTO_TEST_CASE(testFindOneAndFindMany)
{
  std::vector<std::string> filenames{"../CriticAssns_w.d/out1.root"};
  gallery::Event event(filenames);

  std::string inputLabel("ap1");
  bool testAB = false;
  bool testBA = true;
  bool bCollMissing = false;
  critictest::TestFindOne testFindOne("ap1", testAB, testBA, bCollMissing);

  for (; !event.atEnd(); event.next()) {

    art::EventAuxiliary const& aux = event.eventAuxiliary();
    std::cout << aux.id() << "\n";

    testFindOne.testOne<art::FindOne>(event);
    testFindOne.testOne<art::FindOneP>(event);

    testFindOne.testMany<art::FindMany>(event);
    testFindOne.testMany<art::FindManyP>(event);
  }
}
