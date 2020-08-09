#ifndef SimpleDataProducts_TrackSummary_h
#define SimpleDataProducts_TrackSummary_h

//
// Some statistics about a track. Not very complete ...
//

#include <iosfwd>

namespace tex {

  class TrackSummary {

  public:

    TrackSummary();

#ifndef __GCCXML__

    int nHits()      const { return nHitsInner_ + nHitsOuter_; }
    int nHitsInner() const { return nHitsInner_; }
    int nHitsOuter() const { return nHitsOuter_; }

    void incrementInner();
    void incrementOuter();

#endif  // __GCCXML__

  private:
    int nHitsInner_;
    int nHitsOuter_;
  };

#ifndef __GCCXML__
  std::ostream& operator<<(std::ostream& ost,
                           const tex::TrackSummary& helix );
#endif  // __GCCXML__

}
#endif  /* SimpleDataProducts_TrackSummary_h */
