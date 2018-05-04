#ifndef canvas_Persistency_Provenance_FileFormatVersion_h
#define canvas_Persistency_Provenance_FileFormatVersion_h

#include <iosfwd>
#include <string>

namespace art {

  struct FileFormatVersion {
    FileFormatVersion() = default;
    explicit FileFormatVersion(int vers) : FileFormatVersion{vers, {}} {}
    FileFormatVersion(int vers, std::string const& era)
      : value_{vers}, era_{era}
    {}
    bool
    isValid() const
    {
      return value_ >= 0;
    }

    int value_{-1};
    std::string era_{};
  };

  // Note for backward compatibility and efficiency reasons, comparison
  // operators do not take era into account. This must be checked explicitly.
  inline bool
  operator==(FileFormatVersion const& a, FileFormatVersion const& b)
  {
    return a.value_ == b.value_;
  }

  inline bool
  operator!=(FileFormatVersion const& a, FileFormatVersion const& b)
  {
    return !(a == b);
  }

  std::ostream& operator<<(std::ostream& os, FileFormatVersion const& ff);
}

#endif /* canvas_Persistency_Provenance_FileFormatVersion_h */

// Local Variables:
// mode: c++
// End:
