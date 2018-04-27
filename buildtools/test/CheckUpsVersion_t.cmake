include(CheckUpsVersion)

function(ensure_versions THIS MIN COMP)
  string(TOUPPER "${COMP}" COMP)
  check_ups_version(xxx ${THIS} ${MIN}
    PRODUCT_OLDER_VAR older
    PRODUCT_MATCHES_VAR not_older
    )
  if (COMP STREQUAL "OLDER")
    if (NOT older)
      message(FATAL_ERROR "${THIS} should be OLDER than ${MIN}")
    endif()
  else()
    check_ups_version(xxx ${MIN} ${THIS}
      PRODUCT_OLDER_VAR newer
      PRODUCT_MATCHES_VAR not_newer
      )
    if (COMP STREQUAL "NEWER")
      if (NOT newer)
        message(FATAL_ERROR "${THIS} should be NEWER than ${MIN}")
      endif()
    elseif (COMP STREQUAL "SAME")
      if (NOT (not_older AND not_newer))
        message(FATAL_ERROR "${THIS} should be the SAME as ${MIN}")
      endif()
    else()
      message(FATAL_ERROR "Unknown value of COMP: ${COMP}: should be OLDER, NEWER or SAME")
    endif()
  endif()
endfunction()

# Check equivalent versions.
ensure_versions(v1_2_3 v1_02_03 SAME)
ensure_versions(v01_01_01 v1_1_1 SAME)
ensure_versions(v1_01_03RC1 v1_01_03_RC01 SAME)
ensure_versions(v3_04_02_pre01 v3_04_02_PRE1 SAME)
ensure_versions(v1_0 v1 SAME)
ensure_versions(v1_0_0 v1 SAME)
ensure_versions(v1_0_0 v1_0 SAME)
ensure_versions(v1_0_0_0 v1 SAME)
# Check non-numeric versions.
ensure_versions(nightly perennially SAME)
ensure_versions(nightly v0 NEWER)
ensure_versions(nightly v99 NEWER)
ensure_versions(v107 nightly OLDER)
# Check normal version precendence.
ensure_versions(v1 v2 OLDER)
ensure_versions(v1_0 v1_0_0_rc3 NEWER)
ensure_versions(v26_0_0_4 v25 NEWER)
ensure_versions(v26_3_2_4 v26_2_2_4 NEWER)
ensure_versions(v26_3_2_4 v26_3_1_4 NEWER)
ensure_versions(v26_3_2_4 v26_3_2_3 NEWER)
ensure_versions(v26_3_2p v26_3_2_p04 NEWER)
ensure_versions(v26_3_2p v26_3_2q OLDER)
