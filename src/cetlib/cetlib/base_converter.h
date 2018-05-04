#ifndef cetlib_base_converter_h
#define cetlib_base_converter_h

// ==================================================================
//
// base_converter : Arbitary precision base conversion
//
// ==================================================================

#include <string>

namespace cet {
  class base_converter;
}

// ==================================================================

class cet::base_converter {
public:
  std::string const& get_source_base_set() const;
  std::string const& get_target_base_set() const;
  unsigned int get_source_base() const;
  unsigned int get_target_base() const;

  base_converter(std::string const& sourceBaseSet,
                 std::string const& targetBaseSet);

  static std::string dec_to_bin(std::string value);
  static std::string bin_to_dec(std::string value);
  static std::string dec_to_hex(std::string value);
  static std::string hex_to_dec(std::string value);

  std::string convert(std::string value) const;

private:
  static unsigned int divide(std::string const& baseDigits,
                             std::string& x,
                             unsigned int y);

  static unsigned int base2dec(std::string const& baseDigits,
                               std::string const& value);

  static std::string dec2base(std::string const& baseDigits,
                              unsigned int value);

private:
  static constexpr char const*
  binary_set()
  {
    return "01";
  }
  static constexpr char const*
  decimal_set()
  {
    return "0123456789";
  }
  static constexpr char const*
  hex_set()
  {
    return "0123456789ABCDEF";
  }

  std::string source_base_set_;
  std::string target_base_set_;
};

inline std::string const&
cet::base_converter::get_source_base_set() const
{
  return source_base_set_;
}

inline std::string const&
cet::base_converter::get_target_base_set() const
{
  return target_base_set_;
}

inline unsigned int
cet::base_converter::get_source_base() const
{
  return (unsigned int)source_base_set_.length();
}

inline unsigned int
cet::base_converter::get_target_base() const
{
  return (unsigned int)target_base_set_.length();
}

#endif /* cetlib_base_converter_h */

// Local Variables:
// mode: c++
// End:
