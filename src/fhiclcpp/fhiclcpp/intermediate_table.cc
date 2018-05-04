// ======================================================================
//
// intermediate_table
//
// ======================================================================

#include "fhiclcpp/intermediate_table.h"

#include "boost/algorithm/string.hpp"
#include "boost/any.hpp"
#include "fhiclcpp/exception.h"

#include <cctype>
#include <cstdlib>
#include <utility>

using namespace fhicl;

typedef intermediate_table::atom_t atom_t;
typedef intermediate_table::complex_t complex_t;
typedef intermediate_table::sequence_t sequence_t;
typedef intermediate_table::table_t table_t;

typedef intermediate_table::iterator iterator;
typedef intermediate_table::const_iterator const_iterator;

// ----------------------------------------------------------------------
namespace {
  extended_value const&
  nil_item()
  {
    static extended_value const nil_item(false, NIL, std::string("@nil"));
    return nil_item;
  }

  extended_value const&
  empty_seq()
  {
    static extended_value const empty_seq(false, SEQUENCE, sequence_t());
    return empty_seq;
  }

  extended_value const&
  empty_tbl()
  {
    static extended_value const empty_tbl(false, TABLE, table_t());
    return empty_tbl;
  }
}

// ----------------------------------------------------------------------

intermediate_table::intermediate_table() : ex_val(false, TABLE, table_t()) {}

// ----------------------------------------------------------------------

const_iterator
intermediate_table::begin() const
{
  return boost::any_cast<table_t const&>(ex_val.value).begin();
}

const_iterator
intermediate_table::end() const
{
  return boost::any_cast<table_t const&>(ex_val.value).end();
}

// ----------------------------------------------------------------------

bool
intermediate_table::empty() const
{
  return boost::any_cast<table_t const&>(ex_val.value).empty();
}

// ----------------------------------------------------------------------

bool
intermediate_table::insert(std::string const& name,
                           bool in_prolog,
                           value_tag tag,
                           boost::any const& value)
{
  return insert(name, extended_value(in_prolog, tag, value));
}

bool
intermediate_table::insert(std::string const& name, extended_value const& value)
{
  auto found_val = pre_insert_(name, value);
  bool result = (found_val != nullptr);
  if (result) {
    *found_val = value;
  }
  return result;
}

bool
intermediate_table::insert(std::string const& name, extended_value&& value)
{
  auto found_val = pre_insert_(name, value);
  bool result = (found_val != nullptr);
  if (result) {
    *found_val = std::move(value);
  }
  return result;
}

// ----------------------------------------------------------------------

// ----------------------------------------------------------------------

extended_value const&
intermediate_table::find(std::string const& name) const
{
  std::vector<std::string> const& key = split(name);
  extended_value const* p = &ex_val;
  for (unsigned k = 0, sz = key.size(); k != sz; ++k) {
    std::string const& this_key = key[k];
    if (this_key.empty())
      ;
    else if (std::isdigit(this_key[0])) {
      if (!p->is_a(SEQUENCE))
        throw exception(cant_find, name)
          << "-- not a sequence (at part \"" << this_key << "\")";
      sequence_t const& s = boost::any_cast<sequence_t const&>(p->value);
      unsigned i = std::atoi(this_key.c_str());
      if (s.size() <= i)
        throw exception(cant_find, name) << "(at part \"" << this_key << "\")";
      p = &s[i];
    } else { /* this_key[0] is alpha or '_' */
      if (!p->is_a(TABLE))
        throw exception(cant_find, name)
          << "-- not a table (at part \"" << this_key << "\")";
      table_t const& t = boost::any_cast<table_t const&>(p->value);
      const_iterator it = t.find(this_key);
      if (it == t.end())
        throw exception(cant_find, name) << "(at part \"" << this_key << "\")";
      p = &it->second;
    }
  } // for
  return *p;
} // find()

// ----------------------------------------------------------------------

bool
intermediate_table::exists(std::string const& name) const
{
  std::vector<std::string> const& key = split(name);
  extended_value const* p = &ex_val;
  for (unsigned k = 0, sz = key.size(); k != sz; ++k) {
    std::string const& this_key = key[k];
    if (this_key.empty())
      ;
    else if (std::isdigit(this_key[0])) {
      if (!p->is_a(SEQUENCE)) {
        return false;
      }
      sequence_t const& s = boost::any_cast<sequence_t const&>(p->value);
      unsigned i = std::atoi(this_key.c_str());
      if (s.size() <= i) {
        return false;
      }
      p = &s[i];
    } else { /* this_key[0] is alpha or '_' */
      if (!p->is_a(TABLE)) {
        return false;
      }
      table_t const& t = boost::any_cast<table_t const&>(p->value);
      const_iterator it = t.find(this_key);
      if (it == t.end()) {
        return false;
      }
      p = &it->second;
    }
  } // for
  return true;
} // exists()

void
intermediate_table::erase(std::string const& name, bool in_prolog)
{
  // A missing part of the "a.b.c.d" chain will not cause an error; it
  // is an error for an intermediate link in the chain *not* to be a
  // table.
  auto const& key(split(name));
  auto p(&ex_val);
  auto t(boost::any_cast<table_t>(&p->value));
  auto it(t->end());
  if ((!in_prolog) &&
      (((it = t->find(key[0])) == t->end()) || it->second.in_prolog)) {
    return;
  }
  bool at_sequence(false);
  for (auto const& this_key : key) {
    if (this_key.empty())
      ;
    else if (std::isdigit(this_key[0])) {
      if (!p->is_a(SEQUENCE))
        throw exception(cant_find, name)
          << "-- not a sequence (at part \"" << this_key << "\")";
      auto& s = boost::any_cast<sequence_t&>(p->value);
      unsigned i = std::atoi(this_key.c_str());
      if (s.size() <= i) {
        return;
      }
      p = &s[i];
      at_sequence = true;
    } else { /* this_key[0] is alpha or '_' */
      if (!p->is_a(TABLE))
        throw exception(cant_find, name)
          << "-- not a table (at part \"" << this_key << "\")";
      at_sequence = false;
      t = boost::any_cast<table_t>(&p->value);
      it = t->find(this_key);
      if (it == t->end()) {
        return;
      }
      p = &it->second;
      auto prot = p->protection;
      if (prot == Protection::PROTECT_ERROR) {
        throw exception(protection_violation)
          << ((this_key != name) ? (std::string("Part \"") + this_key +
                                    "\" of specification to be erased\n") :
                                   "")
          << '"' << name << "\" is protected on " << p->pretty_src_info()
          << '\n';
      } else if (prot == Protection::PROTECT_IGNORE) {
        return;
      }
    }
  } // for
  if (at_sequence) {
    throw fhicl::exception(unimplemented, "erase sequence member");
  }
  if (it != t->end()) {
    t->erase(it);
  }
}

extended_value*
intermediate_table::pre_insert_(std::string const& name,
                                extended_value const& value)
{
  if (!value.in_prolog) {
    table_t& t = boost::any_cast<table_t&>(ex_val.value);
    std::vector<std::string> const& key = split(name);
    iterator it = t.find(key[0]);
    if (it != t.end() && it->second.in_prolog) {
      t.erase(it);
    }
  }
  auto located = locate_(name, value.in_prolog);
  if ((!located.first->is_a(NIL)) &&
      value.protection > located.first->protection) {
    throw exception(protection_violation)
      << '"' << name << "\" cannot be assigned with increased protection"
      << "\n(previous definition on " << (located.first)->pretty_src_info()
      << ")\n";
  }
  return located.second ? located.first : nullptr;
}

std::pair<extended_value*, bool>
intermediate_table::locate_(std::string const& name, bool const in_prolog)
{
  std::pair<extended_value*, bool> result(nullptr, true);
  std::vector<std::string> const& key = split(name);
  extended_value*& p = result.first;
  p = &ex_val;
  for (unsigned k = 0, sz = key.size(); k != sz; ++k) {
    std::string const& this_key = key[k];
    if (this_key.empty())
      ;
    else if (std::isdigit(this_key[0])) {
      if (p->is_a(NIL)) {
        *p = empty_seq();
      }
      if (!p->is_a(SEQUENCE))
        throw exception(cant_find, name)
          << "-- not a sequence (at part \"" << this_key << "\")";
      auto& s = boost::any_cast<sequence_t&>(p->value);
      unsigned i = std::atoi(this_key.c_str());
      while (s.size() <= i) {
        s.push_back(nil_item());
      }
      p->set_prolog(in_prolog);
      p = &s[i];
    } else { /* this_key[0] is alpha or '_' */
      if (p->is_a(NIL)) {
        *p = empty_tbl();
        p->set_prolog(in_prolog);
      }
      if (!p->is_a(TABLE))
        throw exception(cant_find, name)
          << "-- not a table (at part \"" << this_key << "\")";
      auto& t = boost::any_cast<table_t&>(p->value);
      // This will do what we need whether the key is already in the map
      // or not.
      p = &t.emplace(this_key, nil_item()).first->second;
      p->set_prolog(in_prolog);
    }
    auto prot = p->protection;
    if (prot == Protection::PROTECT_ERROR) {
      throw exception(protection_violation)
        << ((this_key != name) ? (std::string("Part \"") + this_key +
                                  "\" of specification to be overwritten\n") :
                                 "")
        << '"' << name << "\" is protected on " << p->pretty_src_info() << '\n';
    } else if (prot == Protection::PROTECT_IGNORE) {
      result.second = false;
      // Keep going since we might need definition location information
      // for specified item upstream.
    }
  } // for
  return result;
} // locate_()

std::vector<std::string>
intermediate_table::split(std::string const& name) const
{
  std::vector<std::string> result;
  boost::algorithm::split(
    result,
    name,
    boost::algorithm::is_any_of(shims::isSnippetMode() ? "[]" : ".[]"));
  return result;
}

// ======================================================================
