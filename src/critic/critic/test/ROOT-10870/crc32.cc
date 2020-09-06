// ======================================================================
//
// crc32: Calculate a CRC32 checksum
//
// ======================================================================
//
// The following comments are provided by way of attribution in order to
// reflect the origin of the algorithm embedded herein.  They are copied
// verbatim, but have been lightly edited for stylistic consistency and
// to remove details not relevant to this implementation:
//
// ----------------------------------------------------------------------
//
// C implementation of CRC-32 checksums for NAACCR records.  Code is
// based upon and utilizes algorithm published by Ross Williams.
//
// Provided by: Eric Durbin, Kentucky Cancer Registry, University of
//   Kentucky, October 14, 1998
//
// Status: Public Domain
//
// CRC LOOKUP TABLE
// ================
// The following CRC lookup table was generated automagically by the
// Rocksoft^tm Model CRC Algorithm Table Generation Program V1.0 using
// the following model parameters:
//
//    Width   : 4 bytes.
//    Poly    : 0x04C11DB7L
//    Reverse : TRUE.
//
// For more information on the Rocksoft^tm Model CRC Algorithm, see the
// document titled "A Painless Guide to CRC Error Detection Algorithms"
// by Ross Williams (ross@guest.adelaide.edu.au.). This document is
// likely to be in the FTP archive "ftp.adelaide.edu.au/pub/rocksoft".
//
// ======================================================================

#include "crc32.h"

using r10870::crc32;

static_assert(crc32{""}.digest() == 0,
              "Digest for CRC32 of empty string is not 0!");

// ----------------------------------------------------------------------

constexpr std::uint32_t r10870::crc32::crctable[];

crc32::crc32(std::string const& mesg)
{
  operator<<(mesg);
}

crc32&
crc32::operator<<(std::string const& mesg)
{
  for (uchar const ch : mesg) {
    context = crctable[(context ^ ch) & 0xFFL] ^ (context >> 8);
  }
  return *this;
}

// ======================================================================
