
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/product_deps.template.in
                 ${CMAKE_CURRENT_BINARY_DIR}/product_deps.template @ONLY )

INSTALL (FILES ${CMAKE_CURRENT_BINARY_DIR}/product_deps.template
               README
	       product-config.cmake.in.template
	       setup_for_development.noarch.template
               setup_for_development.template
	       setup_deps.template
         DESTINATION ${${product}_data_dir}/templates )
