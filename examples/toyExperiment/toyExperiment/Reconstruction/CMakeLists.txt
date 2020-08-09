
if(DEFINED ENV{CETLIB_EXCEPT_DIR})
art_make(
    LIB_LIBRARIES toyExperiment_Geometry_Geometry_service
    toyExperiment_PDT_PDT_service
    toyExperiment_PDT
    toyExperiment_Geometry
    toyExperiment_RecoDataProducts
    toyExperiment_Utilities
    art_Framework_Services_Registry
    art_Utilities
    canvas
    cetlib cetlib_except
    ${Boost_SYSTEM_LIBRARY}
  MODULE_LIBRARIES
    toyExperiment_Conditions_Conditions_service
    toyExperiment_Geometry_Geometry_service
    toyExperiment_PDT_PDT_service
    toyExperiment_PDT
    toyExperiment_Utilities
    toyExperiment_MCDataProducts
    toyExperiment_RecoDataProducts
    art_Framework_Principal
    art_Persistency_Common
    art_Framework_Services_Registry
    art_Framework_Services_Optional_TFileService_service
    art_Framework_Services_Optional
    canvas
    fhiclcpp
    cetlib cetlib_except
  )
else()
art_make(
    LIB_LIBRARIES toyExperiment_Geometry_Geometry_service
    toyExperiment_PDT_PDT_service
    toyExperiment_PDT
    toyExperiment_Geometry
    toyExperiment_RecoDataProducts
    toyExperiment_Utilities
    art_Framework_Services_Registry
    art_Utilities canvas_Utilities
    cetlib
    ${Boost_SYSTEM_LIBRARY}
  MODULE_LIBRARIES
    toyExperiment_Conditions_Conditions_service
    toyExperiment_Geometry_Geometry_service
    toyExperiment_PDT_PDT_service
    toyExperiment_PDT
    toyExperiment_Utilities
    toyExperiment_MCDataProducts
    toyExperiment_RecoDataProducts
    art_Framework_Principal
    art_Persistency_Common canvas_Persistency_Common
    art_Framework_Services_Registry
    art_Framework_Services_Optional_TFileService_service
    art_Framework_Services_Optional
    fhiclcpp
    cetlib
  )
endif()

install_headers()
install_source()
