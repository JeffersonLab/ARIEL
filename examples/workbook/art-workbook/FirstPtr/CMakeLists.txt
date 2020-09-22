file( GLOB ROOT_MACROS_DO_NOT_BUILD RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.C )

# Fixme: decide what to do with toyexperiment_mcdataproducts and ${CLHEP}
#        These are not needed because the code uses only inlined member functions

art_make(
  EXCLUDE  ${ROOT_MACROS_DO_NOT_BUILD}
  LIB_LIBRARIES
    canvas
    cetlib_except
  MODULE_LIBRARIES
    art-workbook_FirstPtr
    art_Persistency_Provenance
    toyexperiment_mcdataproducts
    toyexperiment_pdt_pdt_service
    ${WORKBOOK_BASIC_ROOT_LIBRARIES}
    ${CLHEP}
  )
