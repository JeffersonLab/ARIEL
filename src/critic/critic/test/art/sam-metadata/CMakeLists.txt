cet_test(SAM_metadata HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c "SAMMetadata_w.fcl"
  --sam-application-family "Ethel"
  --sam-application-version "v0.00.01a"
  --sam-file-type "MC"
  --sam-run-type "MCChallenge"
  --sam-data-tier "The one with the thickest frosting"
  --sam-group "MyGang"
  DATAFILES fcl/SAMMetadata_w.fcl)

cet_test(SAM_metadata_verify_hr HANDBUILT
  TEST_EXEC sam_metadata_dumper
  TEST_ARGS -H -s ../SAM_metadata.d/out.root
  REQUIRED_FILES "../SAM_metadata.d/out.root"
  TEST_PROPERTIES DEPENDS SAM_metadata
  REF "${CMAKE_CURRENT_SOURCE_DIR}/SAM_metadata_verify_hr-ref.txt")

cet_test(SAM_metadata_verify_JSON HANDBUILT
  TEST_EXEC sam_metadata_dumper
  TEST_ARGS -s ../SAM_metadata.d/out.root
  REQUIRED_FILES "../SAM_metadata.d/out.root"
  TEST_PROPERTIES DEPENDS SAM_metadata
  REF "${CMAKE_CURRENT_SOURCE_DIR}/SAM_metadata_verify_JSON-ref.txt")

cet_test(SAM_metadataFromInput_01 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c SAMMetadata_r_passthrough1.fcl
  --sam-inherit-metadata
  --sam-data-tier "Chummy"
  -s ../SAM_metadata.d/out.root
  -o out.root
  REQUIRED_FILES "../SAM_metadata.d/out.root"
  TEST_PROPERTIES DEPENDS SAM_metadata
  DATAFILES fcl/SAMMetadata_r_passthrough1.fcl)

cet_test(SAM_metadataFromInput_01_verify HANDBUILT
  TEST_EXEC sam_metadata_dumper
  TEST_ARGS -H -s ../SAM_metadataFromInput_01.d/out.root
  TEST_PROPERTIES DEPENDS SAM_metadataFromInput_01
  REF "${CMAKE_CURRENT_SOURCE_DIR}/SAM_metadataFromInput_01_verify-ref.txt")

cet_test(SAM_metadataFromInput_02 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c SAMMetadata_r_passthrough2.fcl
  --sam-inherit-run-type
  --sam-data-tier "Peachy"
  -s ../SAM_metadata.d/out.root
  -o out.root
  TEST_PROPERTIES DEPENDS SAM_metadata
  DATAFILES fcl/SAMMetadata_r_passthrough2.fcl)

cet_test(SAM_metadataFromInput_02_verify HANDBUILT
  TEST_EXEC sam_metadata_dumper
  TEST_ARGS -H -s ../SAM_metadataFromInput_02.d/out.root
  TEST_PROPERTIES DEPENDS SAM_metadataFromInput_02
  REF "${CMAKE_CURRENT_SOURCE_DIR}/SAM_metadataFromInput_02_verify-ref.txt")

cet_test(SAM_metadataFromInput_03 HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c SAMMetadata_r_passthrough3.fcl
  --sam-inherit-file-type
  --sam-run-type "Trot"
  --sam-data-tier "Grapey"
  -s ../SAM_metadata.d/out.root
  -o out.root
  TEST_PROPERTIES DEPENDS SAM_metadata
  DATAFILES fcl/SAMMetadata_r_passthrough3.fcl)

cet_test(SAM_metadataFromInput_03_verify HANDBUILT
  TEST_EXEC sam_metadata_dumper
  TEST_ARGS -H -s ../SAM_metadataFromInput_03.d/out.root
  TEST_PROPERTIES DEPENDS SAM_metadataFromInput_03
  REF "${CMAKE_CURRENT_SOURCE_DIR}/SAM_metadataFromInput_03_verify-ref.txt")

cet_test(SAM_metadataFromInput_conflictingFields HANDBUILT
  TEST_EXEC art
  TEST_ARGS -c /dev/null
  --process-name "ShouldFail"
  --sam-inherit-file-type
  --sam-run-type "Canter"
  --sam-data-tier "Perky"
  -s ../SAM_metadataFromInput_01.d/out.root
  -s ../SAM_metadataFromInput_02.d/out.root
  -o out.root
  TEST_PROPERTIES
  PASS_REGULAR_EXPRESSION
  "  The value for 'file_type' for the current file is: \"Pescadero\", which conflicts with the value from the first input file \\(\"\"MC\"\"\\).")

set_tests_properties(SAM_metadataFromInput_conflictingFields
  PROPERTIES DEPENDS "SAM_metadataFromInput_01;SAM_metadataFromInput_02")
