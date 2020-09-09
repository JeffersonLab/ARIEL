#include "boost/program_options.hpp"
#include "cetlib/exempt_ptr.h"
#include "cetlib/filepath_maker.h"
#include "cetlib/parsed_program_options.h"
#include "cetlib_except/demangle.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/detail/ValuePrinter.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/parse.h"
#include "tools/Printer.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace {
  std::string const fhicl_env_var{"FHICL_FILE_PATH"};

  std::string const help{"Help"};
  std::string const processing{"Processing"};
  std::string const config{"Configuration"};
  std::string const usage{"Usage error"};

  struct Help {
    std::string msg;
  };

  struct Options {
    std::string input_filename;
    std::string atom_as{};
    std::string sequence_of{};
    bool names_in{false};
    std::string parameter_key;
    std::unique_ptr<cet::filepath_maker> policy;
    std::string lookup_path;
  };

  using supported_types_t = fhicl::PrinterForTypes<int, double, std::string>;

  std::variant<Options, Help>
  process_arguments(int argc,
                    char** argv,
                    supported_types_t const& supported_types)
  {
    namespace bpo = boost::program_options;

    Options opts;

    bpo::options_description desc(
      "Usage: fhicl-get [options] <key> <file>\n\n"
      "Required parameters:\n"
      "  <key>       A fully-qualified parameter key of the form 'a.b.c.d'\n"
      "  <file>      A valid FHiCL document that contains the parameter with\n"
      "              the name as specified for <key>\n\n"
      "Supported options");
    // clang-format off
    desc.add_options()
      ("help,h", "Produce this help message")
      ("atom-as",  bpo::value<std::string>(&opts.atom_as),
       "Return value for the supplied key as an atom with the provided C++ type.")
      ("sequence-of",  bpo::value<std::string>(&opts.sequence_of),
       "Return value for the supplied key as a sequence of the provided C++ type.")
      ("names-in", "Print the top-level names of the supplied key, which "
       "must correspond to a FHiCL table.")
      ("lookup-policy",
         bpo::value<std::string>()->default_value("permissive"), "see --supported-policies")
      ("lookup-path",
         bpo::value<std::string>(&opts.lookup_path)->default_value(fhicl_env_var),
         "path or environment variable to be used by lookup-policy")
      ("supported-types", "list the C++ types supported for by the --atom-as and --sequence-of options.")
      ("supported-policies", "list the supported file lookup policies");
    // clang-format on

    bpo::options_description positional_desc;
    // clang-format off
    positional_desc.add_options()
      ("key", bpo::value<std::string>(&opts.parameter_key))
      ("config", bpo::value<std::string>(&opts.input_filename));
    // clang-format on

    bpo::positional_options_description p;
    p.add("key", 1);
    p.add("config", 2);

    bpo::options_description all;
    all.add(desc).add(positional_desc);

    auto const vm = cet::parsed_program_options(argc, argv, all, p);

    if (vm.count("help")) {
      std::ostringstream oss;
      oss << '\n' << desc << '\n';
      return Help{oss.str()};
    }

    if (vm.count("supported-types")) {
      std::string result{R"(
For the following command line:

  fhicl-get --atom-as=T <key> <file>

the <file> is queried for the <key>, and an attempt is made to
interpret the corresponding value as an object of type T.

If instead the command line were specified as:

  fhicl-get --sequence-of=T <key> <file>

then the value corresponding to <key> would be interpreted as an
std::vector<T> object.

For either the --atom-as or --sequence-of program options, an
exception will be thrown if the <key> parameter does not exist in the
<file>, or if the parameter does not correspond to a value that can be
interpreted according to the user-specified command-line.

)"};
      result += supported_types.help_message();
      result += '\n';
      return Help{result};
    }

    cet::lookup_policy_selector const supported_policies{};
    if (vm.count("supported-policies")) {
      return Help{supported_policies.help_message()};
    }

    if (!vm.count("config")) {
      std::ostringstream err_stream;
      err_stream << "\nMissing input configuration file.\n\n" << desc << '\n';
      throw cet::exception(config) << err_stream.str();
    }

    opts.names_in = vm.count("names-in") == 1;
    auto const options_count = vm.count("atom-as") + vm.count("sequence-of") +
                               static_cast<unsigned>(opts.names_in);

    if (options_count == 0) {
      throw cet::exception(usage)
        << "One of the 'atom-as', 'sequence-of', and and 'names-in' program "
           "options must be specified.\n";
    }
    if (options_count > 1) {
      throw cet::exception(usage)
        << "The 'atom-as', 'sequence-of', and and 'names-in' program options "
           "are mutually exclusive and may only appear once in the command "
           "line.\n";
    }

    opts.policy =
      supported_policies.select(vm["lookup-policy"].as<std::string>(),
                                vm["lookup-path"].as<std::string>());
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

  void
  print_table_names(fhicl::ParameterSet const& pset, std::string const& key)
  {
    if (not pset.is_key_to_table(key)) {
      throw cet::exception{config} << "The parameter named '" << key
                                   << "' does not have a table value.";
    }
    auto const names = pset.get<fhicl::ParameterSet>(key).get_names();
    for (auto const& name : names) {
      std::cout << name << '\n';
    }
  }
}

int
main(int argc, char** argv)
{
  supported_types_t const printer_for_types{};
  auto const maybe_opts = process_arguments(argc, argv, printer_for_types);

  if (std::holds_alternative<Help>(maybe_opts)) {
    std::cout << std::get<Help>(maybe_opts).msg;
    return 0;
  }

  auto const& opts = std::get<Options>(maybe_opts);

  auto const pset = form_pset(opts.input_filename, *opts.policy);

  auto const& key = opts.parameter_key;

  if (not pset.has_key(key)) {
    throw cet::exception{config} << "A parameter with the fully-qualified key '"
                                 << key << "' does not exist.";
  }

  if (not empty(opts.atom_as)) {
    printer_for_types.value_for_atom(pset, opts.atom_as, key);
    return 0;
  }

  if (not empty(opts.sequence_of)) {
    printer_for_types.value_for_sequence(pset, opts.sequence_of, key);
    return 0;
  }

  if (opts.names_in) {
    print_table_names(pset, key);
    return 0;
  }
}
