#ifndef messagefacility_MessageService_ELstatistics_h
#define messagefacility_MessageService_ELstatistics_h
// vim: set sw=2 expandtab :

#include "cetlib/ostream_handle.h"
#include "fhiclcpp/types/ConfigurationTable.h"
#include "fhiclcpp/types/TableFragment.h"
#include "messagefacility/MessageService/ELdestination.h"

namespace mf::service {

  class ELstatistics : public ELdestination {
  public:
    struct Config {
      fhicl::TableFragment<ELdestination::Config> elDestConfig;
    };
    using Parameters = fhicl::WrappedTable<Config>;

    ~ELstatistics();
    ELstatistics(Parameters const&, std::ostream&);
    ELstatistics(Config const&, cet::ostream_handle&&);
    ELstatistics(ELstatistics const&) = delete;
    ELstatistics(ELstatistics&&) = delete;
    ELstatistics& operator=(ELstatistics const&) = delete;
    ELstatistics& operator=(ELstatistics&&) = delete;

  private:
    void log(mf::ErrorObj&) override;

    // Called only by MessageLoggerScribe::summarize()
    //   Called only by MessageLogger::LogStatistics()
    void summary() override;

    cet::ostream_handle osh_;
  };

} // namespace mf::service

#endif /* messagefacility_MessageService_ELstatistics_h */

// Local variables:
// mode: c++
// End:
