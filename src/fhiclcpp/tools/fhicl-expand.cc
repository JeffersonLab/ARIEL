// ======================================================================
//
// fhicl-expand: read/write specified text, replacing each #include
//               directive with the corresponding file's contents
//
//  If called with no arguments, fhicl-expand reads from standard in.
//  If called with one or more arguments, each argument is
//  interepreted as the name of a file to be processed. If '-' (a
//  single hyphen) is passed as a filename, this is interpreted as
//  instruction to read from standard input.
//
//  By default, the expanded inputs are all written to standard
//  output, and any errors during processing are written to standard
//  error. The -o (or --output) and -e (or --error) options can be
//  used to send output to the named files.
//  ======================================================================

#include "boost/program_options.hpp"
#include "cetlib/container_algorithms.h"
#include "cetlib/filepath_maker.h"
#include "cetlib/includer.h"
#include "cetlib/ostream_handle.h"
#include "cetlib/parsed_program_options.h"
#include "cetlib_except/exception.h"

#include <fstream>
#include <iostream>
#include <istream>
#include <memory>
#include <string>
#include <vector>

namespace {

  std::string const fhicl_env_var{"FHICL_FILE_PATH"};

  // Error categories
  std::string const help{"Help"};
  std::string const processing{"Processing"};

  using std::string;
  using strings = std::vector<string>;

  struct Options {
    string error_filename;
    string output_filename;
    strings input_filenames;
    std::unique_ptr<cet::filepath_maker> policy;
    string lookup_path;
  };

  Options process_arguments(int argc, char** argv);

  int do_including(std::string const& starting_filename,
                   cet::filepath_maker& lookup_policy,
                   std::ostream& to,
                   std::ostream& err);
}

// ===================================================================

int
main(int argc, char** argv)
{
  Options opts;
  try {
    opts = process_arguments(argc, argv);
  }
  catch (cet::exception const& e) {
    if (e.category() == help)
      return 0;
    if (e.category() == processing)
      return 1;
  }

  // Set output/error streams
  auto out = cet::select_stream(opts.output_filename, std::cout);
  auto err = cet::select_stream(opts.error_filename, std::cerr);

  int nfailures{};
  for (std::string const& filename : opts.input_filenames)
    nfailures += do_including(filename, *opts.policy, out, err);

  return nfailures;
}

// ===================================================================

namespace {

  Options
  process_arguments(int argc, char** argv)
  {
    using namespace std;
    namespace bpo = boost::program_options;

    // Parse the command line:

    bpo::options_description desc("fhicl-expand <options> [files]\nOptions");
    Options opts;
    // clang-format off
    desc.add_options()
      ("help,h", "produce help message")
      ("inputs,i", bpo::value<strings>(&opts.input_filenames), "input files")
      ("output,o", bpo::value<string>(&opts.output_filename), "output file")
      ("error,e", bpo::value<string>(&opts.error_filename), "error file")
      ("lookup-policy,l",
         bpo::value<string>()->default_value("permissive"), "see --supported-policies")
      ("path,p",
         bpo::value<string>(&opts.lookup_path)->default_value(fhicl_env_var),
        "path or environment variable to be used by lookup-policy")
      ("supported-policies", "list the supported file lookup policies");
    // clang-format on

    bpo::positional_options_description pd;
    pd.add("inputs", -1);

    auto const vm = cet::parsed_program_options(argc, argv, desc, pd);

    // Interpret options:

    if (vm.count("help")) {
      std::cout << desc << "\n";
      throw cet::exception(help);
    }

    cet::lookup_policy_selector const supported_policies{};
    if (vm.count("supported-policies")) {
      std::cout << supported_policies.help_message();
      throw cet::exception(help);
    }

    if (vm.count("lookup-policy") > 0) {
      opts.policy = supported_policies.select(
        vm["lookup-policy"].as<std::string>(), opts.lookup_path);
    }

    if (opts.input_filenames.empty())
      opts.input_filenames.push_back("-");

    return opts;
  }

  int
  do_including(std::string const& starting_filename,
               cet::filepath_maker& lookup_policy,
               std::ostream& to,
               std::ostream& err) try {
    cet::includer inc{starting_filename, lookup_policy};
    cet::copy_all(inc, std::ostream_iterator<char>{to});
    return 0;
  }
  catch (cet::exception const& e) {
    err << "Error for file: " << starting_filename << '\n' << e.what() << '\n';
    return 1;
  }
}
