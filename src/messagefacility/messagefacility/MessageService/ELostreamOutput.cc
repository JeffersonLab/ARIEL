#include "messagefacility/MessageService/ELostreamOutput.h"
// vim: set sw=2 expandtab :

#include "messagefacility/Utilities/ErrorObj.h"

using namespace std;

namespace mf::service {

  ELostreamOutput::~ELostreamOutput() = default;

  ELostreamOutput::ELostreamOutput(Parameters const& ps,
                                   ostream& os,
                                   bool const emitAtStart)
    : ELostreamOutput{ps, cet::ostream_handle{os}, emitAtStart}
  {}

  ELostreamOutput::ELostreamOutput(Parameters const& ps,
                                   cet::ostream_handle&& h,
                                   bool const emitAtStart)
    : ELostreamOutput{ps(), move(h), emitAtStart}
  {}

  ELostreamOutput::ELostreamOutput(Config const& config,
                                   cet::ostream_handle&& h,
                                   bool const emitAtStart)
    : ELdestination{config.elDestConfig()}, osh{move(h)}
  {
    if (emitAtStart) {
      emitToken(
        osh, "\n=================================================", true, true);
      emitToken(osh,
                "\nMessage Log File written by MessageLogger service\n",
                false,
                true);
      emitToken(osh,
                "\n=================================================\n",
                true,
                true);
    }
  }

  void
  ELostreamOutput::routePayload(ostringstream const& oss, mf::ErrorObj const&)
  {
    osh << oss.str();
    osh.flush();
  }

} // namespace mf::service
