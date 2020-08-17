#ifndef canvas_Persistency_Common_HLTenums_h
#define canvas_Persistency_Common_HLTenums_h
// vim: set sw=2 expandtab :

namespace art::hlt {
  enum HLTState {
    Ready = 0 // not yet run
    ,
    Pass = 1 // accepted
    ,
    Fail = 2 // rejected
    ,
    Exception = 3 // threw an exception
    ,
    N_STATES = 4
  };
} // namespace art::hlt

#endif /* canvas_Persistency_Common_HLTenums_h */

// Local Variables:
// mode: c++
// End:
