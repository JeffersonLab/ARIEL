# Fixme:
#  1) do I need both add_subdirectory and the SUBDIRS clause in art_make
#  2) CMakeLists.txt files under answers are not tested for art v2

add_subdirectory(detail)

art_make(
  LIB_LIBRARIES
    art-workbook_ExUtilities
    ${TOYEXPERIMENT_RECODATAPRODUCTS}
    ${TOYEXPERIMENT_PDT}
    ${TOYEXPERIMENT_UTILITIES}
    ${CANVAS}
    ${CETLIB}
    ${CETLIB_EXCEPT}
    ${CLHEP}

  MODULE_LIBRARIES
    art-workbook_ExDataProducts
    art-workbook_IterativeAlgDevel
    art-workbook_ExUtilities
    ${ART_PERSISTENCY_PROVENANCE}
    ${TOYEXPERIMENT_RECODATAPRODUCTS}
    ${TOYEXPERIMENT_RECONSTRUCTION}
    ${TOYEXPERIMENT_CONDITIONS_CONDITIONS_SERVICE}
    ${TOYEXPERIMENT_CONDITIONS}
    ${TOYEXPERIMENT_PDT_PDT_SERVICE}
    ${TOYEXPERIMENT_PDT}
    ${CETLIB_EXCEPT}
    ${WORKBOOK_BASIC_ROOT_LIBRARIES}

  SUBDIRS
    detail
 )
