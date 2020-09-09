// ======================================================================
//
// Executable for dumping processed configuration files
//
// ======================================================================

#include "boost/program_options.hpp"
#include "cetlib/ostream_handle.h"
#include "cetlib/parsed_program_options.h"
#include "cetlib_except/demangle.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/detail/print_mode.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/parse.h"

#include <fstream>
#include <iostream>
#include <string>
#include <variant>

using namespace fhicl;
using namespace fhicl::detail;

using std::string;

namespace {

  string const fhicl_env_var{"FHICL_FILE_PATH"};

  // Error categories
  string const help{"Help"};
  string const processing{"Processing"};
  string const config{"Configuration"};

  struct Help {
    std::string msg;
  };

  struct Options {
    print_mode mode{print_mode::raw};
    bool quiet{false};
    string output_filename;
    string input_filename;
    std::unique_ptr<cet::filepath_maker> policy;
    string lookup_path;
  };

  std::variant<Options, Help>
  process_arguments(int argc, char** argv);

  fhicl::ParameterSet form_pset(string const& filename,
                                cet::filepath_maker& lookup_policy);
}

//======================================================================

int
main(int argc, char** argv)
{
  auto const opts_or_help = process_arguments(argc, argv);
  if (std::holds_alternative<Help>(opts_or_help)) {
    std::cout << std::get<Help>(opts_or_help).msg;
    return 0;
  }

  auto const& opts = std::get<Options>(opts_or_help);
  auto const pset = form_pset(opts.input_filename, *opts.policy);

  if (opts.quiet)
    return 0;

  auto os = cet::select_stream(opts.output_filename, std::cout);

  os << "# Produced from '" << argv[0] << "' using:\n"
     << "#   Input  : " << opts.input_filename << '\n'
     << "#   Policy : "
     << cet::demangle_symbol(typeid(decltype(*opts.policy)).name()) << '\n'
     << "#   Path   : \"" << opts.lookup_path << "\"\n\n"
     << pset.to_indented_string(0, opts.mode);
}

//======================================================================

namespace {

  std::variant<Options, Help>
  process_arguments(int argc, char** argv)
  {
    namespace bpo = boost::program_options;

    Options opts;

    bool annotate{false};
    bool prefix_annotate{false};

    bpo::options_description desc("fhicl-dump [-c] <file>\nOptions");
    // clang-format off
    desc.add_options()
      ("help,h", "produce this help message")
      ("config,c", bpo::value<std::string>(&opts.input_filename), "input file")
      ("output,o", bpo::value<std::string>(&opts.output_filename),
         "output file (default is STDOUT)")
      ("annotate,a",
         bpo::bool_switch(&annotate),
         "include source location annotations")
      ("prefix-annotate",
         bpo::bool_switch(&prefix_annotate),
         "include source location annotations on line preceding parameter "
         "assignment (mutually exclusive with 'annotate' option)")
      ("quiet,q", "suppress output to STDOUT")
      ("lookup-policy,l",
         bpo::value<string>()->default_value("permissive"), "see --supported-policies")
      ("path,p",
         bpo::value<std::string>(&opts.lookup_path)->default_value(fhicl_env_var),
         "path or environment variable to be used by lookup-policy")
      ("supported-policies", "list the supported file lookup policies");
    // clang-format on

    bpo::positional_options_description p;
    p.add("config", -1);

    auto const vm = cet::parsed_program_options(argc, argv, desc, p);

    if (vm.count("help")) {
      std::ostringstream os;
      os << desc << '\n';
      return Help{os.str()};
    }

    cet::lookup_policy_selector const supported_policies{};
    if (vm.count("supported-policies")) {
      return Help{supported_policies.help_message()};
    }

    if (vm.count("quiet")) {
      if (annotate || prefix_annotate) {
        throw cet::exception(config) << "Cannot specify both '--quiet' and "
                                        "'--(prefix-)annotate' options.\n";
      }
      opts.quiet = true;
    }

    if (annotate && prefix_annotate) {
      throw cet::exception(config) << "Cannot specify both '--annotate' and "
                                      "'--prefix-annotate' options.\n";
    }

    if (vm.count("lookup-policy") > 0) {
      opts.policy = supported_policies.select(
        vm["lookup-policy"].as<std::string>(), opts.lookup_path);
    }

    if (annotate)
      opts.mode = print_mode::annotated;
    if (prefix_annotate)
      opts.mode = print_mode::prefix_annotated;

    if (!vm.count("config")) {
      std::ostringstream err_stream;
      err_stream << "\nMissing input configuration file.\n\n" << desc << '\n';
      throw cet::exception(config) << err_stream.str();
    }
    return opts;
  }

  fhicl::ParameterSet
  form_pset(std::string const& filename, cet::filepath_maker& lookup_policy)
  {
    fhicl::intermediate_table tbl;
    fhicl::parse_document(filename, lookup_policy, tbl);
    fhicl::ParameterSet pset;
    fhicl::make_ParameterSet(tbl, pset);
    return pset;
  }
}
