#ifndef art_root_io_detail_RootErrorParser_h
#define art_root_io_detail_RootErrorParser_h

#include "messagefacility/MessageLogger/MessageLogger.h"

#include <ostream>
#include <sstream>

namespace art::detail {

  struct RootErrorPayload {
    std::string location;
    std::string message;
  };

  class RootErrorParser {
  public:
    explicit RootErrorParser(RootErrorPayload const& payload)
      : payload_{payload}
    {}

    explicit RootErrorParser(std::string const& location,
                             std::string const& message)
      : payload_{location, message}
    {}

    bool
    has_message(std::string const& message_pattern) const
    {
      return payload_.message.find(message_pattern) != std::string::npos;
    }

    bool
    in(std::string const& location) const
    {
      return payload_.location.find(location) != std::string::npos;
    }

    bool
    has_message_in(std::string const& message_pattern,
                   std::string const& location) const
    {
      return has_message(message_pattern) && in(location);
    }

    template <typename... Ts>
    bool
    has_any_of_these_messages(Ts... ts) const
    {
      return (has_message(ts) || ...);
    }

    template <typename... Ts>
    bool
    in_any_of_these_locations(Ts... ts) const
    {
      return (in(ts) || ...);
    }

  private:
    RootErrorPayload payload_;
  };

} // art::detail

#endif /* art_root_io_detail_RootErrorParser_h */

// Local Variables:
// mode: c++
// End:
