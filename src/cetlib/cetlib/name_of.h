#ifndef cetlib_name_of_h
#define cetlib_name_of_h

// ======================================================================
//
// name_of: Define name_of<>::is() for each native type; can be
//          specialized for arbitrary user-defined class/struct type
//
// TODO: specialize for function, ptr-to-mbr-obj, ptr-to-mbr_fctn
// TODO: consider specializing for more standard library types
//
// ======================================================================

#include <cstddef>
#include <iosfwd>
#include <string>
#include <type_traits>

namespace cet {

  // primary template:
  template <class T>
  struct name_of;

  // specializations for native types:
  template <>
  struct name_of<signed char>;
  template <>
  struct name_of<int>;
  template <>
  struct name_of<long>;
  template <>
  struct name_of<short>;
  template <>
  struct name_of<long long>;
  template <>
  struct name_of<unsigned char>;
  template <>
  struct name_of<unsigned int>;
  template <>
  struct name_of<unsigned long>;
  template <>
  struct name_of<unsigned short>;
  template <>
  struct name_of<unsigned long long>;
  template <>
  struct name_of<float>;
  template <>
  struct name_of<double>;
  template <>
  struct name_of<long double>;
  template <>
  struct name_of<void>;
  template <>
  struct name_of<bool>;
  template <>
  struct name_of<char>;

  // specializations for composite types:
  template <class T>
  struct name_of<T const>;
  template <class T>
  struct name_of<T volatile>;
  template <class T>
  struct name_of<T const volatile>;
  template <class T>
  struct name_of<T*>;
  template <class T>
  struct name_of<T&>;
  template <class T, int N>
  struct name_of<T[N]>;

  // specializations for selected library types:
  template <>
  struct name_of<std::istream>;
  template <>
  struct name_of<std::nullptr_t>;
  template <>
  struct name_of<std::ostream>;
  template <>
  struct name_of<std::string>;
}

// ----------------------------------------------------------------------
// primary template:

template <class T>
struct cet::name_of {
  static std::string
  is()
  {
    return "unknown-type";
  }
};

// ----------------------------------------------------------------------
// signed integral types:

template <>
struct cet::name_of<signed char> {
  static std::string
  is()
  {
    return "schar";
  }
};

template <>
struct cet::name_of<int> {
  static std::string
  is()
  {
    return "int";
  }
};

template <>
struct cet::name_of<long> {
  static std::string
  is()
  {
    return "long";
  }
};

template <>
struct cet::name_of<short> {
  static std::string
  is()
  {
    return "short";
  }
};

template <>
struct cet::name_of<long long> {
  static std::string
  is()
  {
    return "llong";
  }
};

// ----------------------------------------------------------------------
// unsigned integral types:

template <>
struct cet::name_of<unsigned char> {
  static std::string
  is()
  {
    return "uchar";
  }
};

template <>
struct cet::name_of<unsigned int> {
  static std::string
  is()
  {
    return "uint";
  }
};

template <>
struct cet::name_of<unsigned long> {
  static std::string
  is()
  {
    return "ulong";
  }
};

template <>
struct cet::name_of<unsigned short> {
  static std::string
  is()
  {
    return "ushort";
  }
};

template <>
struct cet::name_of<unsigned long long> {
  static std::string
  is()
  {
    return "ullong";
  }
};

// ----------------------------------------------------------------------
// floating types:

template <>
struct cet::name_of<float> {
  static std::string
  is()
  {
    return "float";
  }
};

template <>
struct cet::name_of<double> {
  static std::string
  is()
  {
    return "double";
  }
};

template <>
struct cet::name_of<long double> {
  static std::string
  is()
  {
    return "ldouble";
  }
};

// ----------------------------------------------------------------------
// remaining fundamental types:

template <>
struct cet::name_of<void> {
  static std::string
  is()
  {
    return "void";
  }
};

template <>
struct cet::name_of<bool> {
  static std::string
  is()
  {
    return "bool";
  }
};

template <>
struct cet::name_of<char> {
  static std::string
  is()
  {
    return "char";
  }
};

// ----------------------------------------------------------------------
// cv-qualified types:

template <class T>
struct cet::name_of<T const> {
  static std::string
  is()
  {
    return "c_" + name_of<T>::is();
  }
};

template <class T>
struct cet::name_of<T volatile> {
  static std::string
  is()
  {
    return "v_" + name_of<T>::is();
  }
};

template <class T>
struct cet::name_of<T const volatile> {
  static std::string
  is()
  {
    return "c-v_" + name_of<T>::is();
  }
};

// ----------------------------------------------------------------------
// pointer and reference types:

template <class T>
struct cet::name_of<T*> {
  static std::string
  is()
  {
    return "ptr-to_" + name_of<T>::is();
  }
};

template <class T>
struct cet::name_of<T&> {
  static std::string
  is()
  {
    return "ref-to_" + name_of<T>::is();
  }
};

// ----------------------------------------------------------------------
// array types:

template <class T, int N>
struct cet::name_of<T[N]> {
  static std::string
  is()
  {
    return "array[" + std::to_string(N) + "]-of_" + name_of<T>::is();
  }
};

// ----------------------------------------------------------------------
// selected library types::

template <>
struct cet::name_of<std::istream> {
  static std::string
  is()
  {
    return "std::istream";
  }
};

template <>
struct cet::name_of<std::nullptr_t> {
  static std::string
  is()
  {
    return "std::nullptr_t";
  }
};

template <>
struct cet::name_of<std::ostream> {
  static std::string
  is()
  {
    return "std::ostream";
  }
};

template <>
struct cet::name_of<std::string> {
  static std::string
  is()
  {
    return "std::string";
  }
};

// ======================================================================:

#endif /* cetlib_name_of_h */

// Local Variables:
// mode: c++
// End:
