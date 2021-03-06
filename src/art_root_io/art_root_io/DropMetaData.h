#ifndef art_root_io_DropMetaData_h
#define art_root_io_DropMetaData_h

#include <string>

namespace art {

  class DropMetaData {
  public:
    enum enum_t { DropNone, DropPrior, DropAll };

    DropMetaData(enum_t e);
    DropMetaData(std::string const& config);

    bool
    operator==(enum_t const e) const
    {
      return value_ == e;
    }

    bool
    operator!=(enum_t const e) const
    {
      return !operator==(e);
    }

  private:
    enum_t strToValue_(std::string const& dropMetaData);
    enum_t value_;
  };

} // namespace art

#endif /* art_root_io_DropMetaData_h */

// Local Variables:
// mode: c++
// End:
