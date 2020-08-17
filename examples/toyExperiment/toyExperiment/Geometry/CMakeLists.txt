art_make(
  LIB_LIBRARIES
    toyExperiment_Utilities
    fhiclcpp
    cetlib
    cetlib_except
    ${CLHEP}
  SERVICE_LIBRARIES
    toyExperiment_Geometry
    toyExperiment_Utilities
    art_Framework_Principal
    art_Persistency_Provenance
    ${CLHEP})

install_headers()
install_source()
