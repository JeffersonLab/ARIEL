include(CetMake)
include(CetMakeCompletions)

cet_make_exec(fhicl-dump
  SOURCE fhicl-dump.cc
  LIBRARIES
  cetlib fhiclcpp ${Boost_PROGRAM_OPTIONS_LIBRARY}
  )

cet_make_exec(fhicl-expand
  SOURCE fhicl-expand.cc
  LIBRARIES
  cetlib cetlib_except ${Boost_PROGRAM_OPTIONS_LIBRARY}
  )

cet_make_exec(fhicl-write-db
  SOURCE fhicl-write-db.cc
  LIBRARIES
  fhiclcpp
  ${SQLITE3}
)

cet_make_completions(fhicl-dump)
cet_make_completions(fhicl-expand)

install_headers()
install_source()
