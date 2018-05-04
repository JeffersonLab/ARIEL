#include "cetlib_except/demangle.h"
#include "fhiclcpp/test/type_traits_old.h"
#include "fhiclcpp/type_traits.h"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <typeinfo>

template <typename T>
struct compare_is_int
  : public std::integral_constant<bool,
                                  tt::is_int<T>::value ==
                                    tt_old::is_int<T>::value> {
};

template <typename T>
struct compare_is_uint
  : public std::integral_constant<bool,
                                  tt::is_uint<T>::value ==
                                    tt_old::is_uint<T>::value> {
};

template <typename T>
struct compare_is_numeric
  : public std::integral_constant<bool,
                                  tt::is_numeric<T>::value ==
                                    tt_old::is_numeric<T>::value> {
};

template <typename T>
void
assert_cmp(bool exp, std::string const op)
{
  if (!exp) {
    throw std::string("Failed ") + op + " for type " +
      cet::demangle_symbol(typeid(T).name()) + ".";
  }
}

int
main() try {
  // Walter's old templates did not specialize for char, therefore
  // is_int<char> and is_uint<char> were both false.
  char x = -1;
  signed char y = -1;
  if (int(x) == int(y)) { // char == signed char.
    assert_cmp<char>(tt::is_int<char>::value, "is_int");
    assert_cmp<char const>(tt::is_int<char const>::value, "is_int");
    assert_cmp<char>(!tt::is_uint<char>::value, "is_uint");
    assert_cmp<char const>(!tt::is_uint<char const>::value, "is_uint");
  } else { // char == unsigned char.
    assert_cmp<char>(!tt::is_int<char>::value, "is_int");
    assert_cmp<char const>(!tt::is_int<char const>::value, "is_int");
    assert_cmp<char>(tt::is_uint<char>::value, "is_uint");
    assert_cmp<char const>(tt::is_uint<char const>::value, "is_uint");
  }
  assert_cmp<char>(tt::is_numeric<char>::value, "is_numeric");
  assert_cmp<char const>(tt::is_numeric<char const>::value, "is_numeric");
  // These types should all return the same value for new and old templates.
  assert_cmp<signed char>(compare_is_int<signed char>::value, "compare is_int");
  assert_cmp<signed char>(compare_is_uint<signed char>::value,
                          "compare is_uint");
  assert_cmp<signed char>(compare_is_numeric<signed char>::value,
                          "compare is_numeric");
  assert_cmp<signed char const>(compare_is_int<signed char const>::value,
                                "compare is_int");
  assert_cmp<signed char const>(compare_is_uint<signed char const>::value,
                                "compare is_uint");
  assert_cmp<signed char const>(compare_is_numeric<signed char const>::value,
                                "compare is_numeric");
  assert_cmp<unsigned char>(compare_is_int<unsigned char>::value,
                            "compare is_int");
  assert_cmp<unsigned char>(compare_is_uint<unsigned char>::value,
                            "compare is_uint");
  assert_cmp<unsigned char>(compare_is_numeric<unsigned char>::value,
                            "compare is_numeric");
  assert_cmp<unsigned char const>(compare_is_int<unsigned char const>::value,
                                  "compare is_int");
  assert_cmp<unsigned char const>(compare_is_uint<unsigned char const>::value,
                                  "compare is_uint");
  assert_cmp<unsigned char const>(
    compare_is_numeric<unsigned char const>::value, "compare is_numeric");
  assert_cmp<short>(compare_is_int<short>::value, "compare is_int");
  assert_cmp<short>(compare_is_uint<short>::value, "compare is_uint");
  assert_cmp<short>(compare_is_numeric<short>::value, "compare is_numeric");
  assert_cmp<short const>(compare_is_int<short const>::value, "compare is_int");
  assert_cmp<short const>(compare_is_uint<short const>::value,
                          "compare is_uint");
  assert_cmp<short const>(compare_is_numeric<short const>::value,
                          "compare is_numeric");
  assert_cmp<unsigned short>(compare_is_int<unsigned short>::value,
                             "compare is_int");
  assert_cmp<unsigned short>(compare_is_uint<unsigned short>::value,
                             "compare is_uint");
  assert_cmp<unsigned short>(compare_is_numeric<unsigned short>::value,
                             "compare is_numeric");
  assert_cmp<unsigned short const>(compare_is_int<unsigned short const>::value,
                                   "compare is_int");
  assert_cmp<unsigned short const>(compare_is_uint<unsigned short const>::value,
                                   "compare is_uint");
  assert_cmp<unsigned short const>(
    compare_is_numeric<unsigned short const>::value, "compare is_numeric");
  assert_cmp<int>(compare_is_int<int>::value, "compare is_int");
  assert_cmp<int>(compare_is_uint<int>::value, "compare is_uint");
  assert_cmp<int>(compare_is_numeric<int>::value, "compare is_numeric");
  assert_cmp<int const>(compare_is_int<int const>::value, "compare is_int");
  assert_cmp<int const>(compare_is_uint<int const>::value, "compare is_uint");
  assert_cmp<int const>(compare_is_numeric<int const>::value,
                        "compare is_numeric");
  assert_cmp<unsigned int>(compare_is_int<unsigned int>::value,
                           "compare is_int");
  assert_cmp<unsigned int>(compare_is_uint<unsigned int>::value,
                           "compare is_uint");
  assert_cmp<unsigned int>(compare_is_numeric<unsigned int>::value,
                           "compare is_numeric");
  assert_cmp<unsigned int const>(compare_is_int<unsigned int const>::value,
                                 "compare is_int");
  assert_cmp<unsigned int const>(compare_is_uint<unsigned int const>::value,
                                 "compare is_uint");
  assert_cmp<unsigned int const>(compare_is_numeric<unsigned int const>::value,
                                 "compare is_numeric");
  assert_cmp<long>(compare_is_int<long>::value, "compare is_int");
  assert_cmp<long>(compare_is_uint<long>::value, "compare is_uint");
  assert_cmp<long>(compare_is_numeric<long>::value, "compare is_numeric");
  assert_cmp<long const>(compare_is_int<long const>::value, "compare is_int");
  assert_cmp<long const>(compare_is_uint<long const>::value, "compare is_uint");
  assert_cmp<long const>(compare_is_numeric<long const>::value,
                         "compare is_numeric");
  assert_cmp<unsigned long>(compare_is_int<unsigned long>::value,
                            "compare is_int");
  assert_cmp<unsigned long>(compare_is_uint<unsigned long>::value,
                            "compare is_uint");
  assert_cmp<unsigned long>(compare_is_numeric<unsigned long>::value,
                            "compare is_numeric");
  assert_cmp<unsigned long const>(compare_is_int<unsigned long const>::value,
                                  "compare is_int");
  assert_cmp<unsigned long const>(compare_is_uint<unsigned long const>::value,
                                  "compare is_uint");
  assert_cmp<unsigned long const>(
    compare_is_numeric<unsigned long const>::value, "compare is_numeric");
  // Walter's old templates did not specialize for long long or unsigned
  // long long, therefore is_int and is_uint were both false.
  assert_cmp<long long>(tt::is_int<long long>::value, "is_int");
  assert_cmp<long long>(!tt::is_uint<long long>::value, "is_uint");
  assert_cmp<long long>(tt::is_numeric<long long>::value, "is_numeric");
  assert_cmp<long long const>(tt::is_int<long long const>::value, "is_int");
  assert_cmp<long long const>(!tt::is_uint<long long const>::value, "is_uint");
  assert_cmp<long long const>(tt::is_numeric<long long const>::value,
                              "is_numeric");
  assert_cmp<unsigned long long>(!tt::is_int<unsigned long long>::value,
                                 "is_int");
  assert_cmp<unsigned long long>(tt::is_uint<unsigned long long>::value,
                                 "is_uint");
  assert_cmp<unsigned long long>(tt::is_numeric<unsigned long long>::value,
                                 "is_numeric");
  assert_cmp<unsigned long long const>(
    !tt::is_int<unsigned long long const>::value, "is_int");
  assert_cmp<unsigned long long const>(
    tt::is_uint<unsigned long long const>::value, "is_uint");
  assert_cmp<unsigned long long const>(
    tt::is_numeric<unsigned long long const>::value, "is_numeric");
  // Walter's old templates did not specialize for int64_t or uint64_t,
  // meaning that is_int and is_uint were platform-dependent (hinging on
  // whether int64_t was typedef'd to long or to long long).
  assert_cmp<int64_t>(tt::is_int<int64_t>::value, "is_int");
  assert_cmp<int64_t>(!tt::is_uint<int64_t>::value, "is_uint");
  assert_cmp<int64_t>(tt::is_numeric<int64_t>::value, "is_numeric");
  assert_cmp<int64_t const>(tt::is_int<int64_t const>::value, "is_int");
  assert_cmp<int64_t const>(!tt::is_uint<int64_t const>::value, "is_uint");
  assert_cmp<int64_t const>(tt::is_numeric<int64_t const>::value, "is_numeric");
  assert_cmp<uint64_t>(!tt::is_int<uint64_t>::value, "is_int");
  assert_cmp<uint64_t>(tt::is_uint<uint64_t>::value, "is_uint");
  assert_cmp<uint64_t>(tt::is_numeric<uint64_t>::value, "is_numeric");
  assert_cmp<uint64_t const>(!tt::is_int<uint64_t const>::value, "is_int");
  assert_cmp<uint64_t const>(tt::is_uint<uint64_t const>::value, "is_uint");
  assert_cmp<uint64_t const>(tt::is_numeric<uint64_t const>::value,
                             "is_numeric");
  assert_cmp<float>(compare_is_int<float>::value, "compare is_int");
  assert_cmp<float>(compare_is_uint<float>::value, "compare is_uint");
  assert_cmp<float>(compare_is_numeric<float>::value, "compare is_numeric");
  assert_cmp<float const>(compare_is_int<float const>::value, "compare is_int");
  assert_cmp<float const>(compare_is_uint<float const>::value,
                          "compare is_uint");
  assert_cmp<float const>(compare_is_numeric<float const>::value,
                          "compare is_numeric");
  assert_cmp<double>(compare_is_int<double>::value, "compare is_int");
  assert_cmp<double>(compare_is_uint<double>::value, "compare is_uint");
  assert_cmp<double>(compare_is_numeric<double>::value, "compare is_numeric");
  assert_cmp<double const>(compare_is_int<double const>::value,
                           "compare is_int");
  assert_cmp<double const>(compare_is_uint<double const>::value,
                           "compare is_uint");
  assert_cmp<double const>(compare_is_numeric<double const>::value,
                           "compare is_numeric");
  assert_cmp<long double>(compare_is_int<long double>::value, "compare is_int");
  assert_cmp<long double>(compare_is_uint<long double>::value,
                          "compare is_uint");
  assert_cmp<long double>(compare_is_numeric<long double>::value,
                          "compare is_numeric");
  assert_cmp<long double const>(compare_is_int<long double const>::value,
                                "compare is_int");
  assert_cmp<long double const>(compare_is_uint<long double const>::value,
                                "compare is_uint");
  assert_cmp<long double const>(compare_is_numeric<long double const>::value,
                                "compare is_numeric");
}
catch (std::string const& msg) {
  std::cerr << msg << "\n";
  throw;
}
