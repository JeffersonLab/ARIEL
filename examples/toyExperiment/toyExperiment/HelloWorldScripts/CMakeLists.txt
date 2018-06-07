file( GLOB files *.fcl *.cint *.sh inputs.txt helloExample.log )
install( FILES ${files}
         DESTINATION ${${product}_data_dir}/HelloWorldScripts )

install_source()
