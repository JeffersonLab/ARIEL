# we install all the example files here
# All cmake files below this directory are part of the example

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/ToyCmake/ups/product_deps.in
                 ${CMAKE_CURRENT_BINARY_DIR}/ToyCmake/ups/product_deps @ONLY )

set( installUPSfiles ToyCmake/ups/CMakeLists.txt
                     ToyCmake/ups/product-config.cmake.in
		     ${CMAKE_CURRENT_BINARY_DIR}/ToyCmake/ups/product_deps
		     ToyCmake/ups/setup_for_development )

set( installTestFiles ToyCmake/test/CMakeLists.txt
                      ToyCmake/test/ToyCmakeTest.cc )

set( installMathFiles ToyCmake/ToyCmake/Math/CMakeLists.txt
                      ToyCmake/ToyCmake/Math/DoMath.h
		      ToyCmake/ToyCmake/Math/DoMath.cc  )

set( installMathIOFiles ToyCmake/ToyCmake/MathIO/CMakeLists.txt
                        ToyCmake/ToyCmake/MathIO/writeResult.h
		        ToyCmake/ToyCmake/MathIO/writeResult.cc  )

set( installSquareFiles ToyCmake/ToyCmake/Square/CMakeLists.txt
		        ToyCmake/ToyCmake/Square/SquareIt.cpp  )

INSTALL (FILES ToyCmake/CMakeLists.txt ToyCmake/README
         DESTINATION ${${product}_data_dir}/example/ToyCmake )

INSTALL (FILES ToyCmake/ToyCmake/CMakeLists.txt
         DESTINATION ${${product}_data_dir}/example/ToyCmake/ToyCmake )

INSTALL (FILES ToyCmake/ToyCmake/Hello/CMakeLists.txt
               ToyCmake/ToyCmake/Hello/Hello.cc
         DESTINATION ${${product}_data_dir}/example/ToyCmake/ToyCmake/Hello )

INSTALL (FILES ${installMathFiles}
         DESTINATION ${${product}_data_dir}/example/ToyCmake/ToyCmake/Math )

INSTALL (FILES ${installMathIOFiles}
         DESTINATION ${${product}_data_dir}/example/ToyCmake/ToyCmake/MathIO )

INSTALL (FILES ${installSquareFiles}
         DESTINATION ${${product}_data_dir}/example/ToyCmake/ToyCmake/Square )

INSTALL (FILES ${installTestFiles}
         DESTINATION ${${product}_data_dir}/example/ToyCmake/test )

INSTALL (FILES ${installUPSfiles}
         DESTINATION ${${product}_data_dir}/example/ToyCmake/ups )

