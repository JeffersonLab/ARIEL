#ifndef fhiclcpp_tokens_h
#define fhiclcpp_tokens_h

// ======================================================================
//
// tokens
//
// ======================================================================

#include "boost/spirit/home/support/terminal.hpp"
#include "boost/spirit/include/qi.hpp"
#include "cetlib/canonical_number.h"
#include "fhiclcpp/ParameterSetID.h"
#include "fhiclcpp/detail/binding_modifier.h"

#include <cctype>
#include <cstddef>
#include <string>

namespace qi = boost::spirit::qi;

// ----------------------------------------------------------------------
// utilities:

namespace fhicl {

  inline bool
  maximally_munched(char const ch)
  {
    return !std::isgraph(ch) || ch == '#' || ch == '/' || ch == ',' ||
           ch == ']' || ch == '}';
  }

  inline bool
  maximally_munched_number(char const ch)
  {
    return maximally_munched(ch) || ch == ')';
  }

  inline bool
  maximally_munched_ass(char const ch)
  {
    return maximally_munched(ch) || ch == '.' || ch == '[' || ch == ':';
  }

  inline bool
  maximally_munched_dss(char const ch)
  {
    return maximally_munched(ch);
  }

} // fhicl

// ----------------------------------------------------------------------
// identify custom terminal symbols:

namespace fhicl {
  BOOST_SPIRIT_TERMINAL(real)
  BOOST_SPIRIT_TERMINAL(uint)
  BOOST_SPIRIT_TERMINAL(hex)
  BOOST_SPIRIT_TERMINAL(bin)
  BOOST_SPIRIT_TERMINAL(ass)
  BOOST_SPIRIT_TERMINAL(dss)
  BOOST_SPIRIT_TERMINAL(dbid)
  BOOST_SPIRIT_TERMINAL(binding)
}

// ----------------------------------------------------------------------
// identify our tokens as terminals and in qi::domain (only for parsing):

namespace boost {
  namespace spirit {

    template <>
    struct use_terminal<qi::domain, fhicl::tag::real> : mpl::true_ {};

    template <>
    struct use_terminal<qi::domain, fhicl::tag::uint> : mpl::true_ {};

    template <>
    struct use_terminal<qi::domain, fhicl::tag::hex> : mpl::true_ {};

    template <>
    struct use_terminal<qi::domain, fhicl::tag::dbid> : mpl::true_ {};

    template <>
    struct use_terminal<qi::domain, fhicl::tag::bin> : mpl::true_ {};

    template <>
    struct use_terminal<qi::domain, fhicl::tag::ass> : mpl::true_ {};

    template <>
    struct use_terminal<qi::domain, fhicl::tag::dss> : mpl::true_ {};

    template <>
    struct use_terminal<qi::domain, fhicl::tag::binding> : mpl::true_ {};
  }
} // boost::spirit

// ----------------------------------------------------------------------
// implement each token parser:

namespace fhicl {

  struct real_parser : qi::primitive_parser<real_parser> {
    // designate type resulting from successful parse:
    template <typename Context, typename Iterator>
    struct attribute {
      using type = std::string;
    };

    // do the parse:
    template <typename Iterator,
              typename Context,
              typename Skipper,
              typename Attribute>
    bool
    parse(Iterator& first,
          Iterator const& last,
          Context&,
          Skipper const& skipper,
          Attribute& attr) const
    {
      boost::spirit::qi::skip_over(first, last, skipper);

      static std::string const allowed{"0123456789.-+eE"};
      Iterator it = first;
      while (it != last && allowed.find(*it) != std::string::npos)
        ++it;

      if (it != last && !maximally_munched_number(*it))
        return false;

      Attribute raw(first, it);
      if (raw.empty())
        return false;

      Attribute result;
      if (!cet::canonical_number(raw, result))
        return false;

      first = it;
      boost::spirit::traits::assign_to(result, attr);
      return true;
    }

    // identify this token (in case of error):
    template <typename Context>
    boost::spirit::info
    what(Context& /*unused */) const
    {
      return boost::spirit::info{"fhicl::real"};
    }

  }; // real_parser

  struct uint_parser : boost::spirit::qi::primitive_parser<uint_parser> {
    // designate type resulting from successful parse:
    template <typename Context, typename Iterator>
    struct attribute {
      using type = std::string;
    };

    // do the parse:
    template <typename Iterator,
              typename Context,
              typename Skipper,
              typename Attribute>
    bool
    parse(Iterator& first,
          Iterator const& last,
          Context&,
          Skipper const& skipper,
          Attribute& attr) const
    {
      boost::spirit::qi::skip_over(first, last, skipper);

      Iterator it = first;
      while (it != last && std::isdigit(*it))
        ++it;
      Attribute result(first, it);

      if (result.empty())
        return false;

      if (it != last && !maximally_munched_number(*it))
        return false;

      for (std::size_t ndig = result.size(); ndig > 1 && result[0] == '0';
           --ndig)
        result.erase(0, 1);

      first = it;
      boost::spirit::traits::assign_to(result, attr);
      return true;
    }

    // identify this token (in case of error):
    template <typename Context>
    boost::spirit::info
    what(Context& /*unused */) const
    {
      return boost::spirit::info{"fhicl::uint"};
    }

  }; // uint_parser

  struct hex_parser : qi::primitive_parser<hex_parser> {
    // designate type resulting from successful parse:
    template <typename Context, typename Iterator>
    struct attribute {
      using type = std::string;
    };

    // do the parse:
    template <typename Iterator,
              typename Context,
              typename Skipper,
              typename Attribute>
    bool
    parse(Iterator& first,
          Iterator const& last,
          Context&,
          Skipper const& skipper,
          Attribute& attr) const
    {
      boost::spirit::qi::skip_over(first, last, skipper);

      static std::string const allowed{"0123456789abcdefABCDEF"};
      Iterator it = first;

      if (it == last || *it != '0')
        return false;

      ++it;

      if (it == last || toupper(*it) != 'X')
        return false;

      ++it;

      while (it != last && allowed.find(*it) != std::string::npos)
        ++it;

      if (it != last && !maximally_munched_number(*it))
        return false;

      Attribute raw(first, it);
      if (raw.empty() || raw.size() == 2)
        return false;

      Attribute result;
      if (!cet::canonical_number(raw, result))
        return false;

      first = it;
      boost::spirit::traits::assign_to(result, attr);
      return true;
    }

    // identify this token (in case of error):
    template <typename Context>
    boost::spirit::info
    what(Context& /*unused */) const
    {
      return boost::spirit::info{"fhicl::hex"};
    }

  }; // hex_parser

  struct dbid_parser : qi::primitive_parser<dbid_parser> {
    // designate type resulting from successful parse:
    template <typename Context, typename Iterator>
    struct attribute {
      using type = std::string;
    };

    // do the parse:
    template <typename Iterator,
              typename Context,
              typename Skipper,
              typename Attribute>
    bool
    parse(Iterator& first,
          Iterator const& last,
          Context&,
          Skipper const& skipper,
          Attribute& attr) const
    {
      boost::spirit::qi::skip_over(first, last, skipper);

      static std::string const allowed{"0123456789abcdefABCDEF"};
      Iterator it = first;

      if (it == last)
        return false;

      while (it != last && allowed.find(*it) != std::string::npos)
        ++it;

      if (it != last && !maximally_munched_number(*it))
        return false;

      Attribute result(first, it);
      if (result.size() != ParameterSetID::max_str_size())
        return false;

      first = it;
      boost::spirit::traits::assign_to(result, attr);
      return true;
    }

    // identify this token (in case of error):
    template <typename Context>
    boost::spirit::info
    what(Context& /*unused */) const
    {
      return boost::spirit::info{"fhicl::dbid"};
    }

  }; // dbid_parser

  struct bin_parser : qi::primitive_parser<bin_parser> {
    // designate type resulting from successful parse:
    template <typename Context, typename Iterator>
    struct attribute {
      using type = std::string;
    };

    // do the parse:
    template <typename Iterator,
              typename Context,
              typename Skipper,
              typename Attribute>
    bool
    parse(Iterator& first,
          Iterator const& last,
          Context&,
          Skipper const& skipper,
          Attribute& attr) const
    {
      boost::spirit::qi::skip_over(first, last, skipper);

      static std::string const allowed{"01"};
      Iterator it = first;

      if (it == last || *it != '0')
        return false;

      ++it;

      if (it == last || toupper(*it) != 'B')
        return false;

      ++it;

      while (it != last && allowed.find(*it) != std::string::npos)
        ++it;

      if (it != last && !maximally_munched_number(*it))
        return false;

      Attribute raw(first, it);
      if (raw.empty() || raw.size() == 2)
        return false;

      Attribute result;
      if (!cet::canonical_number(raw, result))
        return false;

      first = it;
      boost::spirit::traits::assign_to(result, attr);
      return true;
    }

    // identify this token (in case of error):
    template <typename Context>
    boost::spirit::info
    what(Context& /*unused */) const
    {
      return boost::spirit::info{"fhicl::bin"};
    }

  }; // bin_parser

  struct ass_parser : qi::primitive_parser<ass_parser> {
    // designate type resulting from successful parse:
    template <typename Context, typename Iterator>
    struct attribute {
      using type = std::string;
    };

    // do the parse:
    template <typename Iterator,
              typename Context,
              typename Skipper,
              typename Attribute>
    bool
    parse(Iterator& first,
          Iterator const& last,
          Context&,
          Skipper const& skipper,
          Attribute& attr) const
    {
      boost::spirit::qi::skip_over(first, last, skipper);

      Iterator it = first;
      while (it != last && (std::isalnum(*it) || *it == '_'))
        ++it;

      if (it != last && !maximally_munched_ass(*it))
        return false;

      Attribute result(first, it);
      if (result.empty() || std::isdigit(result[0]))
        return false;

      first = it;
      boost::spirit::traits::assign_to(result, attr);
      return true;
    }

    // identify this token (in case of error):
    template <typename Context>
    boost::spirit::info
    what(Context& /*unused */) const
    {
      return boost::spirit::info{"fhicl::ass"};
    }

  }; // ass_parser

  struct dss_parser : qi::primitive_parser<dss_parser> {
    // designate type resulting from successful parse:
    template <typename Context, typename Iterator>
    struct attribute {
      using type = std::string;
    };

    // do the parse:
    template <typename Iterator,
              typename Context,
              typename Skipper,
              typename Attribute>
    bool
    parse(Iterator& first,
          Iterator const& last,
          Context&,
          Skipper const& skipper,
          Attribute& attr) const
    {
      boost::spirit::qi::skip_over(first, last, skipper);

      bool all_digits = true;
      Iterator it = first;
      for (; it != last && (std::isalnum(*it) || *it == '_'); ++it)
        all_digits = all_digits && std::isdigit(*it);

      if (it != last && !maximally_munched_dss(*it))
        return false;

      Attribute result(first, it);
      if (result.empty() || all_digits || !std::isdigit(result[0]))
        return false;

      first = it;
      boost::spirit::traits::assign_to(result, attr);
      return true;
    }

    // identify this token (in case of error):
    template <typename Context>
    boost::spirit::info
    what(Context& /*unused */) const
    {
      return boost::spirit::info{"fhicl::dss"};
    }

  }; // dss_parser

  struct binding_parser : qi::primitive_parser<binding_parser> {
    // Desired type resulting from a successful parse:
    template <typename Context, typename Iterator>
    struct attribute {
      using type = fhicl::detail::binding_modifier;
    };

    // Do the parse:
    template <typename Iterator,
              typename Context,
              typename Skipper,
              typename Attribute>
    bool
    parse(Iterator& first,
          Iterator const& last,
          Context& c,
          Skipper const& skipper,
          Attribute& attr) const
    {
      using detail::binding_modifier;
      boost::spirit::qi::skip_over(first, last, skipper);

      Attribute result = binding_modifier::NONE;
      boost::spirit::qi::symbols<char, binding_modifier> modifiers;
      modifiers.add("@protect_ignore", binding_modifier::PROTECT_IGNORE);
      modifiers.add("@protect_error", binding_modifier::PROTECT_ERROR);
#if 0 /* Preparation for issue #7231. */
      modifiers.add("@initial", binding_modifier::INITIAL);
      modifiers.add("@replace", binding_modifier::REPLACE);
      modifiers.add("@replace_compat", binding_modifier::REPLACE_COMPAT);
      modifiers.add("@add_or_replace_compat", binding_modifier::ADD_OR_REPLACE_COMPAT);
#endif
      if (((*first) == ':') ||
          (modifiers.parse(first, last, c, skipper, result) &&
           (*first) == ':')) {
        ++first;
      } else {
        return false;
      }
      boost::spirit::traits::assign_to(result, attr);
      return true;
    }

    // identify this token (in case of error):
    template <typename Context>
    boost::spirit::info
    what(Context&) const
    {
      return boost::spirit::info{"fhicl::binding"};
    }
  }; // binding_parser.

} // fhicl

// ----------------------------------------------------------------------
// provide factory functions to create instances of our parser:

namespace boost {
  namespace spirit {
    namespace qi {

      template <typename Modifiers>
      struct make_primitive<fhicl::tag::real, Modifiers> {
        using result_type = fhicl::real_parser;

        result_type operator()(unused_type, unused_type) const
        {
          return result_type{};
        }

      }; // make_primitive<...real...>

      template <typename Modifiers>
      struct make_primitive<fhicl::tag::uint, Modifiers> {
        using result_type = fhicl::uint_parser;

        result_type operator()(unused_type, unused_type) const
        {
          return result_type{};
        }

      }; // make_primitive<...uint...>

      template <typename Modifiers>
      struct make_primitive<fhicl::tag::hex, Modifiers> {
        using result_type = fhicl::hex_parser;

        result_type operator()(unused_type, unused_type) const
        {
          return result_type{};
        }

      }; // make_primitive<...hex...>

      template <typename Modifiers>
      struct make_primitive<fhicl::tag::dbid, Modifiers> {
        using result_type = fhicl::dbid_parser;

        result_type operator()(unused_type, unused_type) const
        {
          return result_type{};
        }

      }; // make_primitive<...dbid...>

      template <typename Modifiers>
      struct make_primitive<fhicl::tag::bin, Modifiers> {
        using result_type = fhicl::bin_parser;

        result_type operator()(unused_type, unused_type) const
        {
          return result_type{};
        }

      }; // make_primitive<...bin...>

      template <typename Modifiers>
      struct make_primitive<fhicl::tag::ass, Modifiers> {
        using result_type = fhicl::ass_parser;

        result_type operator()(unused_type, unused_type) const
        {
          return result_type{};
        }

      }; // make_primitive<...ass...>

      template <typename Modifiers>
      struct make_primitive<fhicl::tag::dss, Modifiers> {
        using result_type = fhicl::dss_parser;

        result_type operator()(unused_type, unused_type) const
        {
          return result_type{};
        }

      }; // make_primitive<...dss...>

      template <typename Modifiers>
      struct make_primitive<fhicl::tag::binding, Modifiers> {
        using result_type = fhicl::binding_parser;

        result_type operator()(unused_type, unused_type) const
        {
          return result_type{};
        }

      }; // make_primitive<...binding...>
    }
  }
} // boost::spirit::qi

// ======================================================================

#endif /* fhiclcpp_tokens_h */

// Local Variables:
// mode: c++
// End:
