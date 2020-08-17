#ifndef canvas_Persistency_Common_RNGsnapshot_h
#define canvas_Persistency_Common_RNGsnapshot_h
// vim: set sw=2 expandtab :

// ======================================================================
// RNGsnapshot is a data product holding saved state from/for the
// RandomNumberGenerator.
// ======================================================================

#include <limits>
#include <string>
#include <type_traits>
#include <vector>

namespace art {

  class RNGsnapshot {
  public:
    // --- CLHEP engine state characteristics:
    using CLHEP_t = unsigned long;
    using engine_state_t = std::vector<CLHEP_t>;

    // --- Our state characteristics:
    using saved_t = unsigned int;
    using snapshot_state_t = std::vector<saved_t>;
    using label_t = std::string;

    // CLHEP specifies that an engine's state will be returned as an
    // object of type vector<unsigned long>.  These static asserts
    // ensure that (a) the schema we use is portable between
    // platforms, and (b) the schema is large enough to hold the
    // necessary data, but not too large to allow data loss when
    // converting from unsigned int (our type) to unsigned long
    // (CLHEP's type).
    static_assert(std::numeric_limits<saved_t>::digits == 32,
                  "std::numeric_limits<saved_t>::digits != 32");
    static_assert(sizeof(saved_t) <= sizeof(CLHEP_t),
                  "sizeof(saved_t) > sizeof(CLHEP_t)");

    RNGsnapshot() = default;
    explicit RNGsnapshot(std::string const& ekind,
                         label_t const& label,
                         engine_state_t const& est);

    // --- Access:
    std::string const&
    ekind() const
    {
      return engine_kind_;
    }
    label_t const&
    label() const
    {
      return label_;
    }
    snapshot_state_t const&
    state() const
    {
      return state_;
    }

    // --- Save/restore:
    void saveFrom(std::string const&, label_t const&, engine_state_t const&);
    engine_state_t restoreState() const;

  private:
    std::string engine_kind_{};
    label_t label_{};
    snapshot_state_t state_{};

  }; // RNGsnapshot

} // art

#endif /* canvas_Persistency_Common_RNGsnapshot_h */

// Local Variables:
// mode: c++
// End:
