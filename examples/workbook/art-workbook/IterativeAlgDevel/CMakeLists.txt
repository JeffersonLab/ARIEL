# Fixme:
#  1) do I need both add_subdirectory and the SUBDIRS clause in art_make
#  2) CMakeLists.txt files under answers are not tested for art v2

add_subdirectory(detail)

art_make(
  LIB_LIBRARIES
    art-workbook_ExUtilities
    toyexperiment_recodataproducts
    toyexperiment_pdt
    toyexperiment_utilities
    canvas
    cetlib
    cetlib_except
    ${CLHEP}

  MODULE_LIBRARIES
    art-workbook_ExDataProducts
    art-workbook_IterativeAlgDevel
    art-workbook_ExUtilities
    art_Persistency_Provenance
    toyexperiment_recodataproducts
    toyexperiment_reconstruction
    toyexperiment_conditions_conditions_service
    toyexperiment_conditions
    toyexperiment_pdt_pdt_service
    toyexperiment_pdt
    cetlib_except
    ${WORKBOOK_BASIC_ROOT_LIBRARIES}

  SUBDIRS
    detail
 )
