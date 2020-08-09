file( GLOB ROOT_MACROS_DO_NOT_BUILD RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.C )

art_make(
  EXCLUDE  ${ROOT_MACROS_DO_NOT_BUILD}
  LIB_LIBRARIES
    toyexperiment_pdt_pdt_service
    art_framework_services_registry
    canvas
    cetlib_except
    ${Boost_SYSTEM_LIBRARY}
  MODULE_LIBRARIES
    art-workbook_UsePDTService
    toyexperiment_pdt
    ${WORKBOOK_BASIC_ROOT_LIBRARIES}
  )
