#ifndef canvas_Persistency_Common_AssnsBase_h
#define canvas_Persistency_Common_AssnsBase_h
// vim: set sw=2:

namespace art::detail {
  // The following base class is provided so that the ROOT I/O
  // system can appropriately handle transient data without having
  // to use a template, which would introduce more coupling of Assns
  // with ROOT than is desired.
  class AssnsBase {
  public:
    virtual ~AssnsBase() noexcept = default;
    virtual void fill_transients() = 0;
    virtual void fill_from_transients() = 0;
  };
}
#endif /* canvas_Persistency_Common_AssnsBase_h */

// Local Variables:
// mode: c++
// End:
