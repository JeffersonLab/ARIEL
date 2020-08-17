#include "canvas/Persistency/Provenance/ProcessConfiguration.h"
// vim: set sw=2 expandtab :

#include "cetlib/MD5Digest.h"

#include <ostream>
#include <sstream>
#include <tuple>

using namespace std;

namespace art {

  ProcessConfiguration::~ProcessConfiguration() = default;
  ProcessConfiguration::ProcessConfiguration() noexcept = default;

  ProcessConfiguration::ProcessConfiguration(
    string const& procName,
    fhicl::ParameterSetID const& pSetID,
    ReleaseVersion const& relVersion) noexcept(false)
    : processName_{procName}
    , parameterSetID_{pSetID}
    , releaseVersion_{relVersion}
  {}

  ProcessConfiguration::ProcessConfiguration(
    ProcessConfiguration const&) noexcept(false) = default;
  ProcessConfiguration::ProcessConfiguration(ProcessConfiguration&&) = default;

  ProcessConfiguration& ProcessConfiguration::operator=(
    ProcessConfiguration const& rhs) noexcept(false) = default;

  ProcessConfiguration& ProcessConfiguration::operator=(
    ProcessConfiguration&& rhs) = default;

  string const&
  ProcessConfiguration::processName() const noexcept
  {
    return processName_;
  }

  fhicl::ParameterSetID const&
  ProcessConfiguration::parameterSetID() const noexcept
  {
    return parameterSetID_;
  }

  ReleaseVersion const&
  ProcessConfiguration::releaseVersion() const noexcept
  {
    return releaseVersion_;
  }

  ProcessConfigurationID
  ProcessConfiguration::id() const
  {
    ostringstream oss;
    oss << *this;
    string stringrep = oss.str();
    cet::MD5Digest md5alg(stringrep);
    return ProcessConfigurationID(md5alg.digest().toString());
  }

  bool
  operator!=(ProcessConfiguration const& a, ProcessConfiguration const& b)
  {
    return !(a == b);
  }

  bool
  operator<(ProcessConfiguration const& a, ProcessConfiguration const& b)
  {
    return std::tie(a.processName(), a.parameterSetID(), a.releaseVersion()) <
           std::tie(b.processName(), b.parameterSetID(), b.releaseVersion());
  }

  bool
  operator==(ProcessConfiguration const& a, ProcessConfiguration const& b)
  {
    return std::tie(a.processName(), a.parameterSetID(), a.releaseVersion()) ==
           std::tie(b.processName(), b.parameterSetID(), b.releaseVersion());
  }

  std::ostream&
  operator<<(std::ostream& os, ProcessConfiguration const& pc)
  {
    os << pc.processName() << ' ' << pc.parameterSetID() << ' '
       << pc.releaseVersion()
       << ' '; // Retain the last space for backwards compatibility
    return os;
  }

} // namespace art
