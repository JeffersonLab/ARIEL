#include "canvas/Persistency/Provenance/Timestamp.h"
#include "boost/date_time/posix_time/posix_time.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>

namespace art {
  std::string
  to_iso_string_assuming_unix_epoch(Timestamp const& ts)
  {
    auto const t = static_cast<time_t>(ts.value());
    auto const p = boost::posix_time::from_time_t(t);
    return boost::posix_time::to_iso_string(p);
  }
}
