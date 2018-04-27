

# how to link with boost and other system libraries
#link_libraries( ${Boost_FILESYSTEM_LIBRARY}
#                ${REFLEX}
#                dl )

cet_make( LIBRARY_NAME ToyMath )

install_headers()
install_source()
