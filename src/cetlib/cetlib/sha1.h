#ifndef cetlib_sha1_h
#define cetlib_sha1_h

// ======================================================================
//
// sha1: Secure Hash Algorithm as defined in FIPS PUB 180-1, 1993-05-11
//
// ======================================================================

#ifdef __APPLE__
#define COMMON_DIGEST_FOR_OPENSSL
#include <CommonCrypto/CommonDigest.h>
#undef COMMON_DIGEST_FOR_OPENSSL
#else
#include <openssl/sha.h>
#endif
#include <array>
#include <string>

namespace cet {
  class sha1;
}

// ======================================================================

class cet::sha1 {
public:
  static std::size_t constexpr digest_sz{20};
  using uchar = unsigned char;
  using digest_t = std::array<uchar, digest_sz>;

  sha1();
  explicit sha1(std::string const& mesg);
  explicit sha1(char const mesg);

  void reset();

  sha1& operator<<(std::string const& mesg);
  sha1& operator<<(char const mesg);
  digest_t digest();

private:
  SHA_CTX context;

}; // sha1

// ======================================================================

#endif /* cetlib_sha1_h */

// Local variables:
// mode: c++
// End:
