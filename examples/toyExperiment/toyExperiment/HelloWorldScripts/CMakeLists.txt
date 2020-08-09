file( GLOB files *.fcl *.cint *.sh inputs.txt helloExample.log )
install( FILES ${files}
         DESTINATION ${product}/${version}/HelloWorldScripts )

install_source()
