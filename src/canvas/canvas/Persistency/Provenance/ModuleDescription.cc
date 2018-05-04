#include "canvas/Persistency/Provenance/ModuleDescription.h"
#include "cetlib/MD5Digest.h"

#include <atomic>
#include <ostream>

art::ModuleDescription::ModuleDescription(
  fhicl::ParameterSetID const parameterSetID,
  std::string const& modName,
  std::string const& modLabel,
  ProcessConfiguration pc,
  ModuleDescriptionID const id)
  : parameterSetID_{parameterSetID}
  , moduleName_{modName}
  , moduleLabel_{modLabel}
  , processConfiguration_{std::move(pc)}
  , id_{id}
{}

bool
art::ModuleDescription::operator<(ModuleDescription const& rh) const
{
  if (moduleLabel() < rh.moduleLabel())
    return true;
  if (rh.moduleLabel() < moduleLabel())
    return false;
  if (processName() < rh.processName())
    return true;
  if (rh.processName() < processName())
    return false;
  if (moduleName() < rh.moduleName())
    return true;
  if (rh.moduleName() < moduleName())
    return false;
  if (parameterSetID() < rh.parameterSetID())
    return true;
  if (rh.parameterSetID() < parameterSetID())
    return false;
  if (releaseVersion() < rh.releaseVersion())
    return true;
  return false;
}

bool
art::ModuleDescription::operator==(ModuleDescription const& rh) const
{
  return !((*this) < rh || rh < (*this));
}

bool
art::ModuleDescription::operator!=(ModuleDescription const& rh) const
{
  return !operator==(rh);
}

void
art::ModuleDescription::write(std::ostream& os) const
{
  os << "Module type=" << moduleName() << ", "
     << "Module label=" << moduleLabel() << ", "
     << "Parameter Set ID=" << parameterSetID() << ", "
     << "Process name=" << processName() << ", "
     << "Release Version=" << releaseVersion() << ", "
     << "Main Parameter Set ID=" << mainParameterSetID();
}

static std::atomic<art::ModuleDescriptionID> s_id{0u};

art::ModuleDescriptionID
art::ModuleDescription::getUniqueID()
{
  return ++s_id;
}
