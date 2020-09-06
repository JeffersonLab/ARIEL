#ifndef canvas_Persistency_Common_HLTGlobalStatus_h
#define canvas_Persistency_Common_HLTGlobalStatus_h
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Common/HLTPathStatus.h"
#include "canvas/Persistency/Common/HLTenums.h"

#include <atomic>
#include <cstddef>
#include <ostream>
#include <vector>

namespace art {
  class HLTGlobalStatus {
  public:
    ~HLTGlobalStatus();
    explicit HLTGlobalStatus(std::size_t n = 0);
    HLTGlobalStatus(HLTGlobalStatus const&);
    HLTGlobalStatus(HLTGlobalStatus&&);
    HLTGlobalStatus& operator=(HLTGlobalStatus const&);
    HLTGlobalStatus& operator=(HLTGlobalStatus&&);

  public:
    std::size_t size() const;
    void reset();
    bool wasrun() const;
    bool accept() const;
    bool error() const;
    HLTPathStatus const& at(unsigned const i) const;
    HLTPathStatus& at(unsigned const i);
    HLTPathStatus const& operator[](unsigned const i) const;
    HLTPathStatus& operator[](unsigned const i);
    bool wasrun(unsigned const i) const;
    bool accept(unsigned const i) const;
    bool error(unsigned const i) const;
    hlt::HLTState state(unsigned const i) const;
    unsigned index(unsigned const i) const;
    void reset(unsigned const i);

  private:
    std::vector<HLTPathStatus> paths_;
  };
  std::ostream& operator<<(std::ostream& ost, HLTGlobalStatus const& hlt);
} // namespace art

#endif /* canvas_Persistency_Common_HLTGlobalStatus_h */

// Local Variables:
// mode: c++
// End:
