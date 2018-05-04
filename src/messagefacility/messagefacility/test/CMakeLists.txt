# ======================================================================
#
# Testing
#
# ======================================================================

cet_enable_asserts()
include(CetTest)

cet_test_env("FHICL_FILE_PATH=.")

add_subdirectory(Utilities)
add_subdirectory(Integration)
add_subdirectory(MessageService)
