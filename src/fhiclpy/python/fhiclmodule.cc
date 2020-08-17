//----------------------------------------------------------------------
//
// Name: fclmodule.cc
//
// Purpose: A python extension module that wraps the c++ fcl
//          api.  The main purpose of this module is to provide the
//          capability to read and parse a fcl file and return the
//          resulting parameter set in the form of a python dictionary.
//
//          This module compiles to a shared library (fhiclmodule.so).
//          When this shared library is on the python path, it can be
//          imported as a python module (import fhicl).
//
// Created: 4-Apr-2017  H. Greenlee
//
// FCL module interface.
//
// Module members.
//
// 1.  Function make_pset(fclfile)
//
// The argument is the name of a fcl file (can be anywhere on $FHICL_FILE_PATH).
// The function reads the fcl file and returns the expanded parameter set
// in the form of a python dictionary.
//
// Example:
//
// #!/usr/bin/env python
// import fhicl
// pset = fhicl.make_pset('myfile.fcl')
//
//----------------------------------------------------------------------

#include "cetlib/compiler_macros.h"
#pragma GCC diagnostic push
#if GCC_IS_AT_LEAST(7, 1, 0) || CLANG_IS_AT_LEAST(6, 0, 0) ||                  \
  APPLE_CLANG_IS_AT_LEAST(10, 0, 0)
#pragma GCC diagnostic ignored "-Wregister"
#endif
#include "pybind11/pybind11.h" // Needs to be (almost) first due to macro definition issues
#pragma GCC diagnostic pop
#include "cetlib/search_path.h"
#include "cetlib_except/exception.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/ParameterSetWalker.h"
#include "fhiclcpp/detail/printing_helpers.h"
#include "fhiclcpp/make_ParameterSet.h"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace py = pybind11;

namespace {

  class PythonDictConverter : public fhicl::ParameterSetWalker {
  public:
    using key_t = std::string;
    using any_t = std::any;

    py::dict result() const;

  private:
    // Base class overrides.
    void enter_table(key_t const& key, any_t const& any) override;
    void enter_sequence(key_t const& key, any_t const& any) override;
    void atom(key_t const& key, any_t const& any) override;
    void exit_table(key_t const& key, any_t const& any) override;
    void exit_sequence(key_t const& key, any_t const& any) override;

    void add_object(key_t const& key,
                    py::object pyobj); // Add object to current parent.

    // Data members.

    std::vector<py::list> lists_{};
    // Result stack.
    // dicts_[0] is the entire parameter set (a python dictionary).
    // dicts_.back() is the current parent container that is being filled
    // (a python dictionary or list).
    std::vector<py::dict> dicts_{py::dict{}};
  };

  py::dict
  PythonDictConverter::result() const
  //
  // Purpose: Return result.  When this method is called, the result stack
  //          should contain exactly one object, and this object should be a
  //          python dictionary.
  //
  // Returns: Python dictionary.
  //
  {
    // Do consistency checks.
    if (dicts_.size() != 1)
      throw cet::exception("fclmodule")
        << "Result stack has wrong size: " << dicts_.size() << std::endl;

    return dicts_[0];
  }

  void
  PythonDictConverter::enter_table(key_t const& key, any_t const&)
  {
    // Make a new empty python dictionary for this table.
    py::dict dict;

    // Insert this dictionary into the current parent container.
    add_object(key, dict);

    dicts_.push_back(std::move(dict));
  }

  void
  PythonDictConverter::enter_sequence(key_t const& key, any_t const& any)
  {
    // Get length of sequence.
    auto const& anyvec = std::any_cast<std::vector<any_t> const&>(any);
    unsigned int const n = anyvec.size();

    // Make a new python list of the required size.
    py::list list{n};

    // Insert the list into the current parent container.
    add_object(key, list);

    // Make the newly created python dictionary the current parent container.
    lists_.push_back(std::move(list));
  }

  void
  PythonDictConverter::atom(key_t const& key, any_t const& any)
  {
    // Extract atom as string.
    auto const& atom = std::any_cast<std::string const&>(any);

    // Get lower case version of argument string.
    std::string lcatom{atom};
    std::transform(lcatom.begin(),
                   lcatom.end(),
                   lcatom.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    // Check for boolean.
    if (lcatom == std::string("true") || lcatom == std::string("\"true\"")) {
      return add_object(key, py::bool_{true});
    } else if (lcatom == std::string("false") ||
               lcatom == std::string("\"false\"")) {
      return add_object(key, py::bool_{false});
    }

    // Check for quoted string.
    auto const n = atom.size();
    if (n >= 2 && atom[0] == '"' && atom[n - 1] == '"') {
      std::string s = atom.substr(1, n - 2);
      return add_object(key, py::str(s));
    }

    // Check for int.
    {
      std::istringstream iss(atom);
      long i;
      iss >> std::noskipws >> i;
      if (iss.eof() && !iss.fail()) {
        return add_object(key, py::int_(i));
      }
    }

    // Check for float.
    {
      std::istringstream iss(atom);
      double x;
      iss >> std::noskipws >> x;
      if (iss.eof() && !iss.fail()) {
        return add_object(key, py::float_(x));
      }
    }

    // Last resort store a copy of the original string (unquoted string).
    add_object(key, py::str(atom));
  }

  void
  PythonDictConverter::exit_table(key_t const&, any_t const&)
  {
    if (dicts_.size() < 2)
      throw cet::exception("fclmodule")
        << "Result stack has wrong size: " << dicts_.size() << std::endl;

    // Pop the current parent (this table) off the result stack.
    dicts_.pop_back();
  }

  void
  PythonDictConverter::exit_sequence(key_t const&, any_t const&)
  {
    if (lists_.empty())
      throw cet::exception("fclmodule")
        << "Result stack has wrong size: " << dicts_.size() << std::endl;

    // Pop the current parent (this sequence) off the result stack.
    lists_.pop_back();
  }

  void
  PythonDictConverter::add_object(key_t const& key, py::object pyobj)
  //
  // Purpose: Add object to the current parent container.  The parent object
  //          can be either a python dictionary or a python list.  The key
  //          argument is only used if the parent is a dictionary.
  //
  // Arguments: key   - Key of object in parent.
  //            pyobj - Python object.
  //
  {
    assert(!dicts_.empty());
    if (!fhicl::detail::is_sequence_element(key)) {
      auto& parent = dicts_.back();
      parent[py::str(key)] = pyobj;
      return;
    }

    assert(!lists_.empty());
    auto& parent_list = lists_.back();
    auto const i = fhicl::detail::index_for_sequence_element(key);
    parent_list[i] = pyobj;
  }

  py::object
  format(py::handle const obj,
         unsigned int pos,
         unsigned int const indent,
         unsigned int const maxlen)
  //
  // Purpose: Convert a python object to a prettified string.  The resulting
  //          string is supposed to be valid python code.
  //
  // Arguments: obj    - Object to be formatted.
  //            pos    - Current character position (number of characters
  //                     printed since the last newline).
  //            indent - Indentation level (spaces) for multiline formatting.
  //            maxlen - Maximum line length before breaking.
  //
  // Returns: c++ string.
  //
  // Usage:
  //
  // This function is designed to call itself recursively in order to descend
  // into structured objects like dictionaries and sequences.
  //
  // Dictionaries and sequences may be printed in either single-line or
  // multiline format, depending on the complexity of the contained objects, and
  // the indent and maxlen parameters.
  //
  {
    // Result string stream.
    using py::isinstance;
    std::ostringstream ss;
    if (isinstance<py::bool_>(obj)) {
      ss << (obj.cast<py::bool_>() ? "True" : "False");
    } else if (isinstance<py::int_>(obj)) {
      ss << obj.cast<long>();
    } else if (isinstance<py::float_>(obj)) {
      ss << obj.cast<double>();
    } else if (isinstance<py::str>(obj)) {
      // String objects, add single quotes, but don't do any other formatting.
      ss << "'" << obj.cast<std::string>() << "'";
    } else if (isinstance<py::dict>(obj)) {
      auto const dict = obj.cast<py::dict>();
      auto const n = dict.size();
      // Always print dictionary objects in multiline format, one key per line.
      // Make a first pass over the list of keys to determine the maximum length
      // key.  Keys are assumed to be strings.
      std::size_t keymaxlen{};
      for (auto const& pr : dict) {
        auto const key_str = pr.first.cast<std::string>();
        if (key_str.size() > keymaxlen)
          keymaxlen = key_str.size();
      }

      // Second pass, loop over keys and values and convert them to strings.
      char sep = '{';
      for (auto const& [key, value] : dict) {
        auto const key_str = key.cast<std::string>();
        auto const py_value =
          format(value, indent + keymaxlen + 7, indent + 2, maxlen);
        auto const value_str = py_value.cast<std::string>();
        std::string ksquote = std::string("'") + key_str + std::string("'");
        ss << sep << '\n'
           << std::setw(indent + 2) << "" << std::setw(keymaxlen + 2)
           << std::left << ksquote << " : " << value_str;
        sep = ',';
      }
      if (n == 0)
        ss << "{}";
      else
        ss << '\n' << std::setw(indent + 1) << std::right << '}';
    } else if (isinstance<py::list>(obj) || isinstance<py::tuple>(obj)) {
      bool const is_list = isinstance<py::list>(obj);

      // Sequence printing handled here.
      // Break lines only when position exceeds maxlen.
      char const open_seq = is_list ? '[' : '(';
      char const close_seq = is_list ? ']' : ')';
      auto const seq = obj.cast<py::sequence>();

      // Loop over elements of this sequence.
      std::string sep(1, open_seq);
      unsigned int i{};
      unsigned int const break_indent = pos + 1;
      for (auto const ele : seq) {
        ss << sep;
        pos += sep.size();

        // Get the formatted string representation of this object.
        auto const value_str = format(ele, pos, break_indent, maxlen);
        auto f = value_str.cast<std::string>();

        // Get the number of characters before the first newline.
        auto const fs = f.size();
        auto const n1 = std::min(f.find('\n'), fs);

        // Decide if we want to break the line before printing this element.
        // Never break at the first element of a sequence.
        // Force a break (except at first element) if this is a structured
        // element. If we do break here, reformat this element with the updated
        // position.
        bool const force_break{isinstance<py::list>(ele) ||
                               isinstance<py::tuple>(ele) ||
                               isinstance<py::dict>(ele)};
        if (i > 0 && (force_break || pos + n1 > maxlen)) {
          ss << '\n' << std::setw(break_indent) << "";
          pos = break_indent;
          auto const value_str = format(ele, pos, break_indent, maxlen);
          f = value_str.cast<std::string>();
        }
        ++i;

        // Print this element
        ss << f;

        // Update the current character position, taking into account
        // whether the string we just printed contains a newline.
        auto const n2 = f.find_last_of('\n');
        if (n2 >= fs)
          pos += fs;
        else
          pos = fs - n2 - 1;

        sep = std::string(", ");
      }

      // Close sequence.
      if (seq.size() == 0ull) {
        ss << open_seq;
      }
      ss << close_seq;
    } else {
      // Not supported by FHiCL
      return py::none{};
    }

    return py::str{ss.str()};
  }

  py::dict
  make_pset(std::string const& filename)
  {
    fhicl::ParameterSet pset;
    cet::filepath_lookup maker{"FHICL_FILE_PATH"};

    fhicl::make_ParameterSet(filename, maker, pset);
    PythonDictConverter converter;
    pset.walk(converter);
    return converter.result();
  }

  py::object
  pretty(py::dict dict)
  {
    return format(dict, 0, 0, 80);
  }
}

PYBIND11_MODULE(fhicl, m)
{
  m.doc() = "Extention module for reading a FHiCL configuration file into a "
            "native Python dictionary.";
  m.def("make_pset", &make_pset, "Make dictionary from parameter set.");
  m.def("pretty",
        &pretty,
        "Print the in-memory dictionary.  Note that what is printed is not a "
        "FHiCL-conforming string.");
  py::register_exception<cet::exception>(m, "CetException");
}
