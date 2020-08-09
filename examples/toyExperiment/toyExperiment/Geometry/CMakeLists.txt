
if(DEFINED ENV{CETLIB_EXCEPT_DIR})
art_make(
  LIB_LIBRARIES
    toyExperiment_Utilities
    fhiclcpp
    cetlib cetlib_except
    ${CLHEP}
  SERVICE_LIBRARIES
    toyExperiment_Geometry
    toyExperiment_Utilities
    art_Persistency_Provenance
    canvas
    fhiclcpp
    cetlib cetlib_except
    ${CLHEP}
  )
else()
art_make(
  LIB_LIBRARIES
    toyExperiment_Utilities
    fhiclcpp
    cetlib
    ${CLHEP}
  SERVICE_LIBRARIES
    toyExperiment_Geometry
    toyExperiment_Utilities
    art_Persistency_Provenance canvas_Persistency_Provenance
    fhiclcpp
    cetlib
    ${CLHEP}
  )
endif()

install_headers()
install_source()
