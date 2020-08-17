cet_enable_asserts()

# add_definitions(-DART_NO_MIX_PTRVECTOR)

# Make sure tests have correct environment settings.
include(CetTest)
# If you explicitly include CetTest in a subdirectory, you will need to
# re-initialize the test environment.
cet_test_env("FHICL_FILE_PATH=.")

include(ArtMake)

# build Persistency libraries
add_subdirectory(CriticTestObjects)
add_subdirectory(art)
add_subdirectory(gallery)
