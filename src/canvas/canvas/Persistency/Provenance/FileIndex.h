#ifndef canvas_Persistency_Provenance_FileIndex_h
#define canvas_Persistency_Provenance_FileIndex_h

////////////////////////////////////////////////////////////////////////
//
// This intra-file indexing system has known issues and will be going
// away at the earliest possible opportunity. In the meantime however,
// there are some things of which the user of this class should be
// aware:
//
// 1. This class is *not* intended for use outside ART. It should
//    probably be put into the detail namespace.
//
// 2. This class depends implicitly on an invalid value of the run,
//    subrun or event in an EventID sorting *before* a valid one. This
//    should be enforced in the comparison operations of the EventID and
//    related classes.
//
// 3. Due to user requirements, it *is* possible to findEventPosition()
//    using an EventID which is invalid in one particular way: the run
//    and event numbers are valid, but the subrun number is not. HOWEVER,
//    this only makes sense in an environment where the run number and
//    event number specify the event uniquely. No check is made that an
//    answer returned by findEventPosition() in these circumstances is
//    in any way unique.
//
////////////////////////////////////////////////////////////////////////

#include "canvas/Persistency/Provenance/EventID.h"
#include "canvas/Persistency/Provenance/RunID.h"
#include "canvas/Persistency/Provenance/SubRunID.h"
#include "canvas/Persistency/Provenance/Transient.h"
#include <iosfwd>
#include <vector>

namespace art {
  class FileIndex;
}

class art::FileIndex {

public:
  using EntryNumber_t = long long;

  FileIndex() = default; // Necessary for ROOT

  void addEntry(EventID const& eID, EntryNumber_t entry);

  void addEntryOnLoad(EventID const& eID, EntryNumber_t entry);

  enum EntryType { kRun, kSubRun, kEvent, kEnd };

  class Element {
  public:
    static EntryNumber_t constexpr invalidEntry{-1};
    Element() = default;
    Element(EventID const& eID) : Element(eID, invalidEntry) {}
    Element(EventID const& eID, EntryNumber_t const entry)
      : eventID_{eID}, entry_{entry}
    {}
    EntryType
    getEntryType() const
    {
      return eventID_.isValid() ?
               kEvent :
               (eventID_.subRunID().isValid() ? kSubRun : kRun);
    }
    EventID eventID_{};
    EntryNumber_t entry_{invalidEntry};
  };

  using const_iterator = std::vector<Element>::const_iterator;
  using iterator = std::vector<Element>::iterator;

  void sortBy_Run_SubRun_Event();
  void sortBy_Run_SubRun_EventEntry();

  const_iterator findPosition(EventID const& eID) const;

  template <typename ID>
  const_iterator findPosition(ID const& id, bool exact) const;

  const_iterator findSubRunOrRunPosition(SubRunID const& srID) const;

  template <typename ID>
  bool
  contains(ID const& id, bool exact) const
  {
    return findPosition(id, exact) != entries_.end();
  }

  iterator
  begin()
  {
    return entries_.begin();
  }
  const_iterator
  begin() const
  {
    return entries_.begin();
  }
  const_iterator
  cbegin() const
  {
    return entries_.begin();
  }

  iterator
  end()
  {
    return entries_.end();
  }
  const_iterator
  end() const
  {
    return entries_.end();
  }
  const_iterator
  cend() const
  {
    return entries_.end();
  }

  std::vector<Element>::size_type
  size() const
  {
    return entries_.size();
  }

  bool
  empty() const
  {
    return entries_.empty();
  }

  bool allEventsInEntryOrder() const;

  bool eventsUniqueAndOrdered() const;

  void print_event_list(std::ostream& os) const;

  enum SortState {
    kNotSorted,
    kSorted_Run_SubRun_Event,
    kSorted_Run_SubRun_EventEntry
  };
  struct Transients {
    bool allInEntryOrder_{false};
    bool resultCached_{false};
    // The default value for sortState_ reflects the fact that
    // the index is always sorted using Run, SubRun, and Event
    // number by the RootOutput before being written out.
    // In the other case when we create a new FileIndex, the
    // vector is empty, which is consistent with it having been
    // sorted.
    SortState sortState_{kSorted_Run_SubRun_Event};
  };

private:
  bool&
  allInEntryOrder() const
  {
    return transients_.get().allInEntryOrder_;
  }
  bool&
  resultCached() const
  {
    return transients_.get().resultCached_;
  }
  SortState&
  sortState() const
  {
    return transients_.get().sortState_;
  }

  const_iterator findEventForUnspecifiedSubRun(EventID const& eID,
                                               bool exact) const;

  std::vector<Element> entries_{};
  mutable Transient<Transients> transients_{};
};

namespace art {
  bool operator<(FileIndex::Element const& lh, FileIndex::Element const& rh);

  inline bool
  operator>(FileIndex::Element const& lh, FileIndex::Element const& rh)
  {
    return rh < lh;
  }

  inline bool
  operator>=(FileIndex::Element const& lh, FileIndex::Element const& rh)
  {
    return !(lh < rh);
  }

  inline bool
  operator<=(FileIndex::Element const& lh, FileIndex::Element const& rh)
  {
    return !(rh < lh);
  }

  inline bool
  operator==(FileIndex::Element const& lh, FileIndex::Element const& rh)
  {
    return !(lh < rh || rh < lh);
  }

  inline bool
  operator!=(FileIndex::Element const& lh, FileIndex::Element const& rh)
  {
    return lh < rh || rh < lh;
  }

  class Compare_Run_SubRun_EventEntry {
  public:
    bool operator()(FileIndex::Element const& lh, FileIndex::Element const& rh);
  };

  std::ostream& operator<<(std::ostream& os, FileIndex::Element const& el);

  std::ostream& operator<<(std::ostream& os, FileIndex const& fileIndex);
}

#endif /* canvas_Persistency_Provenance_FileIndex_h */

// Local Variables:
// mode: c++
// End:
