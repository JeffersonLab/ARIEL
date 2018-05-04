#ifndef canvas_Persistency_Provenance_ModuleDescription_h
#define canvas_Persistency_Provenance_ModuleDescription_h

/*----------------------------------------------------------------------

ModuleDescription: The description of a producer module.

----------------------------------------------------------------------*/

#include "canvas/Persistency/Provenance/ModuleDescriptionID.h"
#include "canvas/Persistency/Provenance/ProcessConfiguration.h"
#include "canvas/Persistency/Provenance/ProcessConfigurationID.h"
#include "fhiclcpp/ParameterSetID.h"
#include <iosfwd>
#include <string>

// ----------------------------------------------------------------------

namespace art {

  // once a module is born, these parts of the module's product provenance
  // are constant   (change to ModuleDescription)

  class ModuleDescription {
  public:
    explicit ModuleDescription() = default;
    explicit ModuleDescription(fhicl::ParameterSetID parameterSetID,
                               std::string const& modName,
                               std::string const& modLabel,
                               ProcessConfiguration pc,
                               ModuleDescriptionID id = getUniqueID());

    // Feel free to use move semantics.

    void write(std::ostream& os) const;

    fhicl::ParameterSetID const&
    parameterSetID() const
    {
      return parameterSetID_;
    }
    std::string const&
    moduleName() const
    {
      return moduleName_;
    }
    std::string const&
    moduleLabel() const
    {
      return moduleLabel_;
    }
    ProcessConfiguration const&
    processConfiguration() const
    {
      return processConfiguration_;
    }
    ProcessConfigurationID const
    processConfigurationID() const
    {
      return processConfiguration().id();
    }
    std::string const&
    processName() const
    {
      return processConfiguration().processName();
    }
    std::string const&
    releaseVersion() const
    {
      return processConfiguration().releaseVersion();
    }
    fhicl::ParameterSetID const&
    mainParameterSetID() const
    {
      return processConfiguration().parameterSetID();
    }

    bool operator<(ModuleDescription const& rh) const;
    bool operator==(ModuleDescription const& rh) const;
    bool operator!=(ModuleDescription const& rh) const;

    ModuleDescriptionID
    id() const
    {
      return id_;
    } // Unique only within a process.

    static ModuleDescriptionID getUniqueID();

    static constexpr ModuleDescriptionID
    invalidID()
    {
      return std::numeric_limits<ModuleDescriptionID>::max();
    }

  private:
    // ID of parameter set of the creator
    fhicl::ParameterSetID parameterSetID_{};

    // The class name of the creator
    std::string moduleName_{};

    // A human friendly string that uniquely identifies the EDProducer
    // and becomes part of the identity of a product that it produces
    std::string moduleLabel_{};

    // The process configuration.
    ProcessConfiguration processConfiguration_{};

    // Unique ID.
    ModuleDescriptionID id_{invalidID()};
  };

  inline std::ostream&
  operator<<(std::ostream& os, ModuleDescription const& p)
  {
    p.write(os);
    return os;
  }

} // art

  // ======================================================================

#endif /* canvas_Persistency_Provenance_ModuleDescription_h */

// Local Variables:
// mode: c++
// End:
