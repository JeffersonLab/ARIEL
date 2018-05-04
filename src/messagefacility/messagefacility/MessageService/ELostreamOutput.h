#ifndef messagefacility_MessageService_ELostreamOutput_h
#define messagefacility_MessageService_ELostreamOutput_h
// vim: set sw=2 expandtab :

#include "cetlib/ostream_handle.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/OptionalAtom.h"
#include "fhiclcpp/types/TableFragment.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/Utilities/ELextendedID.h"

#include <memory>
#include <string>

namespace mf {

  class ErrorObj;

} // namespace mf

namespace mf {
  namespace service {

    class ELostreamOutput : public ELdestination {

    public:
      struct Config {
        fhicl::TableFragment<ELdestination::Config> elDestConfig;
      };

      using Parameters = fhicl::WrappedTable<Config>;

    public:
      ~ELostreamOutput();
      ELostreamOutput(Parameters const& config,
                      cet::ostream_handle&&,
                      bool emitAtStart = false);
      ELostreamOutput(Parameters const& config,
                      std::ostream&,
                      bool emitAtStart = false);
      ELostreamOutput(Config const& config,
                      cet::ostream_handle&&,
                      bool emitAtStart = false);
      ELostreamOutput(ELostreamOutput const&) = delete;
      ELostreamOutput(ELostreamOutput&&) = delete;
      ELostreamOutput& operator=(ELostreamOutput const&) = delete;
      ELostreamOutput& operator=(ELostreamOutput&&) = delete;

    private:
      void routePayload(std::ostringstream const& oss,
                        mf::ErrorObj const& msg) override;

    private:
      cet::ostream_handle osh;
      mf::ELextendedID xid{};
    };

  } // namespace service
} // namespace mf

#endif /* messagefacility_MessageService_ELostreamOutput_h */

// Local variables:
// mode: c++
// End:
