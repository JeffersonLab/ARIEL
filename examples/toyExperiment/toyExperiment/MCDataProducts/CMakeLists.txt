
if(DEFINED cetlib_except_DIR)
art_make(
  LIB_LIBRARIES ${CLHEP}
    art_Persistency_Provenance
    art_Persistency_Common
    canvas
  DICT_LIBRARIES
    art_Persistency_Provenance
    art_Persistency_Common
    art_Utilities
    canvas
    cetlib cetlib_except
    toyExperiment_MCDataProducts
  )
else()
art_make(
  LIB_LIBRARIES ${CLHEP}
    art_Persistency_Provenance canvas_Persistency_Provenance
    art_Persistency_Common canvas_Persistency_Common
  DICT_LIBRARIES
    art_Persistency_Provenance canvas_Persistency_Provenance
    art_Persistency_Common canvas_Persistency_Common
    cetlib
    art_Utilities canvas_Utilities
    toyExperiment_MCDataProducts
  )
endif()

install_headers()
install_source()
