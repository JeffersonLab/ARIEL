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
    cetlib
    cetlib_except
  )

install_headers()
install_source()
