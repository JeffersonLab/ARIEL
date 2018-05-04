#ifndef fhiclcpp_parse_shims_h
#define fhiclcpp_parse_shims_h

#include "fhiclcpp/parse_shims_opts.h"

#include "boost/spirit/include/qi.hpp"

namespace shims {

  inline auto
  lit(char arg)
  {
    return ::boost::spirit::qi::lit(arg);
  }

  inline auto
  lit(std::string arg)
  {
    if (arg[0] == '@' && shims::isSnippetMode())
      arg[0] = '!';

    return ::boost::spirit::qi::lit(arg);
  }

  BOOST_SPIRIT_TERMINAL(catchall)
}

namespace boost {
  namespace spirit {
    template <>
    struct use_terminal<::boost::spirit::qi::domain, shims::tag::catchall>
      : mpl::true_ {
    };
  }
}

namespace shims {

  inline bool
  maximally_munched_ass(char ch)
  {
    return !std::isgraph(ch) || ch == '#' || ch == '/' || ch == ',' ||
           ch == ']' || ch == '}' || ch == '.' || ch == '[' || ch == ':';
  }

  struct catchall_parser
    : ::boost::spirit::qi::primitive_parser<catchall_parser> {
    template <typename Context, typename Iterator>
    struct attribute {
      typedef std::string type;
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

      if (!::shims::isSnippetMode())
        return false;

      Iterator it = first;
      while (it != last &&
             (std::isalnum(*it) || *it == '_' || *it == ':' || *it == '@'))
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

    template <typename Context>
    boost::spirit::info
    what(Context&) const
    {
      return boost::spirit::info("shims::catchall");
    }
  };
}

namespace boost {
  namespace spirit {
    namespace qi {
      template <typename Modifiers>
      struct make_primitive<shims::tag::catchall, Modifiers> {
        typedef shims::catchall_parser result_type;
        result_type operator()(unused_type, unused_type) const
        {
          return result_type();
        }
      };
    }
  }
}
#endif /* fhiclcpp_parse_shims_h */
