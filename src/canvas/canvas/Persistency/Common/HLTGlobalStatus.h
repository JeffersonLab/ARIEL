#ifndef canvas_Persistency_Common_HLTGlobalStatus_h
#define canvas_Persistency_Common_HLTGlobalStatus_h

/** \class art::HLTGlobalStatus
 *
 *  The HLT global status, summarising the status of the individual
 *  HLT triggers, is implemented as a vector of HLTPathStatus objects.
 *
 *  If the user wants map-like indexing of HLT triggers through their
 *  names as key, s/he must use the TriggerNamesService.
 *
 *  \author Martin Grunewald
 */

#include "canvas/Persistency/Common/HLTPathStatus.h"
#include "canvas/Persistency/Common/HLTenums.h"
#include "cetlib/container_algorithms.h"
#include <ostream>
#include <vector>

// ----------------------------------------------------------------------

namespace art {

  class HLTGlobalStatus {
    // Status of each HLT path
    std::vector<HLTPathStatus> paths_{};

  public:
    // Constructor - for n paths
    HLTGlobalStatus() = default;
    explicit HLTGlobalStatus(std::size_t const n) : paths_(n) {}

    // Get number of paths stored
    std::size_t
    size() const
    {
      return paths_.size();
    }

    // Reset status for all paths
    void
    reset()
    {
      cet::for_all(paths_, [](auto& path) { path.reset(); });
    }

    // global "state" variables calculated on the fly!

    // Was at least one path run?
    bool
    wasrun() const
    {
      return state_on_demand(0);
    }
    // Has at least one path accepted the event? If no paths were
    // run, or there are no paths, the answer is, "yes."
    bool
    accept() const
    {
      return state_on_demand(1);
    }
    // Has any path encountered an error (exception)
    bool
    error() const
    {
      return state_on_demand(2);
    }

    // get hold of individual elements, using safe indexing with "at" which
    // throws!
    HLTPathStatus const&
    at(std::size_t const i) const
    {
      return paths_.at(i);
    }
    HLTPathStatus&
    at(std::size_t const i)
    {
      return paths_.at(i);
    }

    HLTPathStatus const& operator[](std::size_t const i) const
    {
      return paths_.at(i);
    }
    HLTPathStatus& operator[](std::size_t const i) { return paths_.at(i); }

    // Was ith path run?
    bool
    wasrun(std::size_t const i) const
    {
      return at(i).wasrun();
    }
    // Has ith path accepted the event?
    bool
    accept(std::size_t const i) const
    {
      return at(i).accept();
    }
    // Has ith path encountered an error (exception)?
    bool
    error(std::size_t const i) const
    {
      return at(i).error();
    }

    // Get status of ith path
    hlt::HLTState
    state(std::size_t const i) const
    {
      return at(i).state();
    }
    // Get index (slot position) of module giving the decision of the ith path
    std::size_t
    index(std::size_t const i) const
    {
      return at(i).index();
    }
    // Reset the ith path
    void
    reset(std::size_t const i)
    {
      at(i).reset();
    }
    // swap function
    void
    swap(HLTGlobalStatus& other)
    {
      paths_.swap(other.paths_);
    }
    // copy assignment implemented with swap()
    // Cannot ref-qualify assignment because of GCC_XML.
    HLTGlobalStatus&
    operator=(HLTGlobalStatus const& rhs)
    {
      HLTGlobalStatus temp(rhs);
      this->swap(temp);
      return *this;
    }

  private:
    // Global state variable calculated on the fly
    bool
    state_on_demand(std::size_t const icase) const
    {
      bool flags[3]{false, false, false};
      for (std::size_t i{}, n{size()}; i != n; ++i) {
        auto const s = state(i);
        if (s != hlt::Ready) {
          flags[0] = true; // at least one trigger was run
          if (s == hlt::Pass) {
            flags[1] = true; // at least one trigger accepted
          } else if (s == hlt::Exception) {
            flags[2] = true; // at least one trigger with error
          }
        }
      }
      // Change in semantics of flags[1] vs pre-art: now we accept if
      // no paths were run.
      flags[1] |= (!flags[0]) | paths_.empty();
      return flags[icase];
    }

  }; // HLTGlobalStatus

  // Free swap function
  inline void
  swap(HLTGlobalStatus& lhs, HLTGlobalStatus& rhs)
  {
    lhs.swap(rhs);
  }

  // Formatted printout of trigger table
  inline std::ostream&
  operator<<(std::ostream& ost, HLTGlobalStatus const& hlt)
  {
    std::vector<std::string> text(4);
    text[0] = "n";
    text[1] = "1";
    text[2] = "0";
    text[3] = "e";
    for (std::size_t i{}, n{hlt.size()}; i != n; ++i)
      ost << text.at(hlt.state(i));
    return ost;
  }

} // art

// ----------------------------------------------------------------------

// The standard allows us to specialize std::swap for non-templates.
// This ensures that HLTGlobalStatus::swap() will be used in algorithms.

namespace std {

  template <>
  inline void
  swap(art::HLTGlobalStatus& lhs, art::HLTGlobalStatus& rhs)
  {
    lhs.swap(rhs);
  }
}

  // ======================================================================

#endif /* canvas_Persistency_Common_HLTGlobalStatus_h */

// Local Variables:
// mode: c++
// End:
