#ifndef cetlib_except_coded_exception_h
#define cetlib_except_coded_exception_h

// ======================================================================
//
//  coded_exception: A family of enumeration-based exception types
//
// ======================================================================

#include "cetlib_except/exception.h"
#include <string>

namespace cet {
  template <typename Code, std::string translate(Code)>
  class coded_exception;
}

// ======================================================================

template <typename Code, std::string translate(Code)>
class cet::coded_exception : public cet::exception {
public:
  // --- c'tors, d'tor:
  explicit coded_exception(Code c);
  coded_exception(Code c, std::string const& m);
  coded_exception(Code c, std::string const& m, exception const& e);
  virtual ~coded_exception() = default;

  // --- inspectors:
  constexpr Code categoryCode() const;
  constexpr int returnCode() const;
  static std::string codeToString(Code code);

private:
  Code category_;

}; // coded_exception<>

// ======================================================================
// c'tors, d'tor:

template <typename Code, std::string translate(Code)>
cet::coded_exception<Code, translate>::coded_exception(Code c)
  : exception{codeToString(c)}, category_{c}
{}

template <typename Code, std::string translate(Code)>
cet::coded_exception<Code, translate>::coded_exception(Code c,
                                                       std::string const& m)
  : exception{codeToString(c), m}, category_{c}
{}

template <typename Code, std::string translate(Code)>
cet::coded_exception<Code, translate>::coded_exception(Code c,
                                                       std::string const& m,
                                                       exception const& e)
  : exception{codeToString(c), m, e}, category_{c}
{}

// ======================================================================
// inspectors:

template <typename Code, std::string translate(Code)>
constexpr Code
cet::coded_exception<Code, translate>::categoryCode() const
{
  return category_;
}

template <typename Code, std::string translate(Code)>
constexpr int
cet::coded_exception<Code, translate>::returnCode() const
{
  return static_cast<int>(category_);
}

template <typename Code, std::string translate(Code)>
std::string
cet::coded_exception<Code, translate>::codeToString(Code code)
{
  return translate(code);
}

  // ======================================================================

#endif /* cetlib_except_coded_exception_h */

// Local variables:
// mode: c++
// End:
