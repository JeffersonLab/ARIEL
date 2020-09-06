#define BOOST_TEST_MODULE(EventID_t)
#include "cetlib/quiet_unit_test.hpp"

#include "canvas/Persistency/Provenance/EventID.h"

using namespace art;

BOOST_AUTO_TEST_SUITE(EventID_t)

BOOST_AUTO_TEST_CASE(DefaultConstruction)
{
  EventID def;
  BOOST_TEST(!def.isValid());
  BOOST_TEST(!def.subRunID().isValid());
  BOOST_TEST(!def.runID().isValid());
}

BOOST_AUTO_TEST_CASE(FullSpecConstruction)
{
  EventNumber_t const et = 1;
  RunNumber_t const rt = 2;
  SubRunNumber_t const srt = 5;
  EventID good(rt, srt, et);
  BOOST_TEST(good.run() == rt);
  BOOST_TEST(good.subRun() == srt);
  BOOST_TEST(good.event() == et);
}

BOOST_AUTO_TEST_CASE(Flush)
{
  EventID f(EventID::flushEvent());
  BOOST_TEST(f.isValid());
  BOOST_TEST(f.isFlush());
  BOOST_TEST(f.subRunID().isFlush());
  BOOST_TEST(f.runID().isFlush());

  EventID fs(EventID::flushEvent(RunID::firstRun()));
  BOOST_TEST(fs.isValid());
  BOOST_TEST(fs.isFlush());
  BOOST_TEST(fs.subRunID().isFlush());
  BOOST_TEST(!fs.runID().isFlush());
  BOOST_TEST(fs.run() == RunID::firstRun().run());

  BOOST_REQUIRE_THROW(EventID(f.run(), f.subRun(), f.event()), art::Exception);
}

BOOST_AUTO_TEST_CASE(Comparison)
{
  EventID const small(1, 1, 1);
  EventID const med(2, 2, 2);
  EventID const med2(2, 2, 2);
  EventID const large(3, 3, 2);
  EventID const largest(3, 3, 3);
  BOOST_TEST(small < med);
  BOOST_TEST(small <= med);
  BOOST_TEST(!(small == med));
  BOOST_TEST(small != med);
  BOOST_TEST(!(small > med));
  BOOST_TEST(!(small >= med));
  BOOST_TEST(med2 == med);
  BOOST_TEST(med2 <= med);
  BOOST_TEST(med2 >= med);
  BOOST_TEST(!(med2 != med));
  BOOST_TEST(!(med2 < med));
  BOOST_TEST(!(med2 > med));
  BOOST_TEST(med < large);
  BOOST_TEST(med <= large);
  BOOST_TEST(!(med == large));
  BOOST_TEST(med != large);
  BOOST_TEST(!(med > large));
  BOOST_TEST(!(med >= large));
  BOOST_TEST(large < largest);
  BOOST_TEST(large <= largest);
  BOOST_TEST(!(large == largest));
  BOOST_TEST(large != largest);
  BOOST_TEST(!(large > largest));
  BOOST_TEST(!(large >= largest));
}

BOOST_AUTO_TEST_CASE(Iteration)
{
  EventID first = EventID::firstEvent();
  EventID second = first.next();
  BOOST_TEST(first < second);
  BOOST_TEST(first == (second.previous()));
  EventID run2(2, 1, 1);
  BOOST_TEST(run2 < run2.nextRun());
  BOOST_TEST(run2 < run2.nextSubRun());

  EventID largeEvent(SubRunID::firstSubRun(), EventID::maxEvent().event());
  BOOST_TEST(largeEvent.isValid());
  EventID next(largeEvent.next());
  BOOST_TEST(SubRunID::firstSubRun().next() == next.subRunID());
  BOOST_TEST(next.event() == EventID::firstEvent().event());
  BOOST_TEST(next.isValid());
  BOOST_TEST(!next.isFlush());
}

BOOST_AUTO_TEST_CASE(AllowConstructedZeroEvent)
{
  EventID e1(3u, 0u, 0u);
  BOOST_TEST(e1.event() == 0u);
  EventID e2(SubRunID(3u, 0u), 0u);
  BOOST_TEST(e2.event() == 0u);
  BOOST_TEST(e2.nextSubRun(0u).event() == 0u);
}

BOOST_AUTO_TEST_SUITE_END()
