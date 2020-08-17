#ifndef fhiclcpp_intermediate_table_h
#define fhiclcpp_intermediate_table_h

////////////////////////////////////////////////////////////////////////
// intermediate table: generic representation of parameters.
//
// This class should only be used in the following two circumstances:
//
// 1. Configuration post-processing, prior to ParameterSet creation
//    (use the "simple interface" where possible).
//
// 2. Internally by FHiCL.
//
// In all other circumstances, ParameterSet is almost certainly more
// appropriate.
//
// 2012/08/29 CG.
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// Supplemental expert notes on intermediate_table vs ParameterSet.
//
// 1. Intermediate tables contain only extended values; ParameterSets
//    contain only std::any.
//
// 2. The std::any in a ParameterSet may not be the same type as the
//    std::any in the corresponding extended_value in the intermediate
//    table whence it came.
//
// 3. An extended_value::sequence_t is std::vector<extended_value>; a
//    ParameterSet::ps_sequence_t is std::vector<std::any>.
//
// 4. An extended_value::table_t is std::map<std::string,
//    extended_value>; the equivalent concept in ParameterSet is
//    ParameterSet (stored as std::any).
//
// 5. An extended_value::complex_t is std::pair<std::string,
//    std::string>; the equivalent concept in ParameterSet is
//    std::string (stored as std::any).
//
// 6. Numbers, boolean values and strings are to be stored in
//    intermediate_tables at all times in their canonical string form
//    (using detail::encode()); complex numbers are stored in
//    intermediate tables as a pair of strings representing the
//    canonical string forms of the real and imaginary parts. In
//    ParameterSets they are stored as a single std::string,
//    "(real,imag)".
//
////////////////////////////////////////////////////////////////////////

#include "fhiclcpp/coding.h"
#include "fhiclcpp/extended_value.h"
#include "fhiclcpp/fwd.h"
#include "fhiclcpp/type_traits.h"

#include <any>
#include <complex>
#include <map>
#include <string>
#include <vector>

// ----------------------------------------------------------------------

class fhicl::intermediate_table {
public:
  // Constructor
  intermediate_table();

  ////////////////////
  // Simple interface:
  bool empty() const;

  bool exists(std::string const& name) const;

  void erase(std::string const& name, bool in_prolog = false);

  template <typename T>
  T get(std::string const& name);

  bool put(std::string const& name,
           std::string const& value, // String.
           bool in_prolog = false);
  bool put(std::string const& name,
           char const* value, // String.
           bool in_prolog = false);
  bool put(std::string const& name,
           bool value, // Boolean.
           bool in_prolog = false);
  template <typename T>
  bool put(std::string const& name,
           std::complex<T> const& value, // Complex.
           bool in_prolog = false);
  template <typename T>
  bool put(std::string const& name,
           std::vector<T> const& value, // Sequence.
           bool in_prolog = false);
  template <typename T>
  typename std::enable_if<tt::is_numeric<T>::value, bool>::type put(
    std::string const& name,
    T value, // Number
    bool in_prolog = false);

  bool putEmptySequence(std::string const& name,
                        bool in_prolog = false); // Empty Sequence.
  bool putEmptyTable(std::string const& name,
                     bool in_prolog = false); // Empty Table.
  bool putNil(std::string const& name,
              bool in_prolog = false); // Nil.

  ////////////////////
  // Expert interface.
  // Typedefs.
  using atom_t = extended_value::atom_t;
  using complex_t = extended_value::complex_t;
  using sequence_t = extended_value::sequence_t;
  using table_t = extended_value::table_t;
  using iterator = table_t::iterator;
  using const_iterator = table_t::const_iterator;

  const_iterator begin() const;
  const_iterator end() const;

  // Flexible insert interface.
  bool insert(std::string const& name,
              bool in_prolog,
              value_tag tag,
              std::any const& value);
  bool insert(std::string const& name, extended_value const& value);
  bool insert(std::string const& name, extended_value&& value);

  /// \throw if item does not exist.
  extended_value const& find(std::string const& name) const;

  /// \return nullptr if not able to be updated.
  extended_value* locate(std::string const& name);

  /// \throw if not able to be updated.
  extended_value& update(std::string const& name);

private:
  // Do all the work required to find somewhere to put the new
  // value. Called by insert().
  extended_value* pre_insert_(std::string const& name,
                              extended_value const& value);

  // Return an item with a bool indicating whether it may be updated.
  std::pair<extended_value*, bool> locate_(std::string const& name,
                                           bool in_prolog = false);

  std::vector<std::string> split(std::string const& name) const;

  extended_value ex_val;

}; // intermediate_table

namespace fhicl {
  namespace detail {

    // Template declaration (no general definition).
    template <typename T, typename Enable = void>
    class it_value_get;

    // Partial specialization for value types.
    template <typename T>
    class it_value_get<
      T,
      typename tt::disable_if<std::is_reference<T>::value ||
                              std::is_pointer<T>::value>::type> {
    public:
      T
      operator()(intermediate_table& table, std::string const& name)
      {
        T result;
        detail::decode(table.find(name).value, result);
        return result;
      }
    };

    // Partial specialization for std::complex<U>.
    template <typename U>
    class it_value_get<
      std::complex<U>,
      typename tt::disable_if<std::is_reference<std::complex<U>>::value ||
                              std::is_pointer<std::complex<U>>::value>::type> {
    public:
      std::complex<U>
      operator()(intermediate_table& table, std::string const& name)
      {
        intermediate_table::complex_t c(table.find(name));
        U r, i;
        detail::decode(c.first, r);
        detail::decode(c.second, i);
        return std::complex<U>(r, i);
      }
    };

    // Full specialization for sequence_t
    template <>
    class it_value_get<intermediate_table::sequence_t> {
    public:
      intermediate_table::sequence_t
      operator()(intermediate_table& table, std::string const& name)
      {
        return std::any_cast<intermediate_table::sequence_t>(
          table.find(name).value);
      }
    };

    // Full specialization for sequence_t &: will throw if not writable
    template <>
    class it_value_get<intermediate_table::sequence_t&> {
    public:
      intermediate_table::sequence_t&
      operator()(intermediate_table& sequence, std::string const& name)
      {
        auto item = sequence.locate(name);
        if (item != nullptr) {
          return std::any_cast<intermediate_table::sequence_t&>(item->value);
        } else {
          throw fhicl::exception(protection_violation)
            << "Requested non-updatable parameter \"" << name
            << "\" for update.\n";
        }
      }
    };

    // Full specialization for sequence_t const &
    template <>
    class it_value_get<intermediate_table::sequence_t const&> {
    public:
      intermediate_table::sequence_t const&
      operator()(intermediate_table const& table, std::string const& name)
      {
        return std::any_cast<intermediate_table::sequence_t const&>(
          table.find(name).value);
      }
    };

    // Full specialization for table_t
    template <>
    class it_value_get<intermediate_table::table_t> {
    public:
      intermediate_table::table_t
      operator()(intermediate_table& table, std::string const& name)
      {
        return std::any_cast<intermediate_table::table_t>(
          table.find(name).value);
      }
    };

    // Full specialization for table_t &: will throw if not writable
    template <>
    class it_value_get<intermediate_table::table_t&> {
    public:
      intermediate_table::table_t&
      operator()(intermediate_table& table, std::string const& name)
      {
        auto item = table.locate(name);
        if (item != nullptr) {
          return std::any_cast<intermediate_table::table_t&>(item->value);
        } else {
          throw fhicl::exception(protection_violation)
            << "Requested non-updatable parameter " << name << " for update.\n";
        }
      }
    };

    // Full specialization for table_t const &
    template <>
    class it_value_get<intermediate_table::table_t const&> {
    public:
      intermediate_table::table_t const&
      operator()(intermediate_table const& table, std::string const& name)
      {
        return std::any_cast<intermediate_table::table_t const&>(
          table.find(name).value);
      }
    };
  }
}

template <typename T>
inline T
fhicl::intermediate_table::get(std::string const& name)
{
  static detail::it_value_get<T> getter;
  return getter(*this, name);
}

inline bool
fhicl::intermediate_table::put(std::string const& name,
                               std::string const& value, // String.
                               bool in_prolog)
{
  return insert(name, in_prolog, STRING, detail::encode(value));
}

inline bool
fhicl::intermediate_table::put(std::string const& name,
                               char const* value, // String.
                               bool const in_prolog)
{
  return insert(name, in_prolog, STRING, detail::encode(value));
}

inline bool
fhicl::intermediate_table::put(std::string const& name,
                               bool const value, // Boolean.
                               bool const in_prolog)
{
  return insert(name, in_prolog, BOOL, detail::encode(value));
}

template <typename T>
bool
fhicl::intermediate_table::put(std::string const& name,
                               std::complex<T> const& value, // Complex.
                               bool const in_prolog)
{
  return insert(
    name,
    in_prolog,
    COMPLEX,
    complex_t(detail::encode(value.real()), detail::encode(value.imag())));
}

template <typename T>
inline bool
fhicl::intermediate_table::put(std::string const& name,
                               std::vector<T> const& value, // Sequence.
                               bool const in_prolog)
{
  bool result = putEmptySequence(name, in_prolog);
  if (!result) {
    return result;
  }
  size_t count(0);
  for (auto const& item : value) {
    result = result &&
             put(name + "[" + std::to_string(count++) + "]", item, in_prolog);
  }
  return result;
}

template <typename T>
inline typename std::enable_if<tt::is_numeric<T>::value, bool>::type
fhicl::intermediate_table::put(std::string const& name,
                               T const value, // Number
                               bool const in_prolog)
{
  return insert(name, in_prolog, NUMBER, detail::encode(value));
}

inline bool fhicl::intermediate_table::putEmptySequence(
  std::string const& name,
  bool const in_prolog) // Sequence.
{
  return insert(name, in_prolog, SEQUENCE, sequence_t{});
}

inline bool fhicl::intermediate_table::putEmptyTable(
  std::string const& name,
  bool const in_prolog) // Table.
{
  return insert(name, in_prolog, TABLE, table_t{});
}

inline bool fhicl::intermediate_table::putNil(std::string const& name,
                                              bool const in_prolog) // Nil.
{
  return insert(name, in_prolog, NIL, detail::encode((void*)0));
}

inline fhicl::extended_value*
fhicl::intermediate_table::locate(std::string const& name)
{
  extended_value* result = nullptr;
  auto located = locate_(name);
  if (located.second) {
    result = located.first;
  }
  return result;
}

inline fhicl::extended_value&
fhicl::intermediate_table::update(std::string const& name)
{
  auto located = locate_(name);
  if (!located.second) {
    throw exception(protection_violation)
      << "Requested non-modifiable item \"" << name << "\" for update.\n";
  }
  return *located.first;
}

#endif /* fhiclcpp_intermediate_table_h */

// Local Variables:
// mode: c++
// End:
