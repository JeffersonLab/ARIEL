# ======================================================================
#
# Build/install script
#
# ======================================================================

include(CetMake)

# ----------------------------------------------------------------------
# Build and install this project's library:

set(LIB_NAME ${PROJECT_NAME})

cet_make_library(LIBRARY_NAME ${LIB_NAME}
  SOURCE
  demangle.cc
  exception.cc
  exception_collector.cc
  )


# ----------------------------------------------------------------------
# Build and install this project's executables:

# ----------------------------------------------------------------------
# Install this project's headers and sources:

install_headers()
install_source()

#
# ======================================================================
# Testing
add_subdirectory(test)
