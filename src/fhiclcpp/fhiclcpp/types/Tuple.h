#ifndef fhiclcpp_types_Tuple_h
#define fhiclcpp_types_Tuple_h

#include "fhiclcpp/detail/printing_helpers.h"
#include "fhiclcpp/type_traits.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/ConfigPredicate.h"
#include "fhiclcpp/types/detail/NameStackRegistry.h"
#include "fhiclcpp/types/detail/SequenceBase.h"
#include "fhiclcpp/types/detail/check_nargs_for_bounded_sequences.h"
#include "fhiclcpp/types/detail/type_traits_error_msgs.h"

#include <memory>
#include <string>
#include <utility>

namespace fhicl {

  namespace tuple_detail {

    // Auxiliary struct for accepting either
    //
    //  (1) {1, false, "Henry"}
    //  (2) std::tuple<int,bool,string>{2, true, "Hannah"}
    //
    // default values for Sequence<T,N>

    template <typename... Defaults>
    class ValueHolder {
    public:
      ValueHolder(Defaults... defaults)
        : holder_{std::forward_as_tuple(defaults...)}
      {}

      ValueHolder(std::tuple<Defaults...> const& tup) : holder_{tup} {}

      template <std::size_t I>
      auto const&
      get() const
      {
        return std::get<I>(holder_);
      }

    private:
      std::tuple<Defaults...> const holder_;
    };
  }

  class ParameterSet;

  //==================================================================
  // e.g. Tuple<int,double,bool> ====> std::tuple<int,double,bool>
  //

  template <typename... T>
  class Tuple final : public detail::SequenceBase,
                      private detail::RegisterIfTableMember {
  public:
    using default_type =
      tuple_detail::ValueHolder<typename tt::fhicl_type<T>::default_type...>;
    using value_type = std::tuple<tt::return_type<T>...>;
    using ftype = std::tuple<std::shared_ptr<tt::fhicl_type<T>>...>;

    static_assert(
      !std::disjunction_v<tt::is_table_fragment<tt::return_type<T>>...>,
      NO_NESTED_TABLE_FRAGMENTS);
    static_assert(
      !std::disjunction_v<tt::is_optional_parameter<tt::return_type<T>>...>,
      NO_OPTIONAL_TYPES);
    static_assert(
      !std::disjunction_v<tt::is_delegated_parameter<tt::return_type<T>>...>,
      NO_DELEGATED_PARAMETERS);

    explicit Tuple(Name&& name);
    explicit Tuple(Name&& name, Comment&& comment);
    explicit Tuple(Name&& name,
                   Comment&& comment,
                   std::function<bool()> maybeUse);

    // c'tors supporting defaults;
    explicit Tuple(Name&& name, default_type const& defaults);
    explicit Tuple(Name&& name,
                   Comment&& comment,
                   default_type const& defaults);
    explicit Tuple(Name&& name,
                   Comment&& comment,
                   std::function<bool()> maybeUse,
                   default_type const& defaults);

    auto operator()() const;

    template <std::size_t I>
    auto
    get() const
    {
      return (*std::get<I>(value_))();
    }

  private:
    ftype value_;

    std::size_t
    get_size() const noexcept override
    {
      return std::tuple_size<ftype>();
    }

    void
    do_set_value(ParameterSet const&, bool /*trimParents*/) override
    {}

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

    //===============================================================
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
    // filling tuple elements from default
    template <size_t I>
    void
    fill_tuple_element(default_type const& defaults)
    {
      using elem_ftype = typename std::tuple_element_t<I, ftype>::element_type;
      std::get<I>(value_) = std::make_shared<elem_ftype>(
        Name::sequence_element(I), defaults.template get<I>());
    }

    template <std::size_t... I>
    void
    fill_tuple_elements(default_type const& default_values,
                        std::index_sequence<I...>)
    {
      (fill_tuple_element<I>(default_values), ...);
    }

    //===================================================================
    // filling return type
    template <std::size_t... I>
    value_type
    get_rtype_result(std::index_sequence<I...>) const
    {
      return value_type{(*std::get<I>(value_))()...};
    }

  }; // class Tuple

  //================= IMPLEMENTATION =========================
  //
  template <typename... T>
  Tuple<T...>::Tuple(Name&& name) : Tuple{std::move(name), Comment("")}
  {}

  template <typename... T>
  Tuple<T...>::Tuple(Name&& name, Comment&& comment)
    : SequenceBase{std::move(name),
                   std::move(comment),
                   par_style::REQUIRED,
                   par_type::TUPLE,
                   detail::AlwaysUse()}
    , RegisterIfTableMember{this}
  {
    finalize_elements(std::index_sequence_for<T...>{});
    NameStackRegistry::end_of_ctor();
  }

  template <typename... T>
  Tuple<T...>::Tuple(Name&& name,
                     Comment&& comment,
                     std::function<bool()> maybeUse)
    : SequenceBase{std::move(name),
                   std::move(comment),
                   par_style::REQUIRED_CONDITIONAL,
                   par_type::TUPLE,
                   maybeUse}
    , RegisterIfTableMember{this}
  {
    finalize_elements(std::index_sequence_for<T...>{});
    NameStackRegistry::end_of_ctor();
  }

  // c'tors supporting defaults

  template <typename... T>
  Tuple<T...>::Tuple(Name&& name, default_type const& defaults)
    : Tuple{std::move(name), Comment(""), defaults}
  {}

  template <typename... T>
  Tuple<T...>::Tuple(Name&& name,
                     Comment&& comment,
                     default_type const& defaults)
    : SequenceBase{std::move(name),
                   std::move(comment),
                   par_style::DEFAULT,
                   par_type::TUPLE,
                   detail::AlwaysUse()}
    , RegisterIfTableMember{this}
  {
    fill_tuple_elements(defaults, std::index_sequence_for<T...>());
    NameStackRegistry::end_of_ctor();
  }

  template <typename... T>
  Tuple<T...>::Tuple(Name&& name,
                     Comment&& comment,
                     std::function<bool()> maybeUse,
                     default_type const& defaults)
    : SequenceBase{std::move(name),
                   std::move(comment),
                   par_style::DEFAULT_CONDITIONAL,
                   par_type::TUPLE,
                   maybeUse}
    , RegisterIfTableMember{this}
  {
    fill_tuple_elements(defaults, std::index_sequence_for<T...>());
    NameStackRegistry::end_of_ctor();
  }

  template <typename... T>
  auto
  Tuple<T...>::operator()() const
  {
    return get_rtype_result(std::index_sequence_for<T...>());
  }
}

#endif /* fhiclcpp_types_Tuple_h */

// Local variables:
// mode: c++
// End:
