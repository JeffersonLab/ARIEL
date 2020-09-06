# ======================================================================
#
# Testing for fhiclcpp-types: Atom<>
#                             Sequence<>
#                             Table<>
#                             TableFragment<>
#                             Tuple<>
#
# The below tests examine the following scenarios where in what
# follows, ’T’ represents a type supported by an Atom<> and ’S’
# represents an explicitly constructed struct that may contain
# Atom<>, Sequence<>, Table(Fragment)<>, or Tuple<> objects.
#
#    [ 1] Atom<T>;
#    [ 2] Sequence<T>
#    [ 3] Sequence<T,SZ>
#    [ 4] Tuple<T...>
#    [ 5] Tuple< Sequence<T>, U...>
#    [ 6] Tuple< Sequence<T,SZ>, U...>
#    [ 7] Tuple< Tuple<T...>,U...>
#    [ 8] Sequence< Tuple<T...> >
#    [ 9] Sequence< Tuple<T...>, SZ >
#    [10] Sequence< Sequence<T> >
#    [11] Sequence< Sequence<T,SZ> >
#    [12] Sequence< Sequence<T>, SZ >
#    [13] Sequence< Sequence<T,SZ>, SZ >
#
#    14-21 cannot support default arguments (since Table<> cannot have a default)
#
#    [14] Table<S>
#    [15] Sequence< Table<S> >
#    [16] Sequence< Table<S>, SZ >
#    [17] Tuple< Table<S>, U... >
#    [18] Tuple< Sequence< Table<S> >, U... >
#    [19] Tuple< Sequence< Table<S>, SZ>, U... >
#    [20] Sequence< Tuple< Table<S>, U... > >
#    [21] Sequence< Tuple< Table<S>, U... >, SZ>
#
#    The TableFragment<> does not support a default
#
#    [22] TableFragment<S>
#
# ======================================================================

cet_test(keymap_t                USE_BOOST_UNIT)
cet_test(keymap_defaults_t       USE_BOOST_UNIT)
cet_test(keymap_table_t          USE_BOOST_UNIT)
cet_test(keys_to_ignore_t        USE_BOOST_UNIT)
cet_test(return_types_t          USE_BOOST_UNIT)
cet_test(return_types_table_t    USE_BOOST_UNIT)
cet_test(return_value_defaults_t USE_BOOST_UNIT)
cet_test(searchAllowedConfiguration_t USE_BOOST_UNIT)

cet_test(tableAs_t USE_BOOST_UNIT
  DATAFILES
  tableAs_t.fcl)

cet_test(tupleAs_t USE_BOOST_UNIT
  DATAFILES
  tupleAs_t.fcl)

cet_test(conditional_t USE_BOOST_UNIT
  DATAFILES
  cond_01_t.fcl
  cond_02_t.fcl
  cond_03_t.fcl
  REF "${CMAKE_CURRENT_SOURCE_DIR}/conditional_t-ref.txt")

cet_test(print_tupleAs_t
  REF "${CMAKE_CURRENT_SOURCE_DIR}/print_tupleAs_t-ref.txt")

cet_test(print_allowed_configuration_t
  REF "${CMAKE_CURRENT_SOURCE_DIR}/print_allowed_configuration_t-ref.txt")

cet_test(print_allowed_configuration_optionalParameters_t
  REF "${CMAKE_CURRENT_SOURCE_DIR}/print_allowed_configuration_optionalParameters_t-ref.txt")

cet_test(print_allowed_configuration_tableFragment_t
  REF "${CMAKE_CURRENT_SOURCE_DIR}/print_allowed_configuration_tableFragment_t-ref.txt")

cet_test(print_allowed_configuration_delegatedParameter_t
  REF "${CMAKE_CURRENT_SOURCE_DIR}/print_allowed_configuration_delegatedParameter_t-ref.txt")

cet_test(print_allowed_configuration_conditional_t
  REF "${CMAKE_CURRENT_SOURCE_DIR}/print_allowed_configuration_conditional_t-ref.txt")

cet_test(print_allowed_configuration_defaults_t
  REF "${CMAKE_CURRENT_SOURCE_DIR}/print_allowed_configuration_defaults_t-ref.txt")

cet_test(validate_ParameterSet_t
  DATAFILES
  validate_ParameterSet_t.fcl
  REF "${CMAKE_CURRENT_SOURCE_DIR}/validate_ParameterSet_t-ref.txt")

cet_test(delegatedParameter_t USE_BOOST_UNIT
  DATAFILES
  delegatedParameter_t.fcl)

cet_test(return_values_from_fcl_t USE_BOOST_UNIT
  DATAFILES
  return_values_from_fcl_t.fcl)

cet_test(return_optionalValues_from_fcl_t USE_BOOST_UNIT
  DATAFILES
  return_optionalValues_from_fcl_t.fcl)

cet_test(return_table_values_from_fcl_t USE_BOOST_UNIT
  DATAFILES
  return_table_values_from_fcl_t.fcl)

cet_test(table_insertionOperator_t USE_BOOST_UNIT)
cet_test(bounded_sequences_with_defaults_t USE_BOOST_UNIT)

cet_test(nestedValidation_01_t USE_BOOST_UNIT)
cet_test(nestedValidation_02_t USE_BOOST_UNIT)
cet_test(nestedValidation_03_t USE_BOOST_UNIT)
