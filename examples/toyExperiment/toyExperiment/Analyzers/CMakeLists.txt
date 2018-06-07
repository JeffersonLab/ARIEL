art_make_library(SOURCE EnsureTApplication.cc PlotOrientation.cc
                        WaitFor.cc
   LIBRARIES
   ${ROOT_CORE}
  )

if(DEFINED cetlib_except_DIR)
set(COMMON_LIBRARIES
  art_Framework_Core
  art_Framework_Principal
  art_Persistency_Common
  art_Framework_Services_Registry
  art_Framework_Services_Optional_TFileService_service
  art_Framework_Services_Optional
  canvas
  fhiclcpp
  cetlib cetlib_except
  ${CLHEP}
)
else()
set(COMMON_LIBRARIES
  art_Framework_Core
  art_Framework_Principal
  art_Persistency_Common canvas_Persistency_Common
  art_Framework_Services_Registry
  art_Framework_Services_Optional_TFileService_service
  art_Framework_Services_Optional
  fhiclcpp
  cetlib
  ${CLHEP}
)
endif()

if(DEFINED cetlib_except_DIR)
simple_plugin( HelloWorld "module"
  art_Framework_Core
  art_Framework_Principal
  art_Persistency_Common
  canvas
  )
else()
simple_plugin( HelloWorld "module"
  art_Framework_Core
  art_Framework_Principal
  art_Persistency_Common canvas_Persistency_Common
  )
endif()

simple_plugin( InspectGenParticles "module"
  toyExperiment_PDT_PDT_service
  toyExperiment_PDT
  toyExperiment_MCDataProducts
  toyExperiment_Conditions_Conditions_service
  ${COMMON_LIBRARIES}
  ${ROOT_HIST}
  ${ROOT_TREE}
  )

simple_plugin( InspectIntersections "module"
  toyExperiment_PDT_PDT_service
  toyExperiment_PDT
  toyExperiment_MCDataProducts
  toyExperiment_Utilities
  ${COMMON_LIBRARIES}
  ${ROOT_HIST}
  ${ROOT_TREE}
  )

simple_plugin( InspectTrkHits "module"
  toyExperiment_RecoDataProducts
  ${COMMON_LIBRARIES}
  ${ROOT_HIST}
  ${ROOT_TREE}
  )

simple_plugin( InspectFittedHelix "module"
  toyExperiment_RecoDataProducts
  ${COMMON_LIBRARIES}
  ${ROOT_HIST}
  ${ROOT_TREE}
  )

simple_plugin( MassPlot "module"
  toyExperiment_PDT_PDT_service
  toyExperiment_PDT
  toyExperiment_Reconstruction
  toyExperiment_RecoDataProducts
  ${COMMON_LIBRARIES}
  ${ROOT_HIST}
  ${ROOT_TREE}
  )

simple_plugin( EventDisplay "module"
  toyExperiment_Conditions_Conditions_service
  toyExperiment_Geometry_Geometry_service
  toyExperiment_PDT_PDT_service
  toyExperiment_PDT
  toyExperiment_MCDataProducts
  toyExperiment_Analyzers
  toyExperiment_Utilities
  ${COMMON_LIBRARIES}
  MF_MessageLogger
  ${ROOT_GRAF}
  ${ROOT_GPAD}
  ${ROOT_HIST}
  ${ROOT_CORE}
  )

install_headers()
install_source()
