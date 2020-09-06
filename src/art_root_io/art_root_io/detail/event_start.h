#ifndef art_root_io_detail_event_start_h
#define art_root_io_detail_event_start_h

#include "canvas/Persistency/Provenance/IDNumber.h"

#include <string>
#include <tuple>

namespace art {
  namespace detail {
    std::tuple<RunNumber_t, SubRunNumber_t, EventNumber_t>
    event_start_for_sampled_input(std::string const& str_num);
  }
}

#endif /* art_root_io_detail_event_start_h */

// Local Variables:
// mode: c++
// End:
