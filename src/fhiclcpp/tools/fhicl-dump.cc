// ======================================================================
//
// Executable for dumping processed configuration files
//
// ======================================================================

#include "boost/program_options.hpp"
#include "cetlib_except/demangle.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/detail/print_mode.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/parse.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace fhicl;
using namespace fhicl::detail;

namespace {

  std::string const fhicl_env_var{"FHICL_FILE_PATH"};

  // Error categories
  std::string const help{"Help"};
  std::string const processing{"Processing"};
  std::string const config{"Configuration"};

  using std::string;

  struct Options {
    print_mode mode{print_mode::raw};
    bool quiet{false};
    string output_filename;
    string input_filename;
    int lookup_policy{};
    string lookup_path;
  };

  Options process_arguments(int argc, char* argv[]);

  fhicl::ParameterSet form_pset(string const& filename,
                                cet::filepath_maker& lookup_policy);

  std::unique_ptr<cet::filepath_maker> get_policy(int const lookup_policy,
                                                  string const& lookup_path);
}

//======================================================================

int
main(int argc, char* argv[])
{

  Options opts;
  try {
    opts = process_arguments(argc, argv);
  }
  catch (cet::exception const& e) {
    if (e.category() == help)
      return 1;
    if (e.category() == processing) {
      std::cerr << e.what() << '\n';
      return 2;
    }
    if (e.category() == config) {
      std::cerr << e.what() << '\n';
      return 3;
    }
  }

  auto const policy = get_policy(opts.lookup_policy, opts.lookup_path);

  ParameterSet pset;
  try {
    pset = form_pset(opts.input_filename, *policy);
  }
  catch (cet::exception const& e) {
    std::cerr << e.what() << '\n';
    return 4;
  }
  catch (...) {
    std::cerr << "Unknown exception\n";
    return 5;
  }

  if (opts.quiet)
    return 0;

  std::ofstream ofs{opts.output_filename};
  std::ostream& os = opts.output_filename.empty() ? std::cout : ofs;

  os << "# Produced from '" << argv[0] << "' using:\n"
     << "#   Input  : " << opts.input_filename << '\n'
     << "#   Policy : "
     << cet::demangle_symbol(typeid(decltype(*policy)).name()) << '\n'
     << "#   Path   : \"" << opts.lookup_path << "\"\n\n"
     << pset.to_indented_string(0, opts.mode);
}

//======================================================================

namespace {

  Options
  process_arguments(int argc, char* argv[])
  {
    namespace bpo = boost::program_options;

    Options opts;

    bool annotated{false};
    bool prefix_annotated{false};

    bpo::options_description desc("fhicl-dump [-c] <file>\nOptions");
    desc.add_options()("help,h", "produce this help message")(
      "config,c", bpo::value<std::string>(&opts.input_filename), "input file")(
      "output,o",
      bpo::value<std::string>(&opts.output_filename),
      "output file (default is STDOUT)")(
      "annotated,a",
      bpo::bool_switch(&annotated)->default_value(false, "false"),
      "include source location annotations")(
      "prefix-annotated",
      bpo::bool_switch(&prefix_annotated)->default_value(false, "false"),
      "include source location annotations on line preceding parameter "
      "assignment (mutually exclusive with 'annotated' option)")(
      "quiet,q", "suppress output to STDOUT")(
      "lookup-policy,l",
      bpo::value<int>(&opts.lookup_policy)->default_value(1),
      "lookup policy code:"
      "\n  0 => cet::filepath_maker"
      "\n  1 => cet::filepath_lookup"
      "\n  2 => cet::filepath_lookup_nonabsolute"
      "\n  3 => cet::filepath_lookup_after1")(
      "path,p",
      bpo::value<std::string>(&opts.lookup_path)->default_value(fhicl_env_var),
      "path or environment variable to be used by lookup-policy");

    bpo::positional_options_description p;
    p.add("config", -1);

    bpo::variables_map vm;
    try {
      bpo::store(
        bpo::command_line_parser(argc, argv).options(desc).positional(p).run(),
        vm);
      bpo::notify(vm);
    }
    catch (bpo::error& err) {
      std::ostringstream err_stream;
      err_stream << "Error processing command line in " << argv[0] << ": "
                 << err.what() << '\n';
      throw cet::exception(processing) << err_stream.str();
    };

    if (vm.count("help")) {
      std::cout << desc << '\n';
      throw cet::exception(help);
    }

    if (vm.count("quiet")) {
      if (annotated || prefix_annotated) {
        throw cet::exception(config) << "Cannot specify both '--quiet' and "
                                        "'--(prefix-)annotated' options.\n";
      }
      opts.quiet = true;
    }

    if (annotated && prefix_annotated) {
      throw cet::exception(config) << "Cannot specify both '--annotated' and "
                                      "'--prefix-annotated' options.\n";
    }

    if (annotated)
      opts.mode = print_mode::annotated;
    if (prefix_annotated)
      opts.mode = print_mode::prefix_annotated;

    if (!vm.count("config")) {
      std::ostringstream err_stream;
      err_stream << "\nMissing input configuration file.\n\n" << desc << '\n';
      throw cet::exception(config) << err_stream.str();
    }
    return opts;
  }

  std::unique_ptr<cet::filepath_maker>
  get_policy(int const lookup_policy, std::string const& lookup_path)
  {
    switch (lookup_policy) {
      case 0:
        return std::make_unique<cet::filepath_maker>();
      case 1:
        return std::make_unique<cet::filepath_lookup>(lookup_path);
      case 2:
        return std::make_unique<cet::filepath_lookup_nonabsolute>(lookup_path);
      case 3:
        return std::make_unique<cet::filepath_lookup_after1>(lookup_path);
      default:
        std::ostringstream err_stream;
        err_stream << "Error: command line lookup-policy " << lookup_policy
                   << " is unknown; choose 0, 1, 2, or 3\n";
        throw std::runtime_error(err_stream.str());
    }
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
