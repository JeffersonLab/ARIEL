art_make_library(SOURCE EnsureTApplication.cc PlotOrientation.cc
                        WaitFor.cc
   LIBRARIES
   ROOT::Core
  )

set(COMMON_LIBRARIES
    art_Framework_Services_Registry
    ${TFILE_SERVICE_LIBRARIES}
    ${CLHEP}
    ROOT::Hist)

simple_plugin(HelloWorld "module")

simple_plugin(InspectGenParticles "module"
              toyExperiment_PDT_PDT_service
              toyExperiment_PDT
              toyExperiment_MCDataProducts
              toyExperiment_Conditions_Conditions_service
              ${COMMON_LIBRARIES}
              ROOT::Tree)

simple_plugin(InspectIntersections "module"
              toyExperiment_PDT_PDT_service
              toyExperiment_PDT
              toyExperiment_MCDataProducts
              toyExperiment_Utilities
              ${COMMON_LIBRARIES}
              ROOT::Tree)

simple_plugin(InspectTrkHits "module"
              toyExperiment_RecoDataProducts
              ${COMMON_LIBRARIES}
              ROOT::Tree)

simple_plugin(InspectFittedHelix "module"
              toyExperiment_RecoDataProducts
              ${COMMON_LIBRARIES}
              ROOT::Tree)

simple_plugin(MassPlot "module"
              toyExperiment_PDT_PDT_service
              toyExperiment_PDT
              toyExperiment_Reconstruction
              toyExperiment_RecoDataProducts
              ${COMMON_LIBRARIES}
              ROOT::Tree)

simple_plugin(EventDisplay "module"
              toyExperiment_Conditions_Conditions_service
              toyExperiment_Geometry_Geometry_service
              toyExperiment_PDT_PDT_service
              toyExperiment_PDT
              toyExperiment_MCDataProducts
              toyExperiment_Analyzers
              toyExperiment_Utilities
              ${COMMON_LIBRARIES}
              MF_MessageLogger
              ROOT::Graf
              ROOT::Gpad)

install_headers()
install_source()
