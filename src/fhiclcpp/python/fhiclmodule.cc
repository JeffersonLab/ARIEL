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
#if GCC_IS_AT_LEAST(7,1,0)
#pragma GCC diagnostic ignored "-Wregister"
#endif
#include "Python.h" // Needs to be (almost) first due to macro definition issues
#pragma GCC diagnostic pop
#include "cetlib/search_path.h"
#include "cetlib_except/exception.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/ParameterSetWalker.h"
#include "fhiclcpp/make_ParameterSet.h"

#include <iomanip>
#include <iostream>
#include <sstream>

class PythonDictConverter : public fhicl::ParameterSetWalker {
public:
  using key_t = std::string;
  using any_t = boost::any;

  // Public methods.

  PythonDictConverter();
  PyObject* result() const; // Result is python dictionary.

private:
  // Base class overrides.
  void enter_table(key_t const& key, any_t const& any) override;
  void enter_sequence(key_t const& key, any_t const& any) override;
  void atom(key_t const& key, any_t const& any) override;
  void exit_table(key_t const& key, any_t const& any) override;
  void exit_sequence(key_t const& key, any_t const& any) override;

  void add_object(key_t const& key,
                  PyObject* pyobj); // Add object to current parent.

  // Data members.

  // Result stack.
  // stack_[0] is the entire parameter set (a python dictionary).
  // stack_.back() is the current parent container that is being filled
  // (a python dictionary or list).
  std::vector<PyObject*> stack_;
};

PythonDictConverter::PythonDictConverter()
{
  // Initialize result stack with an empty python dictionary.
  // This dictionary will represent the parameter set.
  stack_.emplace_back(PyDict_New());
}

PyObject*
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

  if (stack_.size() != 1)
    throw cet::exception("fclmodule")
      << "Result stack has wrong size: " << stack_.size() << std::endl;
  if (!PyDict_Check(stack_[0]))
    throw cet::exception("fclmodule")
      << "Result stack has wrong type." << std::endl;

  // Everything OK.

  return stack_[0];
}

void
PythonDictConverter::enter_table(key_t const& key, any_t const&)
//
// Purpose: Convert table.
//
// Arguments: key - Key of this object.
//            any - Object
//
{
  // Make a new empty python dictionary for this table.
  PyObject* dict = PyDict_New();

  // Insert this dictionary into the current parent container.
  add_object(key, dict);

  // Make the newly created python dictionary the current parent container.
  stack_.emplace_back(dict);
}

void
PythonDictConverter::enter_sequence(key_t const& key, any_t const& any)
//
// Purpose: Convert sequence.
//
// Arguments: key - Key of this object.
//            any - Object
//
{
  // Get length of sequence.
  auto const& anyvec = boost::any_cast<std::vector<any_t> const&>(any);
  unsigned int n = anyvec.size();

  // Make a new python list of the required size.
  PyObject* seq = PyList_New(n);

  // Insert the list into the current parent container.
  add_object(key, seq);

  // Make the newly created python dictionary the current parent container.
  stack_.emplace_back(seq);
}

void
PythonDictConverter::atom(key_t const& key, any_t const& any)
//
// Purpose: Convert atom.
//
// Arguments: key - Key of this object.
//            any - Object
//
{
  PyObject* pyval{nullptr};

  // Extract atom as string.
  auto const& atom = boost::any_cast<std::string const&>(any);

  // Get lower case version of argument string.
  std::string lcatom{atom};
  std::transform(lcatom.begin(),
                 lcatom.end(),
                 lcatom.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  // Check for boolean.
  if (lcatom == std::string("true") || lcatom == std::string("\"true\"")) {
    pyval = Py_True;
    Py_INCREF(pyval);
  } else if (lcatom == std::string("false") ||
             lcatom == std::string("\"false\"")) {
    pyval = Py_False;
    Py_INCREF(pyval);
  }

  // Check for quoted string.
  auto const n = atom.size();
  if (pyval == nullptr && n >= 2 && atom[0] == '"' && atom[n - 1] == '"') {
    std::string s = atom.substr(1, n - 2);
    pyval = PyString_FromString(s.c_str());
  }

  // Check for int.
  if (pyval == nullptr) {
    std::istringstream iss(atom);
    long i;
    iss >> std::noskipws >> i;
    if (iss.eof() && !iss.fail())
      pyval = PyInt_FromLong(i);
  }

  // Check for float.
  if (pyval == nullptr) {
    std::istringstream iss(atom);
    double x;
    iss >> std::noskipws >> x;
    if (iss.eof() && !iss.fail())
      pyval = PyFloat_FromDouble(x);
  }

  // Last resort store a copy of the original string (unquoted string).
  if (pyval == nullptr) {
    pyval = PyString_FromString(atom.c_str());
  }

  // Done converting atom to python.
  add_object(key, pyval);
}

void
PythonDictConverter::exit_table(key_t const&, any_t const&)
//
// Purpose: Close parent table.
//
{
  // Do consistency checks.
  if (stack_.size() < 2)
    throw cet::exception("fclmodule")
      << "Result stack has wrong size: " << stack_.size() << std::endl;
  if (!PyDict_Check(stack_.back()))
    throw cet::exception("fclmodule")
      << "Result stack has wrong type." << std::endl;

  // Pop the current parent (this table) off the result stack.
  stack_.pop_back();
}

void
PythonDictConverter::exit_sequence(key_t const&, any_t const&)
//
// Purpose: Close current sequence.
//
// Arguments: key - Key of this object.
//            any - Object
//
{
  // Do consistency checks.
  if (stack_.size() < 2)
    throw cet::exception("fclmodule")
      << "Result stack has wrong size: " << stack_.size() << std::endl;
  if (!PyList_Check(stack_.back()))
    throw cet::exception("fclmodule")
      << "Result stack has wrong type." << std::endl;

  // Pop the current parent (this sequence) off the result stack.
  stack_.pop_back();
}

void
PythonDictConverter::add_object(key_t const& key, PyObject* pyobj)
//
// Purpose: Add object to the current parent container.  The parent object
//          can be either a python dictionary or a python list.  The key
//          argument is only used if the parent is a dictionary.
//
// Arguments: key   - Key of object in parent.
//            pyobj - Python object.
//
{
  // Get the current parent object.
  if (stack_.empty()) {
    throw cet::exception("fclmodule") << "No parent object." << std::endl;
  }
  PyObject* parent = stack_.back();

  if (PyDict_Check(parent)) {

    // Insert object into dicionary.
    PyDict_SetItemString(parent, key.c_str(), pyobj);
    Py_DECREF(pyobj);
  } else if (PyList_Check(parent)) {

    // Lists handled here.
    if (PyList_Size(parent) == 0)
      throw cet::exception("fclmodule") << "Parent list is empty." << std::endl;

    // Find the first uninitialized list entry (do binary search).
    unsigned int low = 0;
    unsigned int high = PyList_Size(parent) - 1;
    if (PyList_GetItem(parent, 0) == 0)
      high = 0;
    if (PyList_GetItem(parent, high) != 0)
      throw cet::exception("fclmodule")
        << "Parent list has no free entries." << std::endl;
    while (high - low > 1) {
      unsigned int mid = (low + high) / 2;
      if (PyList_GetItem(parent, mid) == 0)
        high = mid;
      else
        low = mid;
    }

    // Insert object into list at the appropriate position.
    PyList_SetItem(parent, high, pyobj);
  } else {
    throw cet::exception("fclmodule")
      << "Parent object is not dictionary or list." << std::endl;
  }
}

static std::string
format(PyObject* obj,
       unsigned int pos,
       unsigned int indent,
       unsigned int maxlen)
//
// Purpose: Convert a python object to a prettified string.  The resulting
// string
//          is supposed to be valid python code.
//
// Arguments: obj    - Object to be formatted.
//            pos    - Current character position (number of characters printed
//                     since the last newline).
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
// Dictionaries and sequences may be printed in either single-line or multiline
// format, depending on the complexity of the contained objects, and the indent
// and maxlen parameters.
//
{
  // Result string stream.
  std::ostringstream ss;

  if (PyString_Check(obj)) {
    // String objects, add single quotes, but don't do any other formatting.
    ss << "'" << PyString_AsString(obj) << "'";
  } else if (PyDict_Check(obj)) {
    // Always print dictionary objects in multiline format, one key per line.
    // Get list of keys.  Keys are assumed to be strings.
    PyObject* keys = PyDict_Keys(obj);

    // Make a first pass over the list of keys to determine the maximum length
    // key.
    int n = PyList_Size(keys);
    int keymaxlen = 0;
    for (int i = 0; i < n; ++i) {
      PyObject* key = PyList_GetItem(keys, i);
      int keylen = PyString_Size(key);
      if (keylen > keymaxlen)
        keymaxlen = keylen;
    }

    // Second pass, loop over keys and values and convert them to strings.
    char sep = '{';
    for (int i = 0; i < n; ++i) {
      PyObject* key = PyList_GetItem(keys, i);
      PyObject* value = PyDict_GetItem(obj, key);
      char const* ks = PyString_AsString(key);
      std::string ksquote =
        std::string("'") + std::string(ks) + std::string("'");
      ss << sep << '\n'
         << std::setw(indent + 2) << "" << std::setw(keymaxlen + 2) << std::left
         << ksquote << " : "
         << format(value, indent + keymaxlen + 7, indent + 2, maxlen);
      sep = ',';
    }
    if (n == 0)
      ss << "{}";
    else
      ss << '\n' << std::setw(indent + 1) << std::right << '}';

    Py_DECREF(keys);

  } else if (PyList_Check(obj) || PyTuple_Check(obj)) {

    // Sequence printing handled here.
    // Break lines only when position exceeds maxlen.
    char open_seq = 0;
    char close_seq = 0;
    int n = 0;
    if (PyList_Check(obj)) {
      open_seq = '[';
      close_seq = ']';
      n = PyList_Size(obj);
    } else {
      open_seq = '(';
      close_seq = ')';
      n = PyTuple_Size(obj);
    }

    // Loop over elements of this sequence.
    std::string sep(1, open_seq);
    unsigned int break_indent = pos + 1;
    for (int i = 0; i < n; ++i) {
      ss << sep;
      pos += sep.size();
      PyObject* ele = PySequence_GetItem(obj, i);

      // Get the formatted string representation of this object.
      std::string f{format(ele, pos, break_indent, maxlen)};

      // Get the number of characters before the first newline.
      auto const fs = f.size();
      auto const n1 = std::min(f.find('\n'), fs);

      // Decide if we want to break the line before printing this element.
      // Never break at the first element of a sequence.
      // Force a break (except at first element) if this is a structured
      // element. If we do break here, reformat this element with the updated
      // position.
      bool const force_break{PyList_Check(ele) || PyTuple_Check(ele) ||
                             PyDict_Check(ele)};
      if (i > 0 && (force_break || pos + n1 > maxlen)) {
        ss << '\n' << std::setw(break_indent) << "";
        pos = break_indent;
        f = format(ele, pos, break_indent, maxlen);
      }

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
      Py_DECREF(ele);
    }

    // Close sequence.
    if (n == 0)
      ss << open_seq;
    ss << close_seq;
  } else {

    // Last resort, use python's string representation.
    PyObject* pystr = PyObject_Str(obj);
    ss << PyString_AsString(pystr);
  }

  return ss.str();
}

static PyObject*
make_pset(PyObject*, PyObject* args)
//
// Purpose: Public module function to read fcl file and return a python
// dictionary.
//
// Arguments: self - Not used, because this is not a member function.
//            args - Argument tuple.  A single string representing the
//                   name of a fcl file.
// Returned value: A python dictionary.
//
{
  // Extract argument as string.
  char const* fclname;
  if (!PyArg_ParseTuple(args, "s", &fclname))
    return nullptr;
  std::string const fclstr{fclname};

  // Make parameter set.
  PyObject* result{nullptr};
  fhicl::ParameterSet pset;
  cet::filepath_lookup maker{"FHICL_FILE_PATH"};
  try {
    fhicl::make_ParameterSet(fclstr, maker, pset);
    PythonDictConverter converter;
    pset.walk(converter);
    result = converter.result();
  }
  catch (cet::exception const& e) {
    PyErr_SetString(PyExc_IOError, e.what());
    result = nullptr;
  }

  return result;
}

static PyObject*
pretty(PyObject*, PyObject* args)
//
// Purpose: Public module function to convert a python fcl dictionary to a
//          prettified string.
//
// Arguments: self - Not used, because this is not a member function.
//            args - Argument tuple.  This function expects a single
//                   python object of any type, but typically a dictionary.
//
// Returned value: A python string or none.
//
{
  PyObject* result{nullptr};

  // Extract argument.
  auto const n = PySequence_Length(args);
  if (n == 0) {
    // No arguments, return none.
    result = Py_None;
    Py_INCREF(result);
  } else {
    // Otherwise, extract the first element.
    PyObject* obj = PySequence_GetItem(args, 0);
    std::string const s{format(obj, 0, 0, 80)};
    result = PyString_FromString(s.c_str());
  }

  return result;
}

// Module method table.
static struct PyMethodDef fclmodule_methods[] = {
  {"make_pset", make_pset, METH_VARARGS, "Make dictionary from parameter set."},
  {"pretty", pretty, METH_VARARGS, "Print the in-memory dictionary."},
  {nullptr, nullptr, 0, nullptr}};

// Initialization function.
extern "C" {
void
initfhicl()
{
  Py_InitModule("fhicl", fclmodule_methods);
}
}
