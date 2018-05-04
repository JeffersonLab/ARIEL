#ifndef canvas_Persistency_Provenance_ProcessConfiguration_h
#define canvas_Persistency_Provenance_ProcessConfiguration_h

// ======================================================================
//
// ProcessConfiguration
//
// ======================================================================

#include "canvas/Persistency/Provenance/ProcessConfigurationID.h"
#include "canvas/Persistency/Provenance/ReleaseVersion.h"
#include "fhiclcpp/ParameterSetID.h"
#include <iosfwd>
#include <string>

namespace art {

  struct ProcessConfiguration {
    ProcessConfiguration() = default;

    ProcessConfiguration(std::string const& procName,
                         fhicl::ParameterSetID const& pSetID,
                         ReleaseVersion const& relVersion)
      : processName_{procName}
      , parameterSetID_{pSetID}
      , releaseVersion_{relVersion}
    {}

    std::string const&
    processName() const
    {
      return processName_;
    }
    fhicl::ParameterSetID const&
    parameterSetID() const
    {
      return parameterSetID_;
    }
    ReleaseVersion const&
    releaseVersion() const
    {
      return releaseVersion_;
    }
    ProcessConfigurationID id() const;

    std::string processName_{};
    fhicl::ParameterSetID parameterSetID_{};
    ReleaseVersion releaseVersion_{};
  };

  bool operator<(ProcessConfiguration const& a, ProcessConfiguration const& b);

  bool operator==(ProcessConfiguration const& a, ProcessConfiguration const& b);

  inline bool
  operator!=(ProcessConfiguration const& a, ProcessConfiguration const& b)
  {
    return !(a == b);
  }

  std::ostream& operator<<(std::ostream& os, ProcessConfiguration const& pc);

} // art

  // ======================================================================

#endif /* canvas_Persistency_Provenance_ProcessConfiguration_h */

// Local Variables:
// mode: c++
// End:
