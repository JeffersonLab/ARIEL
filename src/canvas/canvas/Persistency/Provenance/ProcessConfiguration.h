#ifndef canvas_Persistency_Provenance_ProcessConfiguration_h
#define canvas_Persistency_Provenance_ProcessConfiguration_h
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/ProcessConfigurationID.h"
#include "canvas/Persistency/Provenance/ReleaseVersion.h"
#include "fhiclcpp/ParameterSetID.h"

#include <iosfwd>
#include <string>
#include <vector>

namespace art {

  class ProcessConfiguration {
  public:
    ~ProcessConfiguration();

    ProcessConfiguration() noexcept;

    // The non-default constructor cannot be noexcept because the
    // non-default ParameterSetID c'tor can throw.
    ProcessConfiguration(std::string const& name,
                         fhicl::ParameterSetID const&,
                         ReleaseVersion const&) noexcept(false);
    ProcessConfiguration(ProcessConfiguration const&) noexcept(false);
    ProcessConfiguration(ProcessConfiguration&&) noexcept(false);

    ProcessConfiguration& operator=(ProcessConfiguration const&);
    ProcessConfiguration& operator=(ProcessConfiguration&&);

    std::string const& processName() const noexcept;
    fhicl::ParameterSetID const& parameterSetID() const noexcept;
    ReleaseVersion const& releaseVersion() const noexcept;
    ProcessConfigurationID id() const;

  private:
    std::string processName_{};
    fhicl::ParameterSetID parameterSetID_{};
    ReleaseVersion releaseVersion_{};
  };

  bool operator<(ProcessConfiguration const& a, ProcessConfiguration const& b);
  bool operator==(ProcessConfiguration const& a, ProcessConfiguration const& b);
  bool operator!=(ProcessConfiguration const& a, ProcessConfiguration const& b);

  std::ostream& operator<<(std::ostream& os, ProcessConfiguration const& pc);

  using ProcessConfigurations = std::vector<ProcessConfiguration>;

} // namespace art

#endif /* canvas_Persistency_Provenance_ProcessConfiguration_h */

// Local Variables:
// mode: c++
// End:
