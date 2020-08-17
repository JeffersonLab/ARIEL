set(PYTHON_MODULE_EXTENSION ".so")
find_package(PythonInterp REQUIRED)
find_package(PythonLibs REQUIRED)
find_package(pybind11 REQUIRED CONFIG)
include_directories(${pybind11_INCLUDE_DIRS})

include_directories(${PYTHON_INCLUDE_DIRS})
cet_make(LIBRARY_NAME fhicl
  LIBRARIES cetlib cetlib_except fhiclcpp ${PYTHON_LIBRARIES})
SET_TARGET_PROPERTIES(fhicl PROPERTIES PREFIX "" SUFFIX ".so")
