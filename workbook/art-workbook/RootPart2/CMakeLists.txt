file( GLOB ROOT_MACROS_DO_NOT_BUILD RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.C )

# Fixme: Decide what to do with toyexperiment_mcdataproducts and ${CLHEP}
#        These are not needed since the code only uses inlined member functions.

art_make(
  EXCLUDE ${ROOT_MACROS_DO_NOT_BUILD}
  MODULE_LIBRARIES
    art_Persistency_Provenance
    toyexperiment_utilities
    cetlib_except
    ${WORKBOOK_BASIC_ROOT_LIBRARIES}
)
