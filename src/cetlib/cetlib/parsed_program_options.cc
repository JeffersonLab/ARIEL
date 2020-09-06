#include "cetlib/parsed_program_options.h"
#include "cetlib_except/exception.h"

#include <ostream>

namespace bpo = boost::program_options;

bpo::variables_map
cet::parsed_program_options(
  int argc,
  char** argv,
  bpo::options_description const& desc,
  bpo::positional_options_description const& pos) try {
  bpo::variables_map result;
  bpo::store(bpo::command_line_parser(argc, argv)
               .options(desc)
               .style(bpo::command_line_style::default_style &
                      ~bpo::command_line_style::allow_guessing)
               .positional(pos)
               .run(),
             result);
  bpo::notify(result);
  return result;
}
catch (bpo::error& err) {
  std::ostringstream err_stream;
  err_stream << "Error processing command line in " << argv[0] << ": "
             << err.what() << '\n';
  throw cet::exception("Processing") << err_stream.str();
}
