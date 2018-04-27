
set( MathIO_sources writeResult.cc )

# This example builds both static and shared libraries
add_library(ToyMathIO   SHARED ${MathIO_sources} )
add_library(ToyMathIO_s STATIC ${MathIO_sources} )
# the resulting libraries have the same base name
set_target_properties( ToyMathIO   PROPERTIES OUTPUT_NAME ToyMathIO )
set_target_properties( ToyMathIO_s PROPERTIES OUTPUT_NAME ToyMathIO )
# avoid stepping on toes when building both
set_target_properties(ToyMathIO   PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(ToyMathIO_s PROPERTIES CLEAN_DIRECT_OUTPUT 1)

install ( TARGETS ToyMathIO ToyMathIO_s DESTINATION ${flavorqual_dir}/lib )

install_headers()
install_source()

