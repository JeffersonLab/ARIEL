set(PYTHON_MODULE_EXTENSION ".so")
find_ups_product(python)
find_package(pybind11 REQUIRED CONFIG)
include_directories(${pybind11_INCLUDE_DIRS})

include_directories(${PYTHON_INCLUDE_DIRS})
set(${product}_lib_dir ${${product}_lib_dir}/python${PYTHON_VERSION}/site-packages)
cet_make(LIBRARY_NAME fhicl
  LIBRARIES cetlib cetlib_except fhiclcpp ${PYTHON_LIBRARIES})
SET_TARGET_PROPERTIES(fhicl PROPERTIES PREFIX "" SUFFIX ".so")
