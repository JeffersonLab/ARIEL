#ifndef fhiclcpp_types_TableAs_h
#define fhiclcpp_types_TableAs_h

// ====================================================================
// The 'TableAs' class template allows the conversion of a
// fhicl::Table<Config> type to a user-defined type.  For a given type:
//
//   fhicl::TableAs<MyType, a::MyTypeConfig>
//
// where the 'a::MyTypeConfig' type is a template argument to
// 'fhicl::Table<a::MyTypeConfig>', a conversion to 'MyType'
// automatically occurs if the user provides a conversion function in
// the same namespace as 'MyTypeConfig'.  Specifically:
//
//   namespace a {
//     struct MyTypeConfig {
//       fhicl::Atom<int> myParam{fhicl::Name("myParam")};
//       ...
//     };
//
//     MyType convert(MyTypeConfig const& config)
//     {
//       return MyType{config.myParam(), ...};
//     }
//   }
//
// Assuming these criteria are met, a 'MyType' object will be returned
// upon calling the operator() function of the fhicl::TableAs object.
// the 'MyType' object will be returned.
// ====================================================================

#include "fhiclcpp/type_traits.h"
#include "fhiclcpp/types/ConfigPredicate.h"
#include "fhiclcpp/types/OptionalTable.h"
#include "fhiclcpp/types/detail/ParameterBase.h"
#include "fhiclcpp/types/detail/type_traits_error_msgs.h"

#include <memory>
#include <string>
#include <utility>

namespace fhicl {

  //==================================================================
  // e.g. TableAs<T, Config> ====> T as created by convert(Config)
  //
  template <typename T, typename Config>
  class TableAs {
  public:
    using default_type = T;
    using via_type = typename OptionalTable<Config>::value_type;
    using value_type = T;

    explicit TableAs(Name&& name);
    explicit TableAs(Name&& name, Comment&& comment);
    explicit TableAs(Name&& name,
                     Comment&& comment,
                     std::function<bool()> maybeUse);

    // c'tors supporting default values
    explicit TableAs(Name&& name, T const& t);
    explicit TableAs(Name&& name, Comment&& comment, T const& t);
    explicit TableAs(Name&& name,
                     Comment&& comment,
                     std::function<bool()> maybeUse,
                     T const& t);

    T
    operator()() const
    {
      via_type via;
      return tableObj_(via) ? convert(via) :
                              tableObj_.has_default() ?
                              *t_ :
                              throw fhicl::exception(
                                cant_find); // fix this exception category!
    }

    //=================================================================
    // expert only

    operator detail::ParameterBase&()
    {
      return tableObj_;
    } // Allows implicit conversion from
      // TableAs to ParameterBase (necessary
      // for ParameterWalker)
  private:
    OptionalTable<Config> tableObj_;
    std::shared_ptr<T>
      t_{}; // shared instead of unique to allowed Sequence<TableAs<>> objects.

    Comment conversion_comment(Comment&& comment) const;
    Comment conversion_comment(Comment&& comment, T const& t) const;
  };

  //==================================================================
  // IMPLEMENTATION

  template <typename T, typename Config>
  TableAs<T, Config>::TableAs(Name&& name)
    : TableAs{std::move(name), Comment("")}
  {}

  template <typename T, typename Config>
  TableAs<T, Config>::TableAs(Name&& name, Comment&& comment)
    : tableObj_{std::move(name), conversion_comment(std::move(comment))}
  {
    tableObj_.set_par_style(par_style::REQUIRED);
  }

  template <typename T, typename Config>
  TableAs<T, Config>::TableAs(Name&& name,
                              Comment&& comment,
                              std::function<bool()> maybeUse)
    : tableObj_{std::move(name),
                conversion_comment(std::move(comment)),
                maybeUse}
  {
    tableObj_.set_par_style(par_style::REQUIRED_CONDITIONAL);
  }

  // c'tors supporting default values
  template <typename T, typename Config>
  TableAs<T, Config>::TableAs(Name&& name, T const& t)
    : TableAs{std::move(name), Comment(""), t}
  {}

  template <typename T, typename Config>
  TableAs<T, Config>::TableAs(Name&& name, Comment&& comment, T const& t)
    : tableObj_{std::move(name), conversion_comment(std::move(comment), t)}
    , t_{std::make_shared<T>(t)}
  {
    tableObj_.set_par_style(par_style::DEFAULT);
  }

  template <typename T, typename Config>
  TableAs<T, Config>::TableAs(Name&& name,
                              Comment&& comment,
                              std::function<bool()> maybeUse,
                              T const& t)
    : tableObj_{std::move(name),
                conversion_comment(std::move(comment), t),
                maybeUse}
    , t_{std::make_shared<T>(t)}
  {
    tableObj_.set_par_style(par_style::DEFAULT_CONDITIONAL);
  }

  template <typename T, typename Config>
  Comment
  TableAs<T, Config>::conversion_comment(Comment&& comment) const
  {
    std::string const preface =
      "N.B. The following table is converted to type:";
    std::string const name =
      "        '" + cet::demangle_symbol(typeid(T).name()) + "'";
    std::string const user_comment =
      comment.value.empty() ? "" : "\n\n" + comment.value;

    std::ostringstream oss;
    oss << preface << '\n' << name << user_comment;
    return Comment{oss.str().c_str()};
  }

  //=================================================================
  // metaprogramming necessary for determining if provided type 'T'
  // has an 'std::ostream& operator<<(std::ostream&, T const&)' defined

  namespace has_insertion_operator_impl {
    typedef char no;
    typedef char yes[2];

    struct any_t {
      template <typename T>
      any_t(T const&);
    };

    no operator<<(std::ostream const&, any_t const&);

    yes& test(std::ostream&);
    no test(no);

    template <typename T>
    struct has_insertion_operator {
      static std::ostream& s;
      static T const& t;
      static bool const value = sizeof(test(s << t)) == sizeof(yes);
    };
  }

  template <typename T>
  struct has_insertion_operator
    : has_insertion_operator_impl::has_insertion_operator<T> {};

  struct NoInsert {
    template <typename T>
    std::string
    operator()(T const&)
    {
      return "     A default value is present, but it cannot be\n"
             "     printed out since no 'operator<<' overload has\n"
             "     been provided for the above type.";
    }
  };

  struct YesInsert {
    template <typename T>
    std::string
    operator()(T const& t)
    {
      std::ostringstream os;
      os << "     with a default value of:\n"
         << "        " << t;
      return os.str();
    }
  };

  //===============================================================================

  template <typename T, typename Config>
  Comment
  TableAs<T, Config>::conversion_comment(Comment&& comment, T const& t) const
  {
    std::string const preface{"N.B. The following table is converted to type:"};
    std::string const name{"        '" +
                           cet::demangle_symbol(typeid(T).name()) + "'"};

    std::conditional_t<has_insertion_operator<T>::value, YesInsert, NoInsert>
      stringified_default;

    std::string const user_comment{
      comment.value.empty() ? "" : "\n\n" + comment.value};

    std::ostringstream oss;
    oss << preface << '\n'
        << name << '\n'
        << stringified_default(t) << user_comment;
    return Comment{oss.str().c_str()};
  }
}

#endif /* fhiclcpp_types_TableAs_h */

// Local variables:
// mode: c++
// End:
