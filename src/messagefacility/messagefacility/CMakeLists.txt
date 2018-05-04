include(CetMake)

cet_make_library(
  LIBRARY_NAME
    MF_MessageLogger
  SOURCE
    MessageLogger/MessageLogger.cc
    MessageLogger/MFConfig.cc
    MessageService/ELdestination.cc
    MessageService/ELostreamOutput.cc
    MessageService/ELstatistics.cc
    Utilities/ELextendedID.cc
    Utilities/ELseverityLevel.cc
    Utilities/ErrorObj.cc
    Utilities/exception.cc
  LIBRARIES
    cetlib
    cetlib_except
    fhiclcpp
    hep_concurrency
    ${TBB})

add_subdirectory(MessageLogger)
add_subdirectory(MessageService)
add_subdirectory(Utilities)
add_subdirectory(plugins)
# Testing
add_subdirectory(test)
