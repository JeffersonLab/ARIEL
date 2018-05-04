include_directories($ENV{PYTHON_INCLUDE})
cet_make(LIBRARY_NAME fhicl LIBRARIES cetlib cetlib_except fhiclcpp ${PYTHON})
SET_TARGET_PROPERTIES(fhicl PROPERTIES PREFIX "")
