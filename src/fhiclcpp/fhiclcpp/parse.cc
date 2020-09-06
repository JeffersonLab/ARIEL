// ======================================================================
//
// parse
//
// ======================================================================

#include "fhiclcpp/parse.h"
#include "cetlib/compiler_macros.h"

#ifdef __ICC
#pragma warning(push, disable : 780)
#endif
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "boost/spirit/include/phoenix_bind.hpp"
#include "boost/spirit/include/phoenix_operator.hpp"
#include "boost/spirit/include/qi.hpp"
#include "boost/spirit/include/qi_no_skip.hpp"
#include "boost/spirit/include/support_istream_iterator.hpp"
#ifdef __clang__
#pragma clang diagnostic ignored "-Wpedantic"
#else
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#include "boost/spirit/repository/home/qi/primitive/iter_pos.hpp"
#pragma GCC diagnostic pop
#ifdef __ICC
#pragma warning(pop)
#endif

#include "cetlib/canonical_number.h"
#include "cetlib/canonical_string.h"
#include "cetlib/include.h"
#include "cetlib/includer.h"
#include "fhiclcpp/detail/binding_modifier.h"
#include "fhiclcpp/exception.h"
#include "fhiclcpp/extended_value.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/tokens.h"

#include "fhiclcpp/parse_shims.h"

#include <algorithm>
#include <any>
#include <string>
#include <vector>

namespace ascii = ::boost::spirit::ascii;
namespace phx = ::boost::phoenix;
namespace qi = ::boost::spirit::qi;

using ascii::char_;
using ascii::digit;
using ascii::graph;
using ascii::space;

using phx::ref;

using boost::spirit::repository::qi::iter_pos;
using qi::_val;
using qi::eol;
using qi::lexeme;
using namespace shims; // using qi::lit; /*moved to parse_shims.h*/
using qi::no_skip;
using qi::raw;
using qi::skip;

using namespace fhicl;
using fhicl::detail::binding_modifier;
using atom_t = extended_value::atom_t;
using complex_t = extended_value::complex_t;
using sequence_t = extended_value::sequence_t;
using table_t = extended_value::table_t;

using fhicl::SEQUENCE;
using fhicl::TABLE;
using fhicl::TABLEID;

// ----------------------------------------------------------------------

namespace {

  void
  check_protection(Protection const p, extended_value& v, std::string name = "")
  {
    using std::max;
    if (v.protection == Protection::NONE) {
      v.protection = p;
    } else if (v.protection < p) {
      throw fhicl::exception(fhicl::error::protection_violation)
        << "Nested item " << name << " has protection "
        << to_string(v.protection)
        << ((!v.src_info.empty()) ?
              (std::string(" on ") + v.pretty_src_info()) :
              "")
        << ", which is incompatible with an enclosing item's protection of "
        << to_string(p) << "\n";
    };
    if (v.tag == fhicl::SEQUENCE) {
      std::size_t count = 0;
      using std::to_string;
      for (auto& subv : sequence_t(v)) {
        check_protection(
          max(p, v.protection), subv, name + '[' + to_string(count) + ']');
      }
    } else if (v.tag == fhicl::TABLE) {
      for (auto& [key, value] : table_t(v)) {
        std::string sname(name);
        if (!sname.empty()) {
          sname.append(".");
        }
        sname.append(key);
        check_protection(max(p, v.protection), value, std::move(sname));
      }
    }
  }

  void
  check_protection(extended_value& v, std::string name = "")
  {
    if (v.is_a(fhicl::SEQUENCE) || v.is_a(fhicl::TABLE)) {
      check_protection(v.protection, v, std::move(name));
    }
  }

  void
  set_protection(std::string const& name,
                 binding_modifier const m,
                 extended_value& v)
  {
    if (m == binding_modifier::NONE) {
      return;
    }

    v.protection = static_cast<Protection>(m);
    check_protection(v, name);
  }

  std::string
  canon_nil(std::string const&)
  {
    static std::string const canon_nil(9, '\0');
    return canon_nil;
  }

  std::string
  canon_inf(std::string const& inf)
  {
    return inf[0] == 'i' ? ('+' + inf) : inf;
  }

  std::string
  canon_num(std::string const& num)
  {
    std::string result;
    return cet::canonical_number(num, result) ?
             result :
             throw fhicl::exception(fhicl::error::parse_error)
               << "The string '" << num
               << "' is not representable as a canonical number.";
  }

  std::string
  canon_str(std::string const& str)
  {
    std::string result;
    return cet::canonical_string(str, result) ?
             result :
             throw fhicl::exception(fhicl::error::parse_error)
               << "The string " + str +
                    " is not representable as a canonical string.\n"
               << "It is likely you have an unescaped (or incorrectly escaped) "
                  "character.";
  }

  void
  rebool(bool& b, bool const value)
  {
    b = value;
  }

  extended_value
  xvalue_vp(bool const b, value_tag const t, std::any const v)
  {
    return extended_value{b, t, v};
  }

  template <typename FwdIter>
  extended_value
  xvalue_dp(bool const b,
            value_tag const t,
            std::any const v,
            FwdIter const pos,
            cet::includer const& s)
  {
    return extended_value{b, t, v, s.src_whereis(pos)};
  }

  complex_t
  cplx(atom_t const& c1, atom_t const& c2)
  {
    return std::make_pair(c1, c2);
  }

  template <typename FwdIter>
  extended_value
  local_lookup(std::string const& name,
               intermediate_table const& tbl,
               bool const in_prolog,
               FwdIter const pos,
               cet::includer const& s) try {
    extended_value result = tbl.find(name);
    result.set_prolog(in_prolog);
    result.set_src_info(s.src_whereis(pos));
    return result;
  }
  catch (fhicl::exception const& e) {
    throw fhicl::exception(fhicl::error::parse_error, "Local lookup error", e)
      << "at " << s.highlighted_whereis(pos) << "\n";
  }

  template <typename FwdIter>
  extended_value
  database_lookup(std::string const&,
                  intermediate_table const&,
                  bool,
                  FwdIter const pos,
                  cet::includer const& s)
  {
    throw fhicl::exception(fhicl::error::unimplemented, "Database lookup error")
      << "at " << s.highlighted_whereis(pos)
      << "\nFHiCL-cpp database lookup not yet available.\n";
  }

  template <typename FwdIter>
  void
  tbl_insert(std::string const& name,
             binding_modifier const m,
             extended_value& value,
             intermediate_table& t,
             FwdIter const pos,
             cet::includer const& s) try {
    set_protection(name, m, value);
    t.insert(name, value);
  }
  catch (fhicl::exception& e) {
    throw fhicl::exception(fhicl::error::parse_error, "Error in assignment:", e)
      << " at " << s.highlighted_whereis(pos) << '\n';
  }

  template <typename FwdIter>
  void
  tbl_erase(std::string const& name,
            intermediate_table& t,
            bool const in_prolog,
            FwdIter const pos,
            cet::includer const& s) try {
    t.erase(name, in_prolog);
  }
  catch (fhicl::exception& e) {
    throw fhicl::exception(
      fhicl::error::parse_error, "Error in erase attempt:", e)
      << " at " << s.highlighted_whereis(pos) << '\n';
  }

  void
  map_insert(std::string const& name,
             binding_modifier const m,
             extended_value& value,
             table_t& t)
  {
    set_protection(name, m, value);
    auto const i = t.find(name);
    if (i != t.end()) {
      auto existing_protection = i->second.protection;
      if (value.protection > existing_protection) {
        throw fhicl::exception(fhicl::error::protection_violation)
          << "Inserting name " << name << " would increase protection from "
          << to_string(existing_protection) << " to "
          << to_string(value.protection) << "\n(previous definition on "
          << i->second.pretty_src_info() << ")\n";
      }
      switch (i->second.protection) {
        case Protection::NONE:
          break;
        case Protection::PROTECT_IGNORE:
          // Do not overwrite protected binding.
          return;
        case Protection::PROTECT_ERROR:
          throw fhicl::exception(fhicl::error::protection_violation)
            << '"' << name << "\" is protected on "
            << i->second.pretty_src_info() << '\n';
      }
    }
    t[name] = value;
  }

  template <typename FwdIter>
  void
  map_insert_loc(std::string const& name,
                 binding_modifier const m,
                 extended_value& value,
                 table_t& t,
                 FwdIter const pos,
                 cet::includer const& s) try {
    map_insert(name, m, value, t);
  }
  catch (fhicl::exception& e) {
    throw fhicl::exception(fhicl::error::parse_error, "Error in assignment:", e)
      << " at " << s.highlighted_whereis(pos) << '\n';
  }

  template <typename FwdIter>
  void
  insert_table_in_table(std::string const& name,
                        intermediate_table& tbl,
                        bool in_prolog,
                        table_t& t,
                        FwdIter pos,
                        cet::includer const& s)
  {
    extended_value const xval = local_lookup(name, tbl, false, pos, s);
    if (!xval.is_a(fhicl::TABLE)) {
      throw fhicl::exception(fhicl::error::type_mismatch, "@table::")
        << "key \"" << name << "\" does not refer to a table at "
        << s.highlighted_whereis(pos) << "\n";
    }
    auto const& incoming = std::any_cast<table_t const&>(xval.value);
    for (auto const& [name, value] : incoming) {
      auto& element = t[name];
      if (!element.is_a(fhicl::UNKNOWN)) {
        // Already exists.
        auto const incoming_protection = value.protection;
        if (incoming_protection > element.protection) {
          throw fhicl::exception(fhicl::error::protection_violation)
            << "@table::" << name << ": inserting name " << name
            << " would increase protection from "
            << to_string(element.protection) << " to "
            << to_string(incoming_protection) << "\n(previous definition on "
            << element.pretty_src_info() << ")\n";
        }
        switch (element.protection) {
          case Protection::NONE:
            break;
          case Protection::PROTECT_IGNORE:
            continue;
          case Protection::PROTECT_ERROR:
            throw fhicl::exception(fhicl::error::protection_violation)
              << "@table::" << name << ": inserting name " << name
              << "would violate protection on existing item"
              << "\n(previous definition on " << element.pretty_src_info()
              << ")\n";
        }
      }
      element = value;
      element.set_prolog(in_prolog);
      element.set_src_info(s.src_whereis(pos));
    }
  }

  template <typename FwdIter>
  void
  insert_table(std::string const& name,
               intermediate_table& tbl,
               bool const in_prolog,
               FwdIter pos,
               cet::includer const& s)
  {
    extended_value const& xval = local_lookup(name, tbl, false, pos, s);
    if (!xval.is_a(fhicl::TABLE)) {
      throw fhicl::exception(fhicl::error::type_mismatch, "@table::")
        << "key \"" << name << "\" does not refer to a table at "
        << s.highlighted_whereis(pos) << "\n";
    }
    auto const& incoming = std::any_cast<table_t const&>(xval.value);
    for (auto const& [name, value] : incoming) {
      auto element = value;
      element.set_prolog(in_prolog);
      element.set_src_info(s.src_whereis(pos));
      tbl.insert(name, std::move(element));
    }
  }

  template <typename FwdIter>
  void
  seq_insert_sequence(std::string const& name,
                      intermediate_table& tbl,
                      bool in_prolog,
                      sequence_t& v,
                      FwdIter pos,
                      cet::includer const& s)
  {
    extended_value const xval = local_lookup(name, tbl, false, pos, s);
    if (!xval.is_a(fhicl::SEQUENCE)) {
      throw fhicl::exception(fhicl::error::type_mismatch, "@sequence::")
        << "key \"" << name << "\" does not refer to a sequence at "
        << s.highlighted_whereis(pos) << "\n";
    }
    auto const& incoming = std::any_cast<sequence_t const&>(xval.value);
    auto it = v.insert(v.end(), incoming.cbegin(), incoming.cend());
    for (auto const e = v.end(); it != e; ++it) {
      using std::to_string;
      it->protection = Protection::NONE;
      it->set_prolog(in_prolog);
      it->set_src_info(s.src_whereis(pos));
    }
  }

  void
  seq_insert_value(extended_value xval, sequence_t& v)
  {
    xval.protection = Protection::NONE;
    v.emplace_back(std::move(xval));
  }

  void
  map_erase(std::string const& name, table_t& t)
  {
    auto const i = t.find(name);
    if (i == t.end())
      return;

    switch (i->second.protection) {
      case Protection::NONE:
        t.erase(name);
      case Protection::PROTECT_IGNORE:
        break;
      case Protection::PROTECT_ERROR:
        throw fhicl::exception(fhicl::error::protection_violation)
          << "Unable to erase " << name << " due to protection.\n";
    }
  }

  template <typename FwdIter>
  void
  map_erase_loc(std::string const& name,
                table_t& t,
                FwdIter pos,
                cet::includer const& s) try {
    map_erase(name, t);
  }
  catch (fhicl::exception& e) {
    throw fhicl::exception(
      fhicl::error::parse_error, "Error in erase attempt:", e)
      << " at " << s.highlighted_whereis(pos) << '\n';
  }

  // ----------------------------------------------------------------------

  template <class FwdIter, class Skip>
  struct value_parser;

  template <class FwdIter, class Skip>
  struct document_parser;

  // ----------------------------------------------------------------------

  template <class FwdIter, class Skip>
  struct value_parser : qi::grammar<FwdIter, extended_value(), Skip> {
    using atom_token = qi::rule<FwdIter, atom_t(), Skip>;
    using complex_token = qi::rule<FwdIter, complex_t(), Skip>;
    using sequence_token = qi::rule<FwdIter, sequence_t(), Skip>;
    using table_token = qi::rule<FwdIter, table_t(), Skip>;
    using value_token = qi::rule<FwdIter, extended_value(), Skip>;

    // default c'tor:
    value_parser();

    // data member:
    extended_value v{};

    // parser rules:
    atom_token nil, boolean;
    atom_token inf;
    atom_token squoted, dquoted;
    atom_token number, string, name, catchall;
    atom_token id;
    complex_token complex;
    sequence_token sequence;
    table_token table;
    value_token value;

  }; // value_parser

  // ----------------------------------------------------------------------

  template <class FwdIter, class Skip>
  struct document_parser : qi::grammar<FwdIter, void(), Skip> {
    using val_parser = value_parser<FwdIter, Skip>;

    using atom_token = typename val_parser::atom_token;
    using sequence_token = typename val_parser::sequence_token;
    using table_token = typename val_parser::table_token;
    using value_token = typename val_parser::value_token;
    using nothing_token = qi::rule<FwdIter, void(), Skip>;

    explicit document_parser(cet::includer const& s);

    // data members:
    bool in_prolog{false};
    intermediate_table tbl{};
    val_parser vp{};

    // parser rules:
    atom_token name, qualname, noskip_qualname, localref, dbref;
    sequence_token sequence;
    table_token table;
    value_token value;
    nothing_token prolog, document;

  }; // document_parser

  // ----------------------------------------------------------------------

  template <class FwdIter, class Skip>
  value_parser<FwdIter, Skip>::value_parser() : value_parser::base_type{value}
  {
    nil =
      lexeme[(qi::string("@nil") >>
              !(graph - char_(",]}")))[_val = phx::bind(canon_nil, qi::_1)]];
    boolean = lexeme[(qi::string("true") | qi::string("false")) >>
                     !(graph - char_(",]}"))];
    inf = lexeme[-(qi::string("+") | qi::string("-")) >>
                 qi::string("infinity") >> !(graph - char_("),]}"))];
    squoted = lexeme[char_('\'') >> *(char_ - char_('\'')) >> char_('\'') >>
                     !(graph - char_(",]}"))];
    dquoted =
      lexeme[raw[char_('\"') >> *(qi::string("\\\"") | (char_ - char_('\"'))) >>
                 char_('\"') >> !(graph - char_(",]}"))]];
    number =
      (fhicl::uint[_val = phx::bind(canon_num, qi::_1)] |
       inf[_val = phx::bind(canon_inf, qi::_1)] | fhicl::real[_val = qi::_1] |
       fhicl::hex[_val = qi::_1] | fhicl::bin[_val = qi::_1]);
    string = (fhicl::ass | fhicl::dss | squoted |
              dquoted)[_val = phx::bind(canon_str, ref(qi::_1))];
    name = fhicl::ass[_val = qi::_1];
    complex = (lit('(') > number > lit(',') > number >
               lit(')'))[_val = phx::bind(cplx, qi::_1, qi::_2)];
    sequence = lit('[') > -(value % ',') > lit(']');
    table =
      lit('{') >
      *((name >> fhicl::binding >>
         value)[phx::bind(map_insert, ref(qi::_1), qi::_2, ref(qi::_3), _val)] |
        (name >>
         (lit(':') > lit("@erase")))[phx::bind(map_erase, ref(qi::_1), _val)]) >
      lit('}');
    id = lit("@id::") > no_skip[fhicl::dbid][_val = qi::_1];
    catchall = shims::catchall[_val = phx::bind(canon_str, ref(qi::_1))];
    value = (nil[_val = phx::bind(xvalue_vp, false, NIL, qi::_1)] |
             boolean[_val = phx::bind(xvalue_vp, false, BOOL, qi::_1)] |
             number[_val = phx::bind(xvalue_vp, false, NUMBER, qi::_1)] |
             complex[_val = phx::bind(xvalue_vp, false, COMPLEX, qi::_1)] |
             string[_val = phx::bind(xvalue_vp, false, STRING, qi::_1)] |
             sequence[_val = phx::bind(xvalue_vp, false, SEQUENCE, qi::_1)] |
             table[_val = phx::bind(xvalue_vp, false, TABLE, qi::_1)] |
             id[_val = phx::bind(xvalue_vp, false, TABLEID, qi::_1)] |
             catchall[_val = phx::bind(xvalue_vp, false, STRING, qi::_1)]);
    nil.name("nil token");
    boolean.name("boolean token");
    inf.name("inf token");
    squoted.name("squoted token");
    dquoted.name("dquoted token");
    number.name("number atom");
    string.name("string atom");
    name.name("name atom");
    complex.name("complex atom");
    sequence.name("sequence");
    table.name("table");
    id.name("id atom");
    value.name("value");
    catchall.name("catchall atom");
  } // value_parser c'tor

  // ----------------------------------------------------------------------

  template <class FwdIter, class Skip>
  document_parser<FwdIter, Skip>::document_parser(cet::includer const& s)
    : document_parser::base_type{document}
  {
    using iter_t = cet::includer::const_iterator;
    name = fhicl::ass;
    qualname =
      fhicl::ass[_val = qi::_1] >>
      *((char_('.') > fhicl::ass)[_val += qi::_1 + qi::_2] |
        (char_('[') > fhicl::uint >
         char_(']'))[_val += qi::_1 + qi::_2 + qi::_3]); // Whitespace permitted
                                                         // before, and around
                                                         // delimiters ( '.',
                                                         // '[', ']').
    noskip_qualname =
      no_skip[fhicl::ass][_val = qi::_1] >>
      *((char_('.') > fhicl::ass)[_val += qi::_1 + qi::_2] |
        (char_('[') > fhicl::uint >
         char_(']'))[_val += qi::_1 + qi::_2 + qi::_3]); // Whitespace permitted
                                                         // around delimiters
                                                         // ('.', '[', ']')
                                                         // only.
    localref = lit("@local::") > noskip_qualname;
    dbref = lit("@db::") > noskip_qualname;
    // Can't use simple, "list context" due to the possibility of one of
    // the list elements actually returning multiple elements.
    sequence =
      lit('[') > -(((value[phx::bind(seq_insert_value, ref(qi::_1), _val)]) |
                    ((iter_pos >> lit("@sequence::")) >
                     noskip_qualname)[phx::bind(&seq_insert_sequence<iter_t>,
                                                ref(qi::_2),
                                                ref(tbl),
                                                ref(in_prolog),
                                                _val,
                                                qi::_1,
                                                ref(s))])) >
      *(lit(',') > ((value[phx::bind(seq_insert_value, ref(qi::_1), _val)]) |
                    ((iter_pos >> lit("@sequence::")) >
                     noskip_qualname)[phx::bind(&seq_insert_sequence<iter_t>,
                                                ref(qi::_2),
                                                ref(tbl),
                                                ref(in_prolog),
                                                _val,
                                                qi::_1,
                                                ref(s))])) > lit(']');
    table = lit('{') >
            *((iter_pos >> name >> fhicl::binding >>
               value)[phx::bind(&map_insert_loc<iter_t>,
                                ref(qi::_2),
                                qi::_3,
                                ref(qi::_4),
                                _val,
                                qi::_1,
                                ref(s))] |
              (iter_pos >> name >> (lit(':') > lit("@erase")))[phx::bind(
                &map_erase_loc<iter_t>, ref(qi::_2), _val, qi::_1, ref(s))] |
              ((iter_pos >> lit("@table::")) >
               noskip_qualname)[phx::bind(&insert_table_in_table<iter_t>,
                                          ref(qi::_2),
                                          ref(tbl),
                                          ref(in_prolog),
                                          _val,
                                          qi::_1,
                                          ref(s))]) > lit('}');
    // Clang does not like this (multiple unsequenced modifications to '_val'
    // [-Werror,-Wunsequenced]) TEMPORARILY wrap until validity checked
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsequenced"
#endif
    value =
      ((iter_pos >> vp.nil)
         [_val = phx::bind(
            &xvalue_dp<iter_t>, ref(in_prolog), NIL, qi::_2, qi::_1, ref(s))] |
       (iter_pos >> vp.boolean)
         [_val = phx::bind(
            &xvalue_dp<iter_t>, ref(in_prolog), BOOL, qi::_2, qi::_1, ref(s))] |
       (iter_pos >> vp.number)[_val = phx::bind(&xvalue_dp<iter_t>,
                                                ref(in_prolog),
                                                NUMBER,
                                                qi::_2,
                                                qi::_1,
                                                ref(s))] |
       (iter_pos >> vp.complex)[_val = phx::bind(&xvalue_dp<iter_t>,
                                                 ref(in_prolog),
                                                 COMPLEX,
                                                 qi::_2,
                                                 qi::_1,
                                                 ref(s))] |
       (iter_pos >> vp.string)[_val = phx::bind(&xvalue_dp<iter_t>,
                                                ref(in_prolog),
                                                STRING,
                                                qi::_2,
                                                qi::_1,
                                                ref(s))] |
       (iter_pos >> localref)[_val = phx::bind(&local_lookup<iter_t>,
                                               qi::_2,
                                               ref(tbl),
                                               ref(in_prolog),
                                               qi::_1,
                                               ref(s))] |
       (iter_pos >> dbref)[_val = phx::bind(&database_lookup<iter_t>,
                                            qi::_2,
                                            ref(tbl),
                                            ref(in_prolog),
                                            qi::_1,
                                            ref(s))] |
       (iter_pos >> vp.id)[_val = phx::bind(&xvalue_dp<iter_t>,
                                            ref(in_prolog),
                                            TABLEID,
                                            qi::_2,
                                            qi::_1,
                                            ref(s))] |
       (iter_pos >> sequence)[_val = phx::bind(&xvalue_dp<iter_t>,
                                               ref(in_prolog),
                                               SEQUENCE,
                                               qi::_2,
                                               qi::_1,
                                               ref(s))] |
       (iter_pos >> table)[_val = phx::bind(&xvalue_dp<iter_t>,
                                            ref(in_prolog),
                                            TABLE,
                                            qi::_2,
                                            qi::_1,
                                            ref(s))] |
       (iter_pos >> vp.catchall)[_val = phx::bind(&xvalue_dp<iter_t>,
                                                  ref(in_prolog),
                                                  STRING,
                                                  qi::_2,
                                                  qi::_1,
                                                  ref(s))]);
#ifdef __clang__
#pragma clang diagnostic pop
#endif
    prolog = lit("BEGIN_PROLOG")[phx::bind(rebool, ref(in_prolog), true)] >
             *((iter_pos >> qualname >> fhicl::binding >>
                value)[phx::bind(&tbl_insert<iter_t>,
                                 ref(qi::_2),
                                 qi::_3,
                                 ref(qi::_4),
                                 ref(tbl),
                                 qi::_1,
                                 ref(s))] |
               (iter_pos >> qualname >>
                (lit(':') > lit("@erase")))[phx::bind(&tbl_erase<iter_t>,
                                                      qi::_2,
                                                      ref(tbl),
                                                      ref(in_prolog),
                                                      qi::_1,
                                                      ref(s))] |
               ((iter_pos >> lit("@table::")) >
                noskip_qualname)[phx::bind(&insert_table<iter_t>,
                                           qi::_2,
                                           ref(tbl),
                                           ref(in_prolog),
                                           qi::_1,
                                           ref(s))]) >
             lit("END_PROLOG")[phx::bind(rebool, ref(in_prolog), false)];
    document =
      (*prolog) >> *((iter_pos >> qualname >> fhicl::binding >>
                      value)[phx::bind(&tbl_insert<iter_t>,
                                       ref(qi::_2),
                                       qi::_3,
                                       ref(qi::_4),
                                       ref(tbl),
                                       qi::_1,
                                       ref(s))] |
                     (iter_pos >> qualname >>
                      (lit(':') > lit("@erase")))[phx::bind(&tbl_erase<iter_t>,
                                                            qi::_2,
                                                            ref(tbl),
                                                            ref(in_prolog),
                                                            qi::_1,
                                                            ref(s))] |
                     ((iter_pos >> lit("@table::")) >
                      noskip_qualname)[phx::bind(&insert_table<iter_t>,
                                                 qi::_2,
                                                 ref(tbl),
                                                 ref(in_prolog),
                                                 qi::_1,
                                                 ref(s))]);
    name.name("name atom");
    localref.name("localref atom");
    dbref.name("dbref atom");
    qualname.name("qualified name");
    noskip_qualname.name("qualified name (no pre-skip)");
    sequence.name("sequence");
    table.name("table");
    value.name("value");
    prolog.name("prolog");
    document.name("document");
  } // document_parser c'tor
}

// ----------------------------------------------------------------------

bool
fhicl::parse_value_string(std::string const& s,
                          extended_value& result,
                          std::string& unparsed)
{
  using iter_t = std::string::const_iterator;
  using ws_t = qi::rule<iter_t>;
  ws_t whitespace = space | lit('#') >> *(char_ - eol) >> eol |
                    lit("//") >> *(char_ - eol) >> eol;
  value_parser<iter_t, ws_t> p;
  iter_t begin = s.begin();
  iter_t const end = s.end();
  bool const b =
    qi::phrase_parse(begin, end, p >> *whitespace, whitespace, result) &&
    begin == end;
  unparsed = std::string(begin, end);
  return b;
} // parse_value_string()

// ----------------------------------------------------------------------

namespace {
  void
  parse_document_(cet::includer s, intermediate_table& result)
  {
    using namespace std::string_literals;
    using iter_t = cet::includer::const_iterator;
    using ws_t = qi::rule<iter_t>;
    ws_t whitespace = space | lit('#') >> *(char_ - eol) >> eol |
                      lit("//") >> *(char_ - eol) >> eol;
    document_parser<iter_t, ws_t> p(s);
    iter_t begin = s.begin();
    iter_t const end = s.end();
    bool b = false;
    try {
      b = qi::phrase_parse(begin, end, p, whitespace);
    }
    catch (qi::expectation_failure<iter_t> const& e) {
      begin = e.first;
    }
    std::string const unparsed(begin, end);
    if (b && unparsed.empty()) {
      result = std::move(p.tbl);
    } else {
      auto e = fhicl::exception(fhicl::parse_error, "detected at or near")
               << s.highlighted_whereis(begin) << "\n";
      if (unparsed.find("BEGIN_PROLOG"s) == 0ull) {
        e << "PROLOG blocks must be both contiguous and not nested.\n";
      }
      throw e;
    }
  }
}

void
fhicl::parse_document(std::string const& filename,
                      cet::filepath_maker& maker,
                      intermediate_table& result)
{
  parse_document_(cet::includer{filename, maker}, result);
}

void
fhicl::parse_document(std::istream& is,
                      cet::filepath_maker& maker,
                      intermediate_table& result)
{
  parse_document_(cet::includer(is, maker), result);
}

// ======================================================================
