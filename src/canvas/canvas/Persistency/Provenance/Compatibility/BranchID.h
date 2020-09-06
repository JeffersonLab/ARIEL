#ifndef canvas_Persistency_Provenance_Compatibility_BranchID_h
#define canvas_Persistency_Provenance_Compatibility_BranchID_h

/*----------------------------------------------------------------------

BranchID: A unique identifier for each branch.

----------------------------------------------------------------------*/

#include <cstdlib>
#include <iosfwd>
#include <string>

namespace art {

  class BranchID {
  public:
    typedef unsigned int value_type;

    BranchID() = default;
    explicit BranchID(std::string const& branchName)
      : BranchID{toID(branchName)}
    {}
    explicit BranchID(value_type const id) : id_{id} {}

    void
    setID(std::string const& branchName)
    {
      id_ = toID(branchName);
    }
    unsigned int
    id() const
    {
      return id_;
    }
    bool
    isValid() const
    {
      return id_ != 0;
    }

    // This is for the use of tbb::hash<BranchID> (and possibly
    // std::hash<BranchID>?)
    explicit operator std::size_t() const
    {
      return static_cast<std::size_t>(id_);
    }

    bool
    operator<(BranchID const& rh) const
    {
      return id_ < rh.id_;
    }
    bool
    operator>(BranchID const& rh) const
    {
      return id_ > rh.id_;
    }
    bool
    operator==(BranchID const& rh) const
    {
      return id_ == rh.id_;
    }
    bool
    operator!=(BranchID const& rh) const
    {
      return id_ != rh.id_;
    }

    struct Hash {
      std::size_t
      operator()(BranchID const& bid) const
      {
        return bid.id(); // since the ID is already a checksum, don't
                         // worry about further hashing
      }
    };

  private:
    static value_type toID(std::string const& branchName);
    value_type id_{};
  };

  std::ostream& operator<<(std::ostream& os, BranchID const& id);
}
#endif /* canvas_Persistency_Provenance_Compatibility_BranchID_h */

// Local Variables:
// mode: c++
// End:
