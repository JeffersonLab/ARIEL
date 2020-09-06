art_make(
  LIB_LIBRARIES
  toyExperiment_RecoDataProducts
  art_Persistency_Provenance
  art_Utilities
  ${CLHEP}
  canvas
  fhiclcpp
  cetlib cetlib_except
  ${Boost_SYSTEM_LIBRARY})

install_headers()
install_source()
