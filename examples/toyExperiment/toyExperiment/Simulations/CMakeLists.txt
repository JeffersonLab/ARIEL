
if(DEFINED cetlib_except_DIR)
simple_plugin( EventGenerator "module"
               toyExperiment_Conditions_Conditions_service
               toyExperiment_PDT_PDT_service
               toyExperiment_Utilities
               toyExperiment_MCDataProducts
               ${ART_FRAMEWORK_CORE}
               art_Framework_Principal
               art_Persistency_Common
               canvas
             )

simple_plugin( DetectorSimulation "module"
               toyExperiment_Conditions_Conditions_service
               toyExperiment_PDT_PDT_service
               toyExperiment_Geometry_Geometry_service
               toyExperiment_Utilities
               toyExperiment_MCDataProducts
               toyExperiment_RecoDataProducts
               ${ART_FRAMEWORK_CORE}
               art_Framework_Principal
               art_Persistency_Common
               art_Framework_Services_Registry
               art_Framework_Services_Optional_TFileService_service
               art_Framework_Services_Optional
               canvas
               fhiclcpp
               cetlib cetlib_except
             )

simple_plugin( HitMaker "module"
               toyExperiment_Conditions_Conditions_service
               toyExperiment_Geometry_Geometry_service
               toyExperiment_Utilities
               toyExperiment_MCDataProducts
               toyExperiment_RecoDataProducts
               ${ART_FRAMEWORK_CORE}
               art_Framework_Principal
               art_Persistency_Common
               art_Framework_Services_Registry
               art_Framework_Services_Optional_TFileService_service
               art_Framework_Services_Optional
               canvas
               fhiclcpp
               cetlib cetlib_except
             )
else()
simple_plugin( EventGenerator "module"
               toyExperiment_Conditions_Conditions_service
               toyExperiment_PDT_PDT_service
               toyExperiment_Utilities
               toyExperiment_MCDataProducts
               ${ART_FRAMEWORK_CORE}
               art_Framework_Principal
               art_Persistency_Common canvas_Persistency_Common
             )

simple_plugin( DetectorSimulation "module"
               toyExperiment_Conditions_Conditions_service
               toyExperiment_PDT_PDT_service
               toyExperiment_Geometry_Geometry_service
               toyExperiment_Utilities
               toyExperiment_MCDataProducts
               toyExperiment_RecoDataProducts
               ${ART_FRAMEWORK_CORE}
               art_Framework_Principal
               art_Persistency_Common canvas_Persistency_Common
               art_Framework_Services_Registry
               art_Framework_Services_Optional_TFileService_service
               art_Framework_Services_Optional
               fhiclcpp
               cetlib
             )

simple_plugin( HitMaker "module"
               toyExperiment_Conditions_Conditions_service
               toyExperiment_Geometry_Geometry_service
               toyExperiment_Utilities
               toyExperiment_MCDataProducts
               toyExperiment_RecoDataProducts
               ${ART_FRAMEWORK_CORE}
               art_Framework_Principal
               art_Persistency_Common canvas_Persistency_Common
               art_Framework_Services_Registry
               art_Framework_Services_Optional_TFileService_service
               art_Framework_Services_Optional
               fhiclcpp
               cetlib
             )
endif()

install_headers()
install_source()
