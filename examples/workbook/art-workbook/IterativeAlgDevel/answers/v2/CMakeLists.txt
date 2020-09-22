add_subdirectory(detail)

art_make(

  LIB_LIBRARIES
  art-workbook_ExUtilities
  ${FHICLCPP}
  ${CETLIB}
  art_Persistency_Provenance canvas_Persistency_Provenance
  ${CLHEP}
  ${TOYEXPERIMENT_DATAPRODUCTS}
  ${TOYEXPERIMENT_PDT}
  ${TOYEXPERIMENT_RECODATAPRODUCTS}
  ${TOYEXPERIMENT_RECONSTRUCTION}
  ${TOYEXPERIMENT_UTILITIES}

  MODULE_LIBRARIES
  art-workbook_ExDataProducts
  art-workbook_IterativeAlgDevel
  art-workbook_ExUtilities
  ${TOYEXPERIMENT_RECODATAPRODUCTS}
  ${TOYEXPERIMENT_RECONSTRUCTION}
  ${TOYEXPERIMENT_CONDITIONS_CONDITIONS_SERVICE}
  ${TOYEXPERIMENT_CONDITIONS}
  ${TOYEXPERIMENT_PDT_PDT_SERVICE}
  ${TOYEXPERIMENT_PDT}
  ${ART_FRAMEWORK_CORE}
  ${ART_FRAMEWORK_PRINCIPAL}
  art_Persistency_Common canvas_Persistency_Common
  ${ART_FRAMEWORK_SERVICES_REGISTRY}
  ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
  ${ART_FRAMEWORK_SERVICES_OPTIONAL}
  ${FHICLCPP}
  ${CETLIB}
  ${ROOT_HIST}
  ${ROOT_TREE}

  EXCLUDE
  TestRecoTrk.cc
  TestRecoTrk_answer.cc

  SUBDIRS
  detail
 )

include(CetTest)

cet_test(TestRecoTrk_answer
  LIBRARIES art-workbook_IterativeAlgDevel
  OPTIONAL_GROUPS IterativeAlgDevel
  )

