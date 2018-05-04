#include "cetlib/sha1.h"
#include <cstring>

using cet::sha1;

sha1::sha1()
{
  reset();
}

sha1::sha1(std::string const& mesg)
{
  reset();
  operator<<(mesg);
}

sha1::sha1(char const mesg)
{
  reset();
  operator<<(mesg);
}

void
sha1::reset()
{
  SHA1_Init(&context);
}

sha1&
sha1::operator<<(std::string const& mesg)
{
  auto data = reinterpret_cast<uchar const*>(&mesg[0]);
  SHA1_Update(&context, data, mesg.size());
  return *this;
}

sha1&
sha1::operator<<(char const mesg)
{
  auto data = reinterpret_cast<uchar const*>(&mesg);
  SHA1_Update(&context, data, 1u);
  return *this;
}

sha1::digest_t
sha1::digest()
{
  digest_t result{{}};
  SHA1_Final(&result[0], &context);
  return result;
}
