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
      begin() const
      {
        return holder_.cbegin();
      }
      auto
      end() const
      {
        return holder_.cend();
      }
      auto
      cbegin() const
      {
        return holder_.cbegin();
      }
      auto
      cend() const
      {
        return holder_.cend();
      }

    private:
      std::vector<T> holder_;
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
    static_assert(!tt::is_table_fragment<T>::value, NO_NESTED_TABLE_FRAGMENTS);
    static_assert(!tt::is_optional_parameter<T>::value, NO_OPTIONAL_TYPES);
    static_assert(!tt::is_delegated_parameter<T>::value,
                  NO_DELEGATED_PARAMETERS);

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
    operator()(std::size_t i) const
    {
      return (*value_.at(i))();
    }

  private:
    ftype value_;

    std::size_t
    get_size() const override
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
    static_assert(!tt::is_table_fragment<T>::value, NO_NESTED_TABLE_FRAGMENTS);
    static_assert(!tt::is_optional_parameter<T>::value, NO_OPTIONAL_TYPES);
    static_assert(!tt::is_delegated_parameter<T>::value,
                  NO_DELEGATED_PARAMETERS);

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
    operator()(std::size_t i) const
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
          value_.emplace_back(
            new tt::fhicl_type<T>{Name::sequence_element(key_fragment, i)});
        }
      }
    }

    std::size_t
    get_size() const override
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

#include "fhiclcpp/types/detail/Sequence.icc"

#endif /* fhiclcpp_types_Sequence_h */

// Local variables:
// mode: c++
// End:
