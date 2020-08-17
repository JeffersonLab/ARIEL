# Set override flag to preserve dicionary behavior with older versions
# of art.
#
# Fixme: The service needs functions from toyexperiment_analyzers
#        Move these to a Utility library.
#
SET(CET_DICT_FUNCTIONS TRUE)

art_make(
   LIB_LIBRARIES
    ${ROOT_CORE}
   SERVICE_LIBRARIES
    art-workbook_EventDisplay3D
    art_framework_core
    art_framework_io_root_rootinput_source
    art_framework_io_root
    art_Framework_Principal
    mf_messagelogger
    toyexperiment_analyzers
    cetlib_except
    ${ROOT_EVE}
   MODULE_LIBRARIES
    art-workbook_EventDisplay3D_EvtDisplayService_service
    art-workbook_EventDisplay3D
    art_Persistency_Provenance
    toyexperiment_conditions_conditions_service
    toyexperiment_geometry_geometry_service
    toyexperiment_geometry
    toyexperiment_analyzers
    toyexperiment_pdt_pdt_service
    toyexperiment_pdt
    toyexperiment_mcdataproducts
    toyexperiment_recodataproducts
    toyexperiment_utilities
    ${ROOT_GUI}
    ${ROOT_GEOM}
    ${ROOT_EVE}
    ${ROOT_EG}
    ${ROOT_RGL}
  )
