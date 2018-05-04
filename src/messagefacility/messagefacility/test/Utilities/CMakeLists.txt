# ======================================================================
#
# Testing script
#
# ======================================================================

cet_test_env(CLEAR "FHICL_FILE_PATH=${CMAKE_CURRENT_SOURCE_DIR}")

cet_test(Exception_t LIBRARIES MF_MessageLogger cetlib_except)
cet_test(ExceptionDerived_t LIBRARIES MF_MessageLogger cetlib_except)
cet_test(Simple_t LIBRARIES MF_MessageLogger cetlib_except)
