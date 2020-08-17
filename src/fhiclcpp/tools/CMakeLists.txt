include(CetMake)
include(CetMakeCompletions)

cet_make_exec(fhicl-dump
  SOURCE fhicl-dump.cc
  LIBRARIES cetlib fhiclcpp Boost::program_options)

cet_make_exec(fhicl-expand
  SOURCE fhicl-expand.cc
  LIBRARIES cetlib cetlib_except Boost::program_options)

cet_make_exec(fhicl-get
  SOURCE fhicl-get.cc
  LIBRARIES fhiclcpp cetlib cetlib_except Boost::program_options)

cet_make_exec(fhicl-write-db
  SOURCE fhicl-write-db.cc
  LIBRARIES fhiclcpp ${SQLITE3})

cet_make_completions(fhicl-dump)
cet_make_completions(fhicl-expand)
cet_make_completions(fhicl-get)

install_headers()
install_source()
