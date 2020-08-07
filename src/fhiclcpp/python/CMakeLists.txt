include_directories(${PYTHON_INCLUDE_DIRS})
cet_make(LIBRARY_NAME fhicl LIBRARIES cetlib cetlib_except fhiclcpp ${PYTHON})
SET_TARGET_PROPERTIES(fhicl PROPERTIES PREFIX "")
