#ifndef r10870_coded_exception_h
#define r10870_coded_exception_h

// ======================================================================
//
//  coded_exception: A family of enumeration-based exception types
//
// ======================================================================

#include "exception.h"
#include <string>

namespace r10870 {
  template <typename Code, std::string translate(Code)>
  class coded_exception;
}

// ======================================================================

template <typename Code, std::string translate(Code)>
class r10870::coded_exception : public r10870::exception {
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
r10870::coded_exception<Code, translate>::coded_exception(Code c)
  : exception{codeToString(c)}, category_{c}
{}

template <typename Code, std::string translate(Code)>
r10870::coded_exception<Code, translate>::coded_exception(Code c,
                                                       std::string const& m)
  : exception{codeToString(c), m}, category_{c}
{}

template <typename Code, std::string translate(Code)>
r10870::coded_exception<Code, translate>::coded_exception(Code c,
                                                       std::string const& m,
                                                       exception const& e)
  : exception{codeToString(c), m, e}, category_{c}
{}

// ======================================================================
// inspectors:

template <typename Code, std::string translate(Code)>
constexpr Code
r10870::coded_exception<Code, translate>::categoryCode() const
{
  return category_;
}

template <typename Code, std::string translate(Code)>
constexpr int
r10870::coded_exception<Code, translate>::returnCode() const
{
  return static_cast<int>(category_);
}

template <typename Code, std::string translate(Code)>
std::string
r10870::coded_exception<Code, translate>::codeToString(Code code)
{
  return translate(code);
}

// ======================================================================

#endif /* r10870_coded_exception_h */

// Local variables:
// mode: c++
// End:
