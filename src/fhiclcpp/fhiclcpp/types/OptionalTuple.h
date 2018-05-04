#ifndef fhiclcpp_types_OptionalTuple_h
#define fhiclcpp_types_OptionalTuple_h

#include "fhiclcpp/detail/printing_helpers.h"
#include "fhiclcpp/type_traits.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/ConfigPredicate.h"
#include "fhiclcpp/types/detail/NameStackRegistry.h"
#include "fhiclcpp/types/detail/ParameterWalker.h"
#include "fhiclcpp/types/detail/SequenceBase.h"
#include "fhiclcpp/types/detail/TableMemberRegistry.h"
#include "fhiclcpp/types/detail/check_nargs_for_bounded_sequences.h"
#include "fhiclcpp/types/detail/type_traits_error_msgs.h"

#include <string>
#include <utility>

namespace fhicl {

  class ParameterSet;

  //==================================================================
  // e.g. OptionalTuple<int,double,bool> ====> std::tuple<int,double,bool>
  //

  template <typename... TYPES>
  class OptionalTuple final : public detail::SequenceBase,
                              private detail::RegisterIfTableMember {
  public:
    using ftype = std::tuple<std::shared_ptr<tt::fhicl_type<TYPES>>...>;
    using value_type = std::tuple<tt::return_type<TYPES>...>;

    explicit OptionalTuple(Name&& name);
    explicit OptionalTuple(Name&& name, Comment&& comment);
    explicit OptionalTuple(Name&& name,
                           Comment&& comment,
                           std::function<bool()> maybeUse);

    bool operator()(value_type&) const;

    bool
    hasValue() const
    {
      return has_value_;
    }

  private:
    ftype value_;
    bool has_value_{false};

    std::size_t
    get_size() const override
    {
      return std::tuple_size<ftype>();
    }

    //===================================================================
    // iterate over tuple elements
    using PW_non_const =
      detail::ParameterWalker<tt::const_flavor::require_non_const>;
    using PW_const = detail::ParameterWalker<tt::const_flavor::require_const>;

    void
    visit_element(PW_non_const&)
    {}

    template <typename E, typename... T>
    void
    visit_element(PW_non_const& pw, E& elem, T&... others)
    {
      using elem_ftype = typename E::element_type;
      static_assert(!tt::is_table_fragment<elem_ftype>::value,
                    NO_NESTED_TABLE_FRAGMENTS);
      static_assert(!tt::is_optional_parameter<elem_ftype>::value,
                    NO_OPTIONAL_TYPES);
      static_assert(!tt::is_delegated_parameter<elem_ftype>::value,
                    NO_DELEGATED_PARAMETERS);
      pw.walk_over(*elem);
      visit_element(pw, others...);
    }

    template <std::size_t... I>
    void
    iterate_over_tuple(PW_non_const& pw, std::index_sequence<I...>)
    {
      visit_element(pw, std::get<I>(value_)...);
    }

    void
    do_prepare_elements_for_validation(std::size_t const n) override
    {
      detail::check_nargs_for_bounded_sequences(key(), get_size(), n);
    }

    void
    do_walk_elements(PW_non_const& pw) override
    {
      iterate_over_tuple(pw, std::index_sequence_for<TYPES...>{});
    }

    void
    visit_element(PW_const&) const
    {}

    template <typename E, typename... T>
    void
    visit_element(PW_const& pw, E const& elem, T const&... others) const
    {
      using elem_ftype = typename E::element_type;
      static_assert(!tt::is_table_fragment<elem_ftype>::value,
                    NO_NESTED_TABLE_FRAGMENTS);
      static_assert(!tt::is_optional_parameter<elem_ftype>::value,
                    NO_OPTIONAL_TYPES);
      static_assert(!tt::is_delegated_parameter<elem_ftype>::value,
                    NO_DELEGATED_PARAMETERS);
      pw.walk_over(*elem);
      visit_element(pw, others...);
    }

    template <std::size_t... I>
    void
    iterate_over_tuple(PW_const& pw, std::index_sequence<I...>) const
    {
      visit_element(pw, std::get<I>(value_)...);
    }

    void
    do_walk_elements(PW_const& pw) const override
    {
      iterate_over_tuple(pw, std::index_sequence_for<TYPES...>{});
    }

    //===================================================================
    // finalizing tuple elements
    void finalize_tuple_elements(std::size_t) {}

    // 'E' and 'T' are shared_ptr's.
    template <typename E, typename... T>
    void
    finalize_tuple_elements(std::size_t i, E& elem, T&... others)
    {
      using elem_ftype = typename E::element_type;
      static_assert(!tt::is_table_fragment<elem_ftype>::value,
                    NO_NESTED_TABLE_FRAGMENTS);
      static_assert(!tt::is_optional_parameter<elem_ftype>::value,
                    NO_OPTIONAL_TYPES);
      static_assert(!tt::is_delegated_parameter<elem_ftype>::value,
                    NO_DELEGATED_PARAMETERS);
      elem = std::make_shared<elem_ftype>(Name::sequence_element(i));
      finalize_tuple_elements(++i, others...);
    }

    template <std::size_t... I>
    void
    finalize_elements(std::index_sequence<I...>)
    {
      finalize_tuple_elements(0, std::get<I>(value_)...);
    }

    //===================================================================
    // filling return type

    using TUPLE = std::tuple<tt::fhicl_type<TYPES>...>;

    template <size_t I, typename value_type>
    std::enable_if_t<(I >= std::tuple_size<TUPLE>::value)>
    fill_return_element(value_type&) const
    {}

    template <size_t I, typename value_type>
    std::enable_if_t<(I < std::tuple_size<TUPLE>::value)>
    fill_return_element(value_type& result) const
    {
      std::get<I>(result) = (*std::get<I>(value_))();
      fill_return_element<I + 1>(result);
    }

    void
    assemble_rtype(value_type& result) const
    {
      fill_return_element<0>(result);
    }

    void
    do_set_value(fhicl::ParameterSet const&,
                 bool const /*trimParents*/) override
    {
      // We do not explicitly set the sequence values here as the
      // individual elements are set one at a time.  However, this
      // function is reached in the ValidateThenSet algorithm if the
      // optional parameter is present.  Otherwise, this override is
      // skipped.
      has_value_ = true;
    }

  }; // class OptionalTuple

  //================= IMPLEMENTATION =========================
  //
  template <typename... TYPES>
  OptionalTuple<TYPES...>::OptionalTuple(Name&& name)
    : OptionalTuple{std::move(name), Comment("")}
  {}

  template <typename... TYPES>
  OptionalTuple<TYPES...>::OptionalTuple(Name&& name, Comment&& comment)
    : SequenceBase{std::move(name),
                   std::move(comment),
                   par_style::OPTIONAL,
                   par_type::TUPLE,
                   detail::AlwaysUse()}
    , RegisterIfTableMember{this}
  {
    finalize_elements(std::index_sequence_for<TYPES...>{});
    NameStackRegistry::end_of_ctor();
  }

  template <typename... TYPES>
  OptionalTuple<TYPES...>::OptionalTuple(Name&& name,
                                         Comment&& comment,
                                         std::function<bool()> maybeUse)
    : SequenceBase{std::move(name),
                   std::move(comment),
                   par_style::OPTIONAL_CONDITIONAL,
                   par_type::TUPLE,
                   maybeUse}
    , RegisterIfTableMember{this}
  {
    finalize_elements(std::index_sequence_for<TYPES...>{});
    NameStackRegistry::end_of_ctor();
  }

  template <typename... TYPES>
  bool
  OptionalTuple<TYPES...>::operator()(value_type& r) const
  {
    if (!has_value_)
      return false;
    value_type result;
    assemble_rtype(result);
    std::swap(result, r);
    return true;
  }
}

#endif /* fhiclcpp_types_OptionalTuple_h */

// Local variables:
// mode: c++
// End:
