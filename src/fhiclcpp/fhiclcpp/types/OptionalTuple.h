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

  template <typename... T>
  class OptionalTuple final : public detail::SequenceBase,
                              private detail::RegisterIfTableMember {
  public:
    using ftype = std::tuple<std::shared_ptr<tt::fhicl_type<T>>...>;
    using value_type = std::tuple<tt::return_type<T>...>;

    static_assert(
      !std::disjunction_v<tt::is_table_fragment<tt::return_type<T>>...>,
      NO_NESTED_TABLE_FRAGMENTS);
    static_assert(
      !std::disjunction_v<tt::is_optional_parameter<tt::return_type<T>>...>,
      NO_OPTIONAL_TYPES);
    static_assert(
      !std::disjunction_v<tt::is_delegated_parameter<tt::return_type<T>>...>,
      NO_DELEGATED_PARAMETERS);

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
    get_size() const noexcept override
    {
      return std::tuple_size<ftype>();
    }

    //===================================================================
    // iterate over tuple elements
    using PW_non_const =
      detail::ParameterWalker<tt::const_flavor::require_non_const>;
    using PW_const = detail::ParameterWalker<tt::const_flavor::require_const>;

    template <std::size_t... I>
    void
    iterate_over_tuple(PW_non_const& pw, std::index_sequence<I...>)
    {
      (pw.walk_over(*std::get<I>(value_)), ...);
    }

    void
    do_prepare_elements_for_validation(std::size_t const n) override
    {
      detail::check_nargs_for_bounded_sequences(key(), get_size(), n);
    }

    void
    do_walk_elements(PW_non_const& pw) override
    {
      iterate_over_tuple(pw, std::index_sequence_for<T...>{});
    }

    template <std::size_t... I>
    void
    iterate_over_tuple(PW_const& pw, std::index_sequence<I...>) const
    {
      (pw.walk_over(*std::get<I>(value_)), ...);
    }

    void
    do_walk_elements(PW_const& pw) const override
    {
      iterate_over_tuple(pw, std::index_sequence_for<T...>{});
    }

    //===================================================================
    // finalizing tuple elements
    template <std::size_t I>
    void
    finalize_element()
    {
      using elem_ftype = typename std::tuple_element_t<I, ftype>::element_type;
      std::get<I>(value_) =
        std::make_shared<elem_ftype>(Name::sequence_element(I));
    }

    template <std::size_t... I>
    void
    finalize_elements(std::index_sequence<I...>)
    {
      (finalize_element<I>(), ...);
    }

    //===================================================================
    // filling return type
    template <std::size_t... I>
    value_type
    get_rtype_result(std::index_sequence<I...>) const
    {
      return value_type{(*std::get<I>(value_))()...};
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
  template <typename... T>
  OptionalTuple<T...>::OptionalTuple(Name&& name)
    : OptionalTuple{std::move(name), Comment("")}
  {}

  template <typename... T>
  OptionalTuple<T...>::OptionalTuple(Name&& name, Comment&& comment)
    : SequenceBase{std::move(name),
                   std::move(comment),
                   par_style::OPTIONAL,
                   par_type::TUPLE,
                   detail::AlwaysUse()}
    , RegisterIfTableMember{this}
  {
    finalize_elements(std::index_sequence_for<T...>{});
    NameStackRegistry::end_of_ctor();
  }

  template <typename... T>
  OptionalTuple<T...>::OptionalTuple(Name&& name,
                                     Comment&& comment,
                                     std::function<bool()> maybeUse)
    : SequenceBase{std::move(name),
                   std::move(comment),
                   par_style::OPTIONAL_CONDITIONAL,
                   par_type::TUPLE,
                   maybeUse}
    , RegisterIfTableMember{this}
  {
    finalize_elements(std::index_sequence_for<T...>{});
    NameStackRegistry::end_of_ctor();
  }

  template <typename... T>
  bool
  OptionalTuple<T...>::operator()(value_type& r) const
  {
    if (!has_value_)
      return false;
    auto result = get_rtype_result(std::index_sequence_for<T...>());
    std::swap(result, r);
    return true;
  }
}

#endif /* fhiclcpp_types_OptionalTuple_h */

// Local variables:
// mode: c++
// End:
