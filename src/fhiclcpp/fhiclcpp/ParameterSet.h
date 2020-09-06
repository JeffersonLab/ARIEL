#ifndef fhiclcpp_ParameterSet_h
#define fhiclcpp_ParameterSet_h

// ======================================================================
//
// ParameterSet
//
// ======================================================================

#include "boost/lexical_cast.hpp"
#include "boost/numeric/conversion/cast.hpp"
#include "cetlib_except/demangle.h"
#include "fhiclcpp/ParameterSetID.h"
#include "fhiclcpp/coding.h"
#include "fhiclcpp/detail/ParameterSetImplHelpers.h"
#include "fhiclcpp/detail/encode_extended_value.h"
#include "fhiclcpp/detail/print_mode.h"
#include "fhiclcpp/detail/try_blocks.h"
#include "fhiclcpp/exception.h"
#include "fhiclcpp/extended_value.h"
#include "fhiclcpp/fwd.h"

#include <any>
#include <cctype>
#include <map>
#include <sstream>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

// ----------------------------------------------------------------------

class fhicl::ParameterSet {
public:
  using ps_atom_t = fhicl::detail::ps_atom_t;
  using ps_sequence_t = fhicl::detail::ps_sequence_t;
  using annot_t = std::unordered_map<std::string, std::string>;

  // compiler generates default c'tor, d'tor, copy c'tor, copy assignment

  // observers:
  bool is_empty() const;
  ParameterSetID id() const;

  std::string to_string() const;
  std::string to_compact_string() const;

  std::string to_indented_string() const;
  std::string to_indented_string(unsigned initial_indent_level) const;
  std::string to_indented_string(unsigned initial_indent_level,
                                 bool annotate) const;
  std::string to_indented_string(unsigned initial_indent_level,
                                 detail::print_mode pm) const;

  std::vector<std::string> get_names() const;
  std::vector<std::string> get_pset_names() const;
  std::vector<std::string> get_all_keys() const;

  // retrievers (nested key OK):
  bool has_key(std::string const& key) const;
  bool is_key_to_table(std::string const& key) const;
  bool is_key_to_sequence(std::string const& key) const;
  bool is_key_to_atom(std::string const& key) const;

  template <class T>
  bool get_if_present(std::string const& key, T& value) const;
  template <class T, class Via>
  bool get_if_present(std::string const& key,
                      T& value,
                      T convert(Via const&)) const;

  template <class T>
  T get(std::string const& key) const;
  template <class T, class Via>
  T get(std::string const& key, T convert(Via const&)) const;
  template <class T>
  T get(std::string const& key, T const& default_value) const;
  template <class T, class Via>
  T get(std::string const& key,
        T const& default_value,
        T convert(Via const&)) const;

  std::string get_src_info(std::string const& key) const;

  // Facility to traverse the ParameterSet tree
  void walk(ParameterSetWalker& psw) const;

  // inserters (key must be local: no nesting):
  void put(std::string const& key); // Implicit nil value.
  template <class T>                // Fail on preexisting key.
  void put(std::string const& key, T const& value);
  void put_or_replace(std::string const& key); // Implicit nil value.
  template <class T>                           // Succeed.
  void put_or_replace(std::string const& key, T const& value);
  template <class T> // Fail if preexisting key of incompatible type.
  void put_or_replace_compatible(std::string const& key, T const& value);

  // deleters:
  bool erase(std::string const& key);

  // comparators:
  bool operator==(ParameterSet const& other) const;
  bool operator!=(ParameterSet const& other) const;

private:
  using map_t = std::map<std::string, std::any>;
  using map_iter_t = map_t::const_iterator;

  map_t mapping_;
  annot_t srcMapping_;
  mutable ParameterSetID id_;

  // Private inserters.
  void insert_(std::string const& key, std::any const& value);
  void insert_or_replace_(std::string const& key, std::any const& value);
  void insert_or_replace_compatible_(std::string const& key,
                                     std::any const& value);

  std::string to_string_(bool compact = false) const;
  std::string stringify_(std::any const& a, bool compact = false) const;

  bool key_is_type_(std::string const& key,
                    std::function<bool(std::any const&)> func) const;

  // Local retrieval only.
  template <class T>
  bool get_one_(std::string const& key, T& value) const;
  bool find_one_(std::string const& key) const;
  bool descend_(std::vector<std::string> const& names, ParameterSet& ps) const;

}; // ParameterSet

// ======================================================================

inline std::string
fhicl::ParameterSet::to_string() const
{
  return to_string_();
}

inline std::string
fhicl::ParameterSet::to_compact_string() const
{
  return to_string_(true);
}

inline bool
fhicl::ParameterSet::is_key_to_table(std::string const& key) const
{
  return key_is_type_(key, &detail::is_table);
}

inline bool
fhicl::ParameterSet::is_key_to_sequence(std::string const& key) const
{
  return key_is_type_(key, &detail::is_sequence);
}

inline bool
fhicl::ParameterSet::is_key_to_atom(std::string const& key) const
{
  return key_is_type_(key, [](std::any const& a) {
    return !(detail::is_sequence(a) || detail::is_table(a));
  });
}

template <class T>
void
fhicl::ParameterSet::put(std::string const& key, T const& value)
{
  auto insert = [this, &value](auto const& key) {
    using detail::encode;
    this->insert_(key, std::any(encode(value)));
  };
  detail::try_insert(insert, key);
}

template <class T>
void
fhicl::ParameterSet::put_or_replace(std::string const& key, T const& value)
{
  auto insert_or_replace = [this, &value](auto const& key) {
    using detail::encode;
    this->insert_or_replace_(key, std::any(encode(value)));
    srcMapping_.erase(key);
  };
  detail::try_insert(insert_or_replace, key);
}

template <class T>
void
fhicl::ParameterSet::put_or_replace_compatible(std::string const& key,
                                               T const& value)
{
  auto insert_or_replace_compatible = [this, &value](auto const& key) {
    using detail::encode;
    this->insert_or_replace_compatible_(key, std::any(encode(value)));
    srcMapping_.erase(key);
  };
  detail::try_insert(insert_or_replace_compatible, key);
}

// ----------------------------------------------------------------------

template <class T>
bool
fhicl::ParameterSet::get_if_present(std::string const& key, T& value) const
{
  auto keys = detail::get_names(key);
  ParameterSet ps;
  return descend_(keys.tables(), ps) ? ps.get_one_(keys.last(), value) : false;
}

template <class T, class Via>
bool
fhicl::ParameterSet::get_if_present(std::string const& key,
                                    T& result,
                                    T convert(Via const&)) const
{
  Via go_between;
  if (!get_if_present(key, go_between)) {
    return false;
  }
  result = convert(go_between);
  return true;
} // get_if_present<>()

template <class T>
T
fhicl::ParameterSet::get(std::string const& key) const
{
  T result;
  return get_if_present(key, result) ? result :
                                       throw fhicl::exception(cant_find, key);
}

template <class T, class Via>
T
fhicl::ParameterSet::get(std::string const& key, T convert(Via const&)) const
{
  T result;
  return get_if_present(key, result, convert) ?
           result :
           throw fhicl::exception(cant_find, key);
}

template <class T>
T
fhicl::ParameterSet::get(std::string const& key, T const& default_value) const
{
  T result;
  return get_if_present(key, result) ? result : default_value;
}

template <class T, class Via>
T
fhicl::ParameterSet::get(std::string const& key,
                         T const& default_value,
                         T convert(Via const&)) const
{
  T result;
  return get_if_present(key, result, convert) ? result : default_value;
}

// ----------------------------------------------------------------------

inline bool
fhicl::ParameterSet::operator==(ParameterSet const& other) const
{
  return id() == other.id();
}

inline bool
fhicl::ParameterSet::operator!=(ParameterSet const& other) const
{
  return !operator==(other);
}

// ----------------------------------------------------------------------

template <class T>
bool
fhicl::ParameterSet::get_one_(std::string const& key, T& value) const try {
  auto skey = detail::get_sequence_indices(key);

  map_iter_t it = mapping_.find(skey.name());
  if (it == mapping_.end()) {
    return false;
  }

  auto a = it->second;
  if (!detail::find_an_any(skey.indices().cbegin(), skey.indices().cend(), a)) {
    throw fhicl::exception(error::cant_find);
  }

  using detail::decode;
  decode(a, value);
  return true;
}
catch (fhicl::exception const& e) {
  std::ostringstream errmsg;
  errmsg << "\nUnsuccessful attempt to convert FHiCL parameter '" << key
         << "' to type '" << cet::demangle_symbol(typeid(value).name())
         << "'.\n\n"
         << "[Specific error:]";
  throw fhicl::exception(type_mismatch, errmsg.str(), e);
}
catch (std::exception const& e) {
  std::ostringstream errmsg;
  errmsg << "\nUnsuccessful attempt to convert FHiCL parameter '" << key
         << "' to type '" << cet::demangle_symbol(typeid(value).name())
         << "'.\n\n"
         << "[Specific error:]\n"
         << e.what() << "\n\n";
  throw fhicl::exception(type_mismatch, errmsg.str());
}

// ----------------------------------------------------------------------

namespace fhicl {
  template <>
  void ParameterSet::put(std::string const& key,
                         fhicl::extended_value const& value);
}

// ======================================================================

#endif /* fhiclcpp_ParameterSet_h */

// Local Variables:
// mode: c++
// End:
