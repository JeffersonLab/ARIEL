#ifndef cetlib_parsed_program_options_h
#define cetlib_parsed_program_options_h

// =====================================================================
// The parsed_program_options function provides a simple interface for
// accessing the boost::variables_map object created after the
// successful parsing of the command-line.
//
// The function accepts the 'argc' and 'argv' arguments passed to 'int
// main(...)' as well as the full program-options description object
// and an optional positional-options description object.
//
// This function disables program-option guessing--i.e. if the
// supported program option is --plugh and --plu is specified, it is
// an error and a exception will be thrown.
// =====================================================================

#include "boost/program_options.hpp"

namespace cet {
  boost::program_options::variables_map parsed_program_options(
    int argc,
    char** argv,
    boost::program_options::options_description const& desc,
    boost::program_options::positional_options_description const& pos = {});
}

#endif /* cetlib_parsed_program_options_h */

// Local Variables:
// mode: c++
// End:
