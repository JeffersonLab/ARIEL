#ifndef fhiclcpp_types_detail_type_traits_error_msgs_h
#define fhiclcpp_types_detail_type_traits_error_msgs_h

#define NO_STD_CONTAINERS                                                      \
  "\n\n"                                                                       \
  "fhiclcpp error: Cannot create a fhicl-cpp parameter with any of the "       \
  "following types\n\n"                                                        \
  "                .. std::array\n"                                            \
  "                .. std::pair\n"                                             \
  "                .. std::vector\n"                                           \
  "                .. std::tuple\n\n"                                          \
  "                Please use one of the 'Sequence' options:\n\n"              \
  "                .. Sequence<int,4u>           ===> std::array <int,4u>\n"   \
  "                .. Sequence<int>              ===> std::vector<int>\n"      \
  "                .. Tuple<int,double>          ===> std::tuple "             \
  "<int,double>\n"                                                             \
  "                .. Tuple<int,double,bool>     ===> std::tuple "             \
  "<int,double,bool>\n"                                                        \
  "                .. Sequence<Sequence<int>,4u> ===> std::array "             \
  "<std::vector<int>,4u>\n"                                                    \
  "                .. etc.\n"

#define OPTIONAL_FHICL_TYPES                                                   \
  "                .. OptionalAtom<T>\n"                                       \
  "                .. OptionalSequence<T>\n"                                   \
  "                .. OptionalSequence<T,SZ>\n"                                \
  "                .. OptionalTable<T>\n"                                      \
  "                .. OptionalTuple<T...>\n"                                   \
  "                .. OptionalTupleAs<T(U...)>\n"

#define FHICL_TYPES                                                            \
  "                .. Atom<T>\n"                                               \
  "                .. Sequence<T>\n"                                           \
  "                .. Sequence<T,SZ>\n" OPTIONAL_FHICL_TYPES                   \
  "                .. Table<T>\n"                                              \
  "                .. TableFragment<T>\n"                                      \
  "                .. Tuple<T...>\n"

#define NO_NESTED_FHICL_TYPES_IN_ATOM                                          \
  "\n\n"                                                                       \
  "fhiclcpp error: Cannot create a nested 'Atom'--i.e. cannot create an\n"     \
  "                'Atom' with the following types:\n\n" FHICL_TYPES

#define NO_NESTED_FHICL_TYPES_IN_TABLE                                         \
  "\n\n"                                                                       \
  "fhiclcpp error: Cannot create a nested 'Table'--i.e. cannot create a\n"     \
  "                'Table' with the following types:\n\n" FHICL_TYPES

#define NO_NESTED_TABLE_FRAGMENTS                                              \
  "\n\n"                                                                       \
  "fhiclcpp error: A 'TableFragment' cannot be a template argument (\"T\" "    \
  "below)\n"                                                                   \
  "                to any of the following types:\n\n" FHICL_TYPES

#define NO_DEFAULTS_FOR_TABLE                                                  \
  "\n\n"                                                                       \
  "fhiclcpp error: Cannot specify a default for type 'Table'\n"                \
  "                Please remove 'Table<T>{}' from default list.\n\n"

#define REQUIRE_CLASS_TABLE_FRAGMENT                                           \
  "\n\n"                                                                       \
  "fhiclcpp error: The template argument for a TableFragment must be\n"        \
  "                a 'class' or a 'struct'.\n\n"

#define NO_OPTIONAL_TYPES                                                      \
  "\n\n"                                                                       \
  "fhiclcpp error: The following optional parameters cannot be\n"              \
  "                template arguments to any fhiclcpp "                        \
  "types:\n\n" OPTIONAL_FHICL_TYPES

#define NO_DELEGATED_PARAMETERS                                                \
  "\n\n"                                                                       \
  "fhiclcpp error: The 'DelegatedParameter' and 'OptionalDelegatedParameter'"  \
  "                types cannot be template arguments to any fhiclcpp "        \
  "types.\n\n"

#endif /* fhiclcpp_types_detail_type_traits_error_msgs_h */

// Local Variables:
// mode: c++
// End:
