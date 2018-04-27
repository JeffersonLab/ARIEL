# Special case for geant4 since it has so many libraries
#
# find_ups_geant4(  [minimum] )
#  minimum - optional minimum version 

include(CheckUpsVersion)

# since variables are passed, this is implemented as a macro
macro( find_ups_geant4  )

cmake_parse_arguments( FUG "" "" "" ${ARGN} )
set( minimum )
if( FUG_UNPARSED_ARGUMENTS )
  list( GET FUG_UNPARSED_ARGUMENTS 0 minimum )
endif()
find_ups_product( geant4 ${minimum} )
find_ups_product( xerces_c v3_0_0 )

# add include directory to include path if it exists
include_directories ( $ENV{G4INCLUDE} )

# geant4 libraries
find_library( XERCESC NAMES xerces-c PATHS $ENV{XERCESCROOT}/lib NO_DEFAULT_PATH  )
cet_find_library( G4FR NAMES G4FR PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4GMOCREN NAMES G4GMocren PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4OPENGL NAMES G4OpenGL PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4RAYTRACER  NAMES G4RayTracer PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4TREE  NAMES G4Tree PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4VRML  NAMES G4VRML PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4ANALYSIS  NAMES G4analysis PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4DIGITS_HITS  NAMES G4digits_hits PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4ERROR_PROPAGATION  NAMES G4error_propagation PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4EVENT  NAMES G4event PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4GEOMETRY  NAMES G4geometry PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4GL2PS  NAMES G4gl2ps PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4GLOBAL  NAMES G4global PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4GRAPHICS_REPS  NAMES G4graphics_reps PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4INTERCOMS  NAMES G4intercoms PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4INTERFACES  NAMES G4interfaces PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4MATERIALS NAMES G4materials PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4MODELING  NAMES G4modeling PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4PARMODELS  NAMES G4parmodels PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4PARTICLES  NAMES G4particles PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4PERSISTENCY  NAMES G4persistency PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4PHYSICSLISTS  NAMES G4physicslists PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4PROCESSES  NAMES G4processes PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4READOUT  NAMES G4readout PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4RUN  NAMES G4run PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4TRACK  NAMES G4track PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4TRACKING  NAMES G4tracking PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4VISHEPREP  NAMES G4visHepRep PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4VISXXX  NAMES G4visXXX PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4VIS_MANAGEMENT  NAMES G4vis_management PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4ZLIB  NAMES G4zlib PATHS ENV G4LIB NO_DEFAULT_PATH )

set( G4_LIB_LIST ${XERCESC}
                 ${G4FR}
                 ${G4GMOCREN}
                 ${G4OPENGL}
                 ${G4RAYTRACER}
                 ${G4TREE}
                 ${G4VRML}
                 ${G4ANALYSIS}
                 ${G4DIGITS_HITS}
                 ${G4ERROR_PROPAGATION}
                 ${G4EVENT}
                 ${G4GEOMETRY}
                 ${G4GL2PS}
                 ${G4GLOBAL}
                 ${G4GRAPHICS_REPS}
                 ${G4INTERCOMS}
                 ${G4INTERFACES}
                 ${G4MATERIALS}
                 ${G4MODELING}
                 ${G4PARMODELS}
                 ${G4PARTICLES}
                 ${G4PERSISTENCY}
                 ${G4PHYSICSLISTS}
                 ${G4PROCESSES}
                 ${G4READOUT}
                 ${G4RUN}
                 ${G4TRACK}
                 ${G4TRACKING}
                 ${G4VISHEPREP}
                 ${G4VISXXX}
                 ${G4VIS_MANAGEMENT}
                 ${G4ZLIB}
)


endmacro( find_ups_geant4 )


