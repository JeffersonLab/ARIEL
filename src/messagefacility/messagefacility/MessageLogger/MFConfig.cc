#include "messagefacility/MessageLogger/MFConfig.h"
// vim: set sw=2 expandtab :

#include "fhiclcpp/types/ConfigurationTable.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/TableFragment.h"

#include <string>

using namespace std;

namespace mf {

  MFConfig::Config::~Config() {}

  MFConfig::Config::Config()
    : debugModules{fhicl::Name{"debugModules"},
                   fhicl::Comment{
                     "The 'debugModules' parameter is a sequence of strings.\n"
                     "Ignored. Accepted for backwards-compatibility only."s},
                   fhicl::Sequence<string>::default_type{}}
    , suppressDebug{fhicl::Name{"suppressDebug"},
                    fhicl::Comment{
                      "The 'suppressDebug' parameter is a sequence of "
                      "strings.\n"
                      "Ignored. Accepted for backwards-compatibility only."s},
                    fhicl::Sequence<string>::default_type{}}
    , suppressInfo{fhicl::Name{"suppressInfo"},
                   fhicl::Comment{
                     "The 'suppressInfo' parameter is a sequence of strings.\n"
                     "Ignored. Accepted for backwards-compatibility only."s},
                   fhicl::Sequence<string>::default_type{}}
    , suppressWarning{fhicl::Name{"suppressWarning"},
                      fhicl::Comment{
                        "The 'suppressWarning' parameter is a sequence of "
                        "strings.\n"
                        "Ignored. Accepted for backwards-compatibility only."s},
                      fhicl::Sequence<string>::default_type{}}
    , destinations{}
  {}

  MFConfig::~MFConfig() {}

  MFConfig::MFConfig() {}

  MFDestinationConfig::Config::~Config() {}

  MFDestinationConfig::Config::Config()
    : destinations{
        fhicl::Name{"destinations"},
        fhicl::Comment{
          R"(The 'destinations' parameter represents a FHiCL table of named
destinations, each of which are configured to specify how messages
are logged to a given target.  It has the general form of:

  destinations: {
    // Ordinary destinations
    dest1 : {...}
    dest2 : {...}
    ...
    statistics: { // optional
      // Statistics destinations
      stat1: {...}
      ...
    }
  }

For a listing of allowed ordinary destinations, type:

  art --print-available mfPlugin

The allowed configuration for a given destination type can be printed
by specifying:

  art --print-description mfPlugin:<destination type>

It is permitted to specify an ordinary destination called
'statistics'. The 'statistics' destination is a FHiCL table that has
named statistics destinations, which can be used to encapsulate the
configuration related to statistics-tracking of messages logged to a
each ordinary destination.  To print out the allowed statistics
destinations and the allowed configuration corresponding to a given
statistics destination, replace the 'mfPlugin' specification with
'mfStatsPlugin'.

If a value for 'destinations' is not supplied, one will be provided for you.)"}}
  {}

} // namespace mf
