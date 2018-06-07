
if(DEFINED cetlib_except_DIR)
art_make(
  LIB_LIBRARIES
    art_Persistency_Provenance
    canvas
    ${CLHEP}
  DICT_LIBRARIES
    toyExperiment_RecoDataProducts
    art_Persistency_Provenance
    art_Persistency_Common
    art_Utilities
    canvas
    cetlib cetlib_except
  )
else()
art_make(
  LIB_LIBRARIES
    art_Persistency_Provenance canvas_Persistency_Provenance
    ${CLHEP}
  DICT_LIBRARIES
    toyExperiment_RecoDataProducts
    art_Persistency_Provenance canvas_Persistency_Provenance
    art_Persistency_Common canvas_Persistency_Common
    art_Utilities
    cetlib
  )
endif()

install_headers()
install_source()
