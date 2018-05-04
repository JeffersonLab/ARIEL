# Not using cet_make_library() because we explicitly *don't* want a
# shared library.
add_library(mf_catch_main STATIC catch_main.cc)
target_include_directories(mf_catch_main PUBLIC $ENV{CATCH_INC})
add_custom_command(TARGET mf_catch_main POST_BUILD
  COMMAND strip -S $<TARGET_FILE:mf_catch_main>
  COMMENT "Stripping messagefacility-Catch main library"
  )
_cet_check_lib_directory()
cet_add_to_library_list(mf_catch_main)
install(TARGETS mf_catch_main ARCHIVE DESTINATION ${${product}_lib_dir})

# install files
install_headers()
install_source()
