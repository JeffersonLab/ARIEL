#include "gallery/Event.h"

#include <string>
#include <utility>
#include <vector>

void
check_event_status(gallery::Event const& ev)
{
  // Since the Event advances to the first event, if we have no
  // events, we end up advancing past the file.
  assert(ev.eventEntry() == 0);
  assert(ev.fileEntry() == 1);

  // We don't have any events, so we're not valid and we're at the
  // end.
  assert(!ev.isValid());
  assert(ev.atEnd());

  // We've advanced off the end of the list of files, so we've got no
  // TFile or TTree.
  assert(ev.getTFile() == nullptr);
  assert(ev.getTTree() == nullptr);
}

int
main(int argc, char* argv[])
{

  gallery::Event ee({});

  std::vector<std::string> args(argv + 1, argv + argc);
  if (args.empty())
    return 1;

  // Create a gallery::Event from a file with no events.
  gallery::Event ev(args);
  assert(ev.numberOfEventsInFile() == 0);
  check_event_status(ev);

  ev.toBegin();
  check_event_status(ev);
  ev.first();
  check_event_status(ev);

  gallery::Event e2(std::move(ev));
  check_event_status(e2);

  ev = std::move(e2);
  check_event_status(ev);

  // An attempt to increment an Event which is at the end of the file
  // is an error.
  try {
    ev.next();
    assert("Failed to throw an exception!");
  }
  catch (art::Exception const& x) {
    assert(x.categoryCode() == art::errors::LogicError);
  }
  catch (...) {
    assert("Failed to throw required exception!" == nullptr);
  }

  // An attempt to decrement an Event which is at the end of the file
  // is an error.
  try {
    ev.previous();
    assert("Failed to throw an exception!");
  }
  catch (art::Exception const& x) {
    assert(x.categoryCode() == art::errors::LogicError);
  }
  catch (...) {
    assert("Failed to throw required exception!" == nullptr);
  }
}
