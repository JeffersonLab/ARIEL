#ifndef art_root_io_GetFileFormatVersion_h
#define art_root_io_GetFileFormatVersion_h

namespace art {
  // We do not inline this function to help avoid inconsistent
  // versions being inlined into different libraries.

  int getFileFormatVersion();
} // namespace art
#endif /* art_root_io_GetFileFormatVersion_h */

// Local Variables:
// mode: c++
// End:
