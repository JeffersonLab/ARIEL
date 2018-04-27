########################################################################
# install_license()
# install LICENSE and README
#
####################################
# Recommended use:
#
# install_license()
#
########################################################################

include(CetCurrentSubdir)

macro( install_license   )
  _cet_current_subdir( CURRENT_SUBDIR )
  set(license_install_dir ${product}/${version} )
  _cet_debug_message( "install_license: files will be installed in  ${license_install_dir}" )
  FILE(GLOB license_files
	    README LICENSE
	    )
  if( license_files )
    INSTALL( FILES ${license_files}
             DESTINATION ${license_install_dir} )
  endif( license_files )
endmacro( install_license )
