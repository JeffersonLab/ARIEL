cet_script( fhicl-diff )

add_subdirectory(detail)

art_make_library( LIBRARY_NAME art-workbook_GoodWorkflow
                  SOURCE
                  detail/tidbits.cc
                )

art_make_exec( fhicl-dump 
               LIBRARIES
               ${CETLIB}
               ${FHICLCPP}
               Boost::program_options
)
simple_plugin(Producer "module" art-workbook_GoodWorkflow)
