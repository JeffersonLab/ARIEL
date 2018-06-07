
if(DEFINED cetlib_except_DIR)
art_make(
  LIB_LIBRARIES
  toyExperiment_RecoDataProducts
  art_Persistency_Provenance
  art_Utilities
  ${CLHEP}
  canvas
  fhiclcpp
  cetlib cetlib_except
  ${Boost_SYSTEM_LIBRARY}
  )
else()
art_make(
  LIB_LIBRARIES
  toyExperiment_RecoDataProducts
  art_Persistency_Provenance canvas_Persistency_Provenance
  art_Utilities canvas_Utilities
  ${CLHEP}
  fhiclcpp
  cetlib
  ${Boost_SYSTEM_LIBRARY}
  )
endif()

install_headers()
install_source()
