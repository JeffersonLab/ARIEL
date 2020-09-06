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
    cetlib
    cetlib_except
    toyExperiment_MCDataProducts)

install_headers()
install_source()
