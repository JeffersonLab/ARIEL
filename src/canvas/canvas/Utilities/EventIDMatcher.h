#ifndef canvas_Utilities_EventIDMatcher_h
#define canvas_Utilities_EventIDMatcher_h
// vim: set sw=2 :

#include "canvas/Persistency/Provenance/EventID.h"

#include <string>
#include <vector>

namespace art {

  class EventIDMatcher {

  private: // TYPES
    class PatternRangeElement {

    public:
      PatternRangeElement(unsigned low, unsigned high, bool wildcard)
        : low_(low), high_(high), wildcard_(wildcard)
      {}

    public:
      unsigned low_;
      unsigned high_;
      bool wildcard_;
    };

  public: // MEMBER FUNCTIONS
    explicit EventIDMatcher(std::string const& pattern);

    explicit EventIDMatcher(std::vector<std::string> const& patterns);

    bool operator()(EventID const&) const;

    bool match(EventID const&) const;

  private: // MEMBER FUNCTIONS
    void parse_pattern();

  private: // MEMBER DATA
    std::vector<std::string> pattern_;
    std::vector<std::vector<std::vector<PatternRangeElement>>> parsed_patterns_;
  };

} // namespace art

#endif /* canvas_Utilities_EventIDMatcher_h */

// Local Variables:
// mode: c++
// End:
