// ======================================================================
//
// exception
//
// ======================================================================

#include "cetlib_except/exception.h"
#include <cctype>

using cet::exception;

using Category = exception::Category;
using CategoryList = exception::CategoryList;

// ======================================================================
// helpers:

namespace {

  bool
  ends_with_whitespace(std::string const& s)
  {
    return s.empty() || std::isspace(*(s.end() - 1));
  }

  bool
  ends_with_newline(std::string const& s)
  {
    return s.empty() || *(s.end() - 1) == '\n';
  }

  constexpr char const* indent{"  "};

  std::string
  indent_string(std::string const& s)
  {
    std::string result{indent};
    result.append(s);
    size_t cpos = 0;
    while ((cpos = result.find('\n', cpos)) != std::string::npos) {
      ++cpos;
      if (cpos != result.size()) {
        result.insert(cpos, indent);
      }
    }
    return result;
  }

} // anon. namespace

// ======================================================================
// c'tors, d'tors:

exception::exception(Category const& c) : category_{{c}} {}

exception::exception(Category const& c, std::string const& m) : category_{{c}}
{
  ost_ << m;
  if (!ends_with_whitespace(m))
    ost_ << ' ';
}

exception::exception(Category const& c,
                     std::string const& m,
                     exception const& e)
  : category_{{c}}
{
  if (!m.empty())
    ost_ << m << '\n';
  category_.insert(category_.end(), e.history().begin(), e.history().end());
  append(e);
}

// ======================================================================
// copy c'tor:

exception::exception(exception const& other) noexcept
  : category_{other.category_}, what_{other.what_}
{
  ost_ << other.ost_.str();
}

// ======================================================================
// inspectors:

char const*
exception::what() const noexcept
{
  what_ = explain_self();
  return what_.c_str();
}

std::string
exception::explain_self() const
{
  std::ostringstream ost;

  ost << "---- " << category() << " BEGIN\n";

  std::string part(indent_string(ost_.str()));
  ost << part;
  if (!ends_with_newline(part))
    ost << '\n';

  ost << "---- " << category() << " END\n";

  return ost.str();
}

std::string
exception::category() const
{
  return category_.front();
}

CategoryList const&
exception::history() const
{
  return category_;
}

std::string
exception::root_cause() const
{
  return category_.back();
}

// ======================================================================
// mutators:

void
exception::append(exception const& e)
{
  ost_ << e.explain_self();
}

void
exception::append(std::string const& more_information)
{
  ost_ << more_information;
}

void
exception::append(char const more_information[])
{
  ost_ << more_information;
}

void
exception::append(std::ostream& f(std::ostream&))
{
  f(ost_);
}

void
exception::append(std::ios_base& f(std::ios_base&))
{
  f(ost_);
}

// ======================================================================

std::ostream&
cet::operator<<(std::ostream& os, exception const& e)
{
  return os << e.explain_self();
}

// ======================================================================
