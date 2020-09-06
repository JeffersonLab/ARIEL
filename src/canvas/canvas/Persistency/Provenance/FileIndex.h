#ifndef canvas_Persistency_Provenance_FileIndex_h
#define canvas_Persistency_Provenance_FileIndex_h
// vim: set sw=2 expandtab :

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

  class FileIndex {

  public: // TYPES
    using EntryNumber_t = long long;

    enum EntryType {
      kRun /* 0 */
      ,
      kSubRun /* 1 */
      ,
      kEvent /* 2 */
      ,
      kEnd /* 3 */
    };

    class Element {

    public: // MEMBER DATA -- Static
      static constexpr EntryNumber_t invalidEntry{-1};

    public: // MEMBER FUNCTIONS -- Special Member Functions
      Element() = default;

      Element(EventID const& eID);

      Element(EventID const& eID, EntryNumber_t const entry);

    public: // MEMBER FUNCTIONS
      EntryType getEntryType() const;

    public: // MEMBER DATA
      EventID eventID_{};

      EntryNumber_t entry_{invalidEntry};
    };

    using const_iterator = std::vector<Element>::const_iterator;
    using iterator = std::vector<Element>::iterator;

    enum SortState {
      kNotSorted /* 0 */
      ,
      kSorted_Run_SubRun_Event /* 1 */
      ,
      kSorted_Run_SubRun_EventEntry /* 2 */
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

  public: // MEMBER FUNCTIONS
    void addEntry(EventID const& eID, EntryNumber_t entry);

    void addEntryOnLoad(EventID const& eID, EntryNumber_t entry);

    void sortBy_Run_SubRun_Event();

    void sortBy_Run_SubRun_EventEntry();

    const_iterator findPosition(EventID const& eID) const;

    const_iterator findPosition(EventID const& eID, bool exact) const;

    const_iterator findPosition(SubRunID const& srID, bool exact) const;

    const_iterator findPosition(RunID const& rID, bool exact) const;

    const_iterator findSubRunOrRunPosition(SubRunID const& srID) const;

    bool contains(EventID const& id, bool exact) const;

    bool contains(SubRunID const& id, bool exact) const;

    bool contains(RunID const& id, bool exact) const;

    iterator begin();

    const_iterator begin() const;

    const_iterator cbegin() const;

    iterator end();

    const_iterator end() const;

    const_iterator cend() const;

    std::vector<Element>::size_type size() const;

    bool empty() const;

    bool allEventsInEntryOrder() const;

    bool eventsUniqueAndOrdered() const;

    void print_event_list(std::ostream& os) const;

  private: // MEMBER FUNCTIONS -- Implementation details
    bool& allInEntryOrder() const;

    bool& resultCached() const;

    SortState& sortState() const;

    const_iterator findEventForUnspecifiedSubRun(EventID const& eID,
                                                 bool exact) const;

  private: // MEMBER DATA
    std::vector<Element> entries_{};

    mutable Transient<Transients> transients_{};
  };

  bool operator<(FileIndex::Element const& lh, FileIndex::Element const& rh);

  bool operator>(FileIndex::Element const& lh, FileIndex::Element const& rh);

  bool operator>=(FileIndex::Element const& lh, FileIndex::Element const& rh);

  bool operator<=(FileIndex::Element const& lh, FileIndex::Element const& rh);

  bool operator==(FileIndex::Element const& lh, FileIndex::Element const& rh);

  bool operator!=(FileIndex::Element const& lh, FileIndex::Element const& rh);

  class Compare_Run_SubRun_EventEntry {

  public:
    bool operator()(FileIndex::Element const& lh, FileIndex::Element const& rh);
  };

  std::ostream& operator<<(std::ostream& os, FileIndex::Element const& el);

  std::ostream& operator<<(std::ostream& os, FileIndex const& fileIndex);

} // namespace art

#endif /* canvas_Persistency_Provenance_FileIndex_h */

// Local Variables:
// mode: c++
// End:
