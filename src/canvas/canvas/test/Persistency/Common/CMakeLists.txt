cet_test(aggregate_t USE_BOOST_UNIT
  LIBRARIES canvas)

cet_test(aggregate_clhep_t USE_BOOST_UNIT
  LIBRARIES
  canvas
  ${CLHEP})

cet_test(const_assns_iter_t LIBRARIES canvas)

cet_test(for_each_group_t LIBRARIES canvas)

cet_test(for_each_group_with_left_t LIBRARIES canvas)

cet_test(ptr_hash_t LIBRARIES canvas)

cet_test(maybeCastObj_t USE_BOOST_UNIT LIBRARIES canvas)

cet_test(sampled_t LIBRARIES canvas)

cet_test(set_ptr_customization_t USE_BOOST_UNIT LIBRARIES canvas)

cet_test(wrapper_t LIBRARIES canvas)
