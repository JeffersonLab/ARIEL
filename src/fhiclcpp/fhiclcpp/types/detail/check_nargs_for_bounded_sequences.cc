#include "fhiclcpp/types/detail/check_nargs_for_bounded_sequences.h"
#include "fhiclcpp/types/detail/validationException.h"

#include <cstddef>
#include <string>

void
fhicl::detail::check_nargs_for_bounded_sequences(std::string const& key,
                                                 std::size_t const expected,
                                                 std::size_t const nargs)
{
  constexpr auto maybe_plural = [](std::size_t const nargs) {
    return nargs == 1ull ? "" : "s";
  };
  constexpr auto has_or_have = [](std::size_t const nargs) {
    return nargs == 1ull ? "has" : "have";
  };
  if (nargs != expected) {
    std::ostringstream oss;
    oss << '\n'
        << nargs << " value" << maybe_plural(nargs) << ' ' << has_or_have(nargs)
        << " been provided for the parameter with key:\n"
        << "  " << key << '\n'
        << "which expects " << expected << " value" << maybe_plural(expected)
        << '.';
    throw detail::validationException{oss.str().c_str()};
  }
}
