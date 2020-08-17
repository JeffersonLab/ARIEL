simple_plugin(EventGenerator "module"
              toyExperiment_Conditions_Conditions_service
              toyExperiment_PDT_PDT_service
              toyExperiment_Utilities
              toyExperiment_MCDataProducts)

simple_plugin(DetectorSimulation "module"
              toyExperiment_Conditions_Conditions_service
              toyExperiment_PDT_PDT_service
              toyExperiment_Geometry_Geometry_service
              toyExperiment_Utilities
              toyExperiment_MCDataProducts
              toyExperiment_RecoDataProducts
              art_Framework_Services_Registry
              ${TFILE_SERVICE_LIBRARIES})

simple_plugin(HitMaker "module"
              toyExperiment_Conditions_Conditions_service
              toyExperiment_Geometry_Geometry_service
              toyExperiment_Utilities
              toyExperiment_MCDataProducts
              toyExperiment_RecoDataProducts
              art_Framework_Services_Registry
              ${TFILE_SERVICE_LIBRARIES})

install_headers()
install_source()
