# Set override flag to preserve dicionary behavior with older versions
# of art.
#
# Fixme: The service needs functions from toyexperiment_analyzers
#        Move these to a Utility library.
#
SET(CET_DICT_FUNCTIONS TRUE)

art_make(
   LIB_LIBRARIES
    ROOT::Core
   SERVICE_LIBRARIES
    art-workbook_EventDisplay3D
    art_root_io_RootInput_source
    art_root_io
    art_Framework_Core
    art_Framework_Principal
    MF_MessageLogger
    toyExperiment_Analyzers
    cetlib_except
    ROOT::Eve
   MODULE_LIBRARIES
    art-workbook_EventDisplay3D_EvtDisplayService_service
    art-workbook_EventDisplay3D
    art_Persistency_Provenance
    toyExperiment_Conditions_Conditions_service
    toyExperiment_Geometry_Geometry_service
    toyExperiment_Geometry
    toyExperiment_Analyzers
    toyExperiment_PDT_PDT_service
    toyExperiment_PDT
    toyExperiment_MCDataProducts
    toyExperiment_RecoDataProducts
    toyExperiment_Utilities
    ROOT::Gui
    ROOT::Geom
    ROOT::Eve
    ROOT::EG
    ROOT::RGL
  )
