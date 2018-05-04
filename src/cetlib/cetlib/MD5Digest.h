#ifndef cetlib_MD5Digest_h
#define cetlib_MD5Digest_h

#include <iosfwd>
#include <string>
#ifdef __APPLE__
#define COMMON_DIGEST_FOR_OPENSSL
#include <CommonCrypto/CommonDigest.h>
#undef COMMON_DIGEST_FOR_OPENSSL
#else
#include <openssl/md5.h>
#endif

namespace cet {

  struct MD5Result {
    // The default-constructed MD5Result is invalid; all others are
    // valid. The MD5 digest of the empty string is the value of the
    // default-constructed MD5Result.
    MD5Result();

    // This is the MD5 digest.
    unsigned char bytes[16];

    // Convert the digest to a printable string (the 'hexdigest')
    std::string toString() const;

    // The MD5 digest (not hexdigest) in string form
    // 'std::basic_string<char>', rather than
    // 'unsigned char [16]'
    std::string compactForm() const;

    // Set our data from the given hexdigest string.
    void fromHexifiedString(std::string const& s);

    bool isValid() const;
  };

  bool operator==(MD5Result const& a, MD5Result const& b);
  bool operator<(MD5Result const& a, MD5Result const& b);

  inline bool
  operator!=(MD5Result const& a, MD5Result const& b)
  {
    return !(a == b);
  }

  inline std::ostream&
  operator<<(std::ostream& os, MD5Result const& r)
  {
    os << r.toString();
    return os;
  }

  // Digest creates an MD5 digest of the given string. The digest can
  // be updated by using 'append'.
  class MD5Digest {
  public:
    MD5Digest();
    explicit MD5Digest(std::string const& s);

    void append(std::string const& s);
    MD5Result digest() const;

  private:
    mutable MD5_CTX context_;
  };
}

#endif /* cetlib_MD5Digest_h */

// Local Variables:
// mode: c++
// End:
