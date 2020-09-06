#ifndef tools_Printer_h
#define tools_Printer_h

#include "cetlib/exempt_ptr.h"
#include "cetlib_except/demangle.h"
#include "cetlib_except/exception.h"
#include "fhiclcpp/ParameterSet.h"

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace fhicl {
  class Printer {
  public:
    virtual ~Printer() = default;
    void
    print_as_atom(fhicl::ParameterSet const& pset, std::string const& key) const
    {
      do_atomic_print(pset, key);
    }
    void
    print_as_sequence(fhicl::ParameterSet const& pset,
                      std::string const& key) const
    {
      do_sequence_print(pset, key);
    }

  private:
    virtual void do_atomic_print(fhicl::ParameterSet const& pset,
                                 std::string const& key) const = 0;
    virtual void do_sequence_print(fhicl::ParameterSet const& pset,
                                   std::string const& key) const = 0;
  };

  template <typename T>
  class PrinterFor : public Printer {
    void
    do_atomic_print(fhicl::ParameterSet const& pset,
                    std::string const& key) const
    {
      std::cout << pset.get<T>(key) << '\n';
    }

    void
    do_sequence_print(fhicl::ParameterSet const& pset,
                      std::string const& key) const
    {
      auto const entries = pset.get<std::vector<T>>(key);
      for (auto const& entry : entries) {
        std::cout << entry << '\n';
      }
    }
  };

  namespace detail {
    template <typename T>
    std::string
    name_of()
    {
      return cet::demangle_symbol(typeid(T).name());
    }

    template <>
    std::string
    name_of<std::string>()
    {
      return "string";
    }
  }

  template <typename... Args>
  class PrinterForTypes {
  public:
    PrinterForTypes()
    {
      (printers_.emplace(detail::name_of<Args>(),
                         std::make_unique<PrinterFor<Args>>()),
       ...);
    }

    std::string
    help_message() const
    {
      std::string result{"Supported types include:\n"};
      for (auto const& pr : printers_) {
        result += "  '";
        result += pr.first;
        result += "'\n";
      }
      return result;
    }

    void
    value_for_atom(fhicl::ParameterSet const& pset,
                   std::string const& cpp_type,
                   std::string const& key) const
    {
      get_(cpp_type)->print_as_atom(pset, key);
    }

    void
    value_for_sequence(fhicl::ParameterSet const& pset,
                       std::string const& cpp_type,
                       std::string const& key) const
    {
      get_(cpp_type)->print_as_sequence(pset, key);
    }

    cet::exempt_ptr<Printer const>
    get_(std::string const& cpp_type) const
    {
      if (auto it = printers_.find(cpp_type); it != cend(printers_)) {
        return it->second.get();
      }
      throw cet::exception{"Usage error"} << "The specified type '" << cpp_type
                                          << "' is not supported.\n"
                                          << help_message();
    }

    std::map<std::string, std::unique_ptr<Printer>> printers_;
  };

}

#endif /* tools_Printer_h */

// Local Variables:
// mode: c++
// End:
