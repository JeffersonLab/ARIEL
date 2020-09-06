#ifndef fhiclcpp_types_Sequence_h
#define fhiclcpp_types_Sequence_h

#include "cetlib/container_algorithms.h"
#include "fhiclcpp/type_traits.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/ConfigPredicate.h"
#include "fhiclcpp/types/detail/NameStackRegistry.h"
#include "fhiclcpp/types/detail/ParameterArgumentTypes.h"
#include "fhiclcpp/types/detail/ParameterMetadata.h"
#include "fhiclcpp/types/detail/ParameterWalker.h"
#include "fhiclcpp/types/detail/SequenceBase.h"
#include "fhiclcpp/types/detail/check_nargs_for_bounded_sequences.h"

#include <array>
#include <initializer_list>
#include <memory>
#include <string>
#include <type_traits>

namespace fhicl {

  namespace sequence_detail {

    // Auxiliary struct for accepting either
    //
    //  (1)                  {1, 3, 5}   or
    //  (2) std::array<int,3>{2, 4, 6}
    //
    // default values for Sequence<T,N>

    template <typename T>
    class ValueHolder {
    public:
      ValueHolder(std::initializer_list<T> list) : holder_{list} {}

      template <std::size_t N>
      ValueHolder(std::array<T, N> const& array)
        : holder_(array.cbegin(), array.cend())
      {}

      auto
      begin() const noexcept
      {
        return holder_.cbegin();
      }
      auto
      end() const noexcept
      {
        return holder_.cend();
      }
      auto
      cbegin() const noexcept
      {
        return holder_.cbegin();
      }
      auto
      cend() const noexcept
      {
        return holder_.cend();
      }

    private:
      std::vector<T> const holder_;
    };
  }

  class ParameterSet;

  //==================================================================
  // e.g. Sequence<int,4> ====> std::array<int,4>
  //
  template <typename T, std::size_t N = -1ull>
  class Sequence final : public detail::SequenceBase,
                         private detail::RegisterIfTableMember {
  public:
    static_assert(!tt::is_table_fragment_v<T>, NO_NESTED_TABLE_FRAGMENTS);
    static_assert(!tt::is_optional_parameter_v<T>, NO_OPTIONAL_TYPES);
    static_assert(!tt::is_delegated_parameter_v<T>, NO_DELEGATED_PARAMETERS);

    using default_type =
      sequence_detail::ValueHolder<typename tt::fhicl_type<T>::default_type>;
    using ftype = std::array<std::shared_ptr<tt::fhicl_type<T>>, N>;
    using value_type = std::array<tt::return_type<T>, N>;

    explicit Sequence(Name&& name);
    explicit Sequence(Name&& name, Comment&& comment);
    explicit Sequence(Name&& name,
                      Comment&& comment,
                      std::function<bool()> maybeUse);

    // c'tors that support defaults
    explicit Sequence(Name&& name, default_type const& defaults);
    explicit Sequence(Name&& name,
                      Comment&& comment,
                      default_type const& defaults);
    explicit Sequence(Name&& name,
                      Comment&& comment,
                      std::function<bool()> maybeUse,
                      default_type const& defaults);

    auto
    operator()() const
    {
      value_type result = {{tt::return_type<T>()}};
      cet::transform_all(
        value_, result.begin(), [](auto const& elem) { return (*elem)(); });
      return result;
    }

    auto
    operator()(std::size_t const i) const
    {
      return (*value_.at(i))();
    }

  private:
    ftype value_;

    std::size_t
    get_size() const noexcept override
    {
      return value_.size();
    }

    void
    do_prepare_elements_for_validation(std::size_t const n) override
    {
      detail::check_nargs_for_bounded_sequences(key(), get_size(), n);
    }

    void
    do_walk_elements(
      detail::ParameterWalker<tt::const_flavor::require_non_const>& pw) override
    {
      cet::for_all(value_, [&pw](auto& e) { pw.walk_over(*e); });
    }

    void
    do_walk_elements(detail::ParameterWalker<tt::const_flavor::require_const>&
                       pw) const override
    {
      cet::for_all(value_, [&pw](auto const& e) { pw.walk_over(*e); });
    }

    void
    do_set_value(fhicl::ParameterSet const&, bool /*trimParents*/) override
    {}
  };

  //==================================================================
  // e.g. Sequence<int> ====> std::vector<int>
  //
  template <typename T>
  class Sequence<T, -1ull> final : public detail::SequenceBase,
                                   private detail::RegisterIfTableMember {
  public:
    static_assert(!tt::is_table_fragment_v<T>, NO_NESTED_TABLE_FRAGMENTS);
    static_assert(!tt::is_optional_parameter_v<T>, NO_OPTIONAL_TYPES);
    static_assert(!tt::is_delegated_parameter_v<T>, NO_DELEGATED_PARAMETERS);

    using default_type = std::vector<typename tt::fhicl_type<T>::default_type>;
    using ftype = std::vector<std::shared_ptr<tt::fhicl_type<T>>>;
    using value_type = std::vector<tt::return_type<T>>;

    explicit Sequence(Name&& name);
    explicit Sequence(Name&& name, Comment&& comment);
    explicit Sequence(Name&& name,
                      Comment&& comment,
                      std::function<bool()> maybeUse);

    // c'tors supporting default values
    explicit Sequence(Name&& name, default_type const& defaults);
    explicit Sequence(Name&& name,
                      Comment&& comment,
                      default_type const& defaults);
    explicit Sequence(Name&& name,
                      Comment&& comment,
                      std::function<bool()> maybeUse,
                      default_type const& defaults);

    auto
    operator()() const
    {
      value_type result;
      cet::transform_all(value_, std::back_inserter(result), [](auto const& e) {
        return (*e)();
      });
      return result;
    }

    auto
    operator()(std::size_t const i) const
    {
      return (*value_.at(i))();
    }

  private:
    ftype value_;

    void
    do_prepare_elements_for_validation(std::size_t const n) override
    {
      // For an unbounded sequence, we need to resize it so that any
      // nested parameters of the elements can be checked.
      if (n < value_.size()) {
        value_.resize(n);
      } else if (n > value_.size()) {
        std::string key_fragment{key()};
        // When emplacing a new element, do not include in the key
        // argument the current name-stack stem--it will automatically
        // be prepended.
        auto const& nsr = NameStackRegistry::instance();
        if (!nsr.empty()) {
          std::string const& current_stem = nsr.current();
          std::size_t const pos =
            key_fragment.find(current_stem) != std::string::npos ?
              current_stem.size() + 1ul : // + 1ul to account for the '.'
              0ul;
          key_fragment.replace(0ul, pos, "");
        }

        for (auto i = value_.size(); i != n; ++i) {
          value_.push_back(std::make_shared<tt::fhicl_type<T>>(Name::sequence_element(key_fragment, i)));
        }
      }
    }

    std::size_t
    get_size() const noexcept override
    {
      return value_.size();
    }

    void
    do_walk_elements(
      detail::ParameterWalker<tt::const_flavor::require_non_const>& pw) override
    {
      cet::for_all(value_, [&pw](auto& e) { pw.walk_over(*e); });
    }

    void
    do_walk_elements(detail::ParameterWalker<tt::const_flavor::require_const>&
                       pw) const override
    {
      cet::for_all(value_, [&pw](auto const& e) { pw.walk_over(*e); });
    }

    void
    do_set_value(fhicl::ParameterSet const&, bool /*trimParents*/) override
    {}
  };
}

#include "cetlib/container_algorithms.h"
#include "cetlib_except/demangle.h"
#include "fhiclcpp/detail/printing_helpers.h"
#include "fhiclcpp/type_traits.h"
#include "fhiclcpp/types/detail/TableMemberRegistry.h"
#include "fhiclcpp/types/detail/ostream_helpers.h"
#include "fhiclcpp/types/detail/type_traits_error_msgs.h"

#include <iostream>
#include <string>

namespace fhicl {

  //==================================================================
  // e.g. Sequence<int,4> ====> std::array<int,4>
  //

  template <typename T, std::size_t N>
  Sequence<T, N>::Sequence(Name&& name) : Sequence{std::move(name), Comment("")}
  {}

  template <typename T, std::size_t N>
  Sequence<T, N>::Sequence(Name&& name, Comment&& comment)
    : SequenceBase{std::move(name),
                   std::move(comment),
                   par_style::REQUIRED,
                   par_type::SEQ_ARRAY,
                   detail::AlwaysUse()}
    , RegisterIfTableMember{this}
    , value_{{nullptr}}
  {
    for (std::size_t i{}; i != N; ++i) {
      value_.at(i) =
        std::make_shared<tt::fhicl_type<T>>(Name::sequence_element(i));
    }
    NameStackRegistry::end_of_ctor();
  }

  template <typename T, std::size_t N>
  Sequence<T, N>::Sequence(Name&& name,
                           Comment&& comment,
                           std::function<bool()> maybeUse)
    : SequenceBase{std::move(name),
                   std::move(comment),
                   par_style::REQUIRED_CONDITIONAL,
                   par_type::SEQ_ARRAY,
                   maybeUse}
    , RegisterIfTableMember{this}
    , value_{{nullptr}}
  {
    for (std::size_t i{}; i != N; ++i) {
      value_.at(i) =
        std::make_shared<tt::fhicl_type<T>>(Name::sequence_element(i));
    }
    NameStackRegistry::end_of_ctor();
  }

  // c'tors that support defaults
  template <typename T, std::size_t N>
  Sequence<T, N>::Sequence(Name&& name, default_type const& defaults)
    : Sequence{std::move(name), Comment{""}, defaults}
  {}

  template <typename T, std::size_t N>
  Sequence<T, N>::Sequence(Name&& name,
                           Comment&& comment,
                           default_type const& defaults)
    : SequenceBase{std::move(name),
                   std::move(comment),
                   par_style::DEFAULT,
                   par_type::SEQ_ARRAY,
                   detail::AlwaysUse()}
    , RegisterIfTableMember{this}
    , value_{{nullptr}}
  {
    std::size_t i{};
    for (auto const& arg : defaults) {
      value_.at(i) =
        std::make_shared<tt::fhicl_type<T>>(Name::sequence_element(i), arg);
      ++i;
    }
    NameStackRegistry::end_of_ctor();
  }

  template <typename T, std::size_t N>
  Sequence<T, N>::Sequence(Name&& name,
                           Comment&& comment,
                           std::function<bool()> maybeUse,
                           default_type const& defaults)
    : SequenceBase{std::move(name),
                   std::move(comment),
                   par_style::DEFAULT_CONDITIONAL,
                   par_type::SEQ_ARRAY,
                   maybeUse}
    , RegisterIfTableMember{this}
    , value_{{nullptr}}
  {
    std::size_t i{};
    for (auto const& arg : defaults) {
      value_.at(i) =
        std::make_shared<tt::fhicl_type<T>>(Name::sequence_element(i), arg);
      ++i;
    }
    NameStackRegistry::end_of_ctor();
  }

  //==================================================================
  // e.g. Sequence<int> ====> std::vector<int>
  //
  template <typename T>
  Sequence<T, -1ull>::Sequence(Name&& name)
    : Sequence{std::move(name), Comment{""}}
  {}

  template <typename T>
  Sequence<T, -1ull>::Sequence(Name&& name, Comment&& comment)
    : SequenceBase{std::move(name),
                   std::move(comment),
                   par_style::REQUIRED,
                   par_type::SEQ_VECTOR,
                   detail::AlwaysUse()}
    , RegisterIfTableMember{this}
    , value_{{std::make_shared<tt::fhicl_type<T>>(Name::sequence_element(0ul))}}
  {
    NameStackRegistry::end_of_ctor();
  }

  template <typename T>
  Sequence<T, -1ull>::Sequence(Name&& name,
                               Comment&& comment,
                               std::function<bool()> maybeUse)
    : SequenceBase{std::move(name),
                   std::move(comment),
                   par_style::REQUIRED_CONDITIONAL,
                   par_type::SEQ_VECTOR,
                   maybeUse}
    , RegisterIfTableMember{this}
    , value_{{std::make_shared<tt::fhicl_type<T>>(Name::sequence_element(0ul))}}
  {
    NameStackRegistry::end_of_ctor();
  }

  // c'tors that support defaults
  template <typename T>
  Sequence<T, -1ull>::Sequence(Name&& name, default_type const& defaults)
    : Sequence{std::move(name), Comment{""}, defaults}
  {}

  template <typename T>
  Sequence<T, -1ull>::Sequence(Name&& name,
                               Comment&& comment,
                               default_type const& defaults)
    : SequenceBase{std::move(name),
                   std::move(comment),
                   par_style::DEFAULT,
                   par_type::SEQ_VECTOR,
                   detail::AlwaysUse()}
    , RegisterIfTableMember{this}
  {
    static_assert(!tt::is_table_v<T>, NO_DEFAULTS_FOR_TABLE);
    std::size_t i{};
    for (auto const& t : defaults) {
      value_.push_back(std::make_shared<tt::fhicl_type<T>>(Name::sequence_element(i), t));
      ++i;
    }
    NameStackRegistry::end_of_ctor();
  }

  template <typename T>
  Sequence<T, -1ull>::Sequence(Name&& name,
                               Comment&& comment,
                               std::function<bool()> maybeUse,
                               default_type const& defaults)
    : SequenceBase{std::move(name),
                   std::move(comment),
                   par_style::DEFAULT_CONDITIONAL,
                   par_type::SEQ_VECTOR,
                   maybeUse}
    , RegisterIfTableMember{this}
  {
    static_assert(!tt::is_table_v<T>, NO_DEFAULTS_FOR_TABLE);
    std::size_t i{};
    for (auto const& t : defaults) {
      value_.emplace_back(std::make_shared<tt::fhicl_type<T>>(Name::sequence_element(i), t));
      ++i;
    }
    NameStackRegistry::end_of_ctor();
  }
}

#endif /* fhiclcpp_types_Sequence_h */

// Local variables:
// mode: c++
// End:
