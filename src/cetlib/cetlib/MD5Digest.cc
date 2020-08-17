// change
#include <iomanip>
#include <sstream>

#include "cetlib/MD5Digest.h"
#include "cetlib/nybbler.h"

namespace cet {
  namespace {
    MD5Result const&
    invalidResult()
    {
      static const MD5Result val;
      return val;
    }
  }

  //--------------------------------------------------------------------
  //
  // MD5Result and associated free functions
  //

  void
  set_to_default(MD5Result& val)
  {
    val.bytes[0] = 0xd4;
    val.bytes[1] = 0x1d;
    val.bytes[2] = 0x8c;
    val.bytes[3] = 0xd9;
    val.bytes[4] = 0x8f;
    val.bytes[5] = 0x00;
    val.bytes[6] = 0xb2;
    val.bytes[7] = 0x04;
    val.bytes[8] = 0xe9;
    val.bytes[9] = 0x80;
    val.bytes[10] = 0x09;
    val.bytes[11] = 0x98;
    val.bytes[12] = 0xec;
    val.bytes[13] = 0xf8;
    val.bytes[14] = 0x42;
    val.bytes[15] = 0x7e;
  }

  MD5Result::MD5Result() { set_to_default(*this); }

  std::string
  MD5Result::toString() const
  {
    // Note: The code used to be this:
    //
    // std::ostringstream os;
    // os << std::hex << std::setfill('0');
    // for (size_t i = 0; i < sizeof(bytes); ++i)
    //  os << std::setw(2) << static_cast<int>(bytes[i]);
    // return os.str();
    //
    // However profiling shows that this causes a measurable
    // slowdown.  The following code does the same thing, but
    // is much faster.
    constexpr char hex_bytes[] = "0123456789abcdef";
    std::string ret;
    ret.resize(sizeof(bytes) << 1);
    for (size_t i = 0; i < sizeof(bytes); ++i) {
      ret[i << 1] = hex_bytes[bytes[i] >> 4];
      ret[(i << 1) + 1] = hex_bytes[bytes[i] & 0x0F];
    }
    return ret;
  }

  std::string
  MD5Result::compactForm() const
  {
    // This is somewhat dangerous, because the conversion of 'unsigned
    // char' to 'char' may be undefined if 'char' is a signed type
    // (4.7p3 in the Standard).
    const char* p = reinterpret_cast<const char*>(&bytes[0]);
    return std::string(p, p + sizeof(bytes));
  }

  void
  MD5Result::fromHexifiedString(std::string const& hexy)
  {
    switch (hexy.size()) {
      case 0: {
        set_to_default(*this);
      } break;
      case 32: {
        std::string const chars = cet::nybbler(hexy).as_char();
        std::string::const_iterator it = chars.begin();
        for (size_t i = 0; i != 16; ++i)
          bytes[i] = *it++;
      } break;
      default: {
        throw "String of illegal length given to MD5Result::fromHexifiedString";
      }
    }
  }

  bool
  MD5Result::isValid() const
  {
    return (*this != invalidResult());
  }

  bool
  operator==(MD5Result const& a, MD5Result const& b)
  {
    return std::equal(a.bytes, a.bytes + sizeof(a.bytes), b.bytes);
  }

  bool
  operator<(MD5Result const& a, MD5Result const& b)
  {
    return std::lexicographical_compare(
      a.bytes, a.bytes + sizeof(a.bytes), b.bytes, b.bytes + sizeof(b.bytes));
  }

  //--------------------------------------------------------------------
  //
  // MD5Digest
  //

  MD5Digest::MD5Digest() : context_() { MD5_Init(&context_); }

  MD5Digest::MD5Digest(std::string const& s) : context_()
  {
    MD5_Init(&context_);
    this->append(s);
  }

  void
  MD5Digest::append(std::string const& s)
  {
    using md5_byte_t = unsigned char;
    md5_byte_t const* data = reinterpret_cast<md5_byte_t const*>(s.data());
    MD5_Update(&context_, const_cast<md5_byte_t*>(data), s.size());
  }

  MD5Result
  MD5Digest::digest() const
  {
    MD5Result aDigest;
    MD5_Final(aDigest.bytes, &context_);
    return aDigest;
  }
}
