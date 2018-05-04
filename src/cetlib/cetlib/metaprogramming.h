#ifndef cetlib_metaprogramming_h
#define cetlib_metaprogramming_h

// Common metaprogramming utilities.

#include <type_traits>

namespace cet {

  using no_tag = char (&)[1];  // type indicating FALSE
  using yes_tag = char (&)[2]; // type indicating TRUE

  //=====================================
  // Detect type
  //
  //   Use idiom:
  //
  //   template <typename T, typename = void>
  //   struct has_nested_type : std::false_type {};
  //
  //   template <typename T>
  //   struct has_nested_type<T, enable_if_type_exists_t<typename
  //   T::NestedType>> : std::true_type {}
  //
  template <class T, class R = void>
  struct enable_if_type_exists {
    using type = R;
  };

  template <class T>
  using enable_if_type_exists_t = typename enable_if_type_exists<T>::type;

  //=====================================
  // Detect function
  //
  //   Use idiom:
  //
  //   template <typename T, typename = void>
  //   struct has_my_function : std::false_type {};
  //
  //   template <typename T>
  //   struct has_my_function<T, enable_if_function_exists_t<void(T::*)(int),
  //   &T::my_function>> : std::true_type {}
  //
  template <typename T, typename U, typename R = void>
  using enable_if_same_t = std::enable_if_t<std::is_same<T, U>::value, R>;

  template <typename FT, FT f, typename R = void>
  using enable_if_function_exists_t = enable_if_same_t<FT, decltype(f), R>;
}
#endif /* cetlib_metaprogramming_h */

// Local Variables:
// mode: c++
// End:
