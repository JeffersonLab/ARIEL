#ifndef MCDataProducts_MCRunSummary_h
#define MCDataProducts_MCRunSummary_h

//
// Some summary information about one art::Run of MC events.
// Used to illustrate adding a run scope data product at the end of run.
//

#include <ostream>

namespace tex {

  class MCRunSummary {

  public:
    MCRunSummary();

    void increment(size_t ngens, size_t nwith);
    void clear();

    size_t
    nAlive() const
    {
      return _nAlive;
    }
    size_t
    nGood() const
    {
      return _nGood;
    }

  private:
    // Total number of generated particles, summed over the run,
    // that were alive at the start of the detector simulation.
    int _nAlive;

    // Of these, the number with 5 or more intersections.
    int _nGood;
  };

  std::ostream& operator<<(std::ostream& ost, const MCRunSummary& sum);
}

#endif /* MCDataProducts_MCRunSummary_h */
