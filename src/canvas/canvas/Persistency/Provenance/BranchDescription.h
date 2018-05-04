#ifndef canvas_Persistency_Provenance_BranchDescription_h
#define canvas_Persistency_Provenance_BranchDescription_h

// ========================================================================
// BranchDescription: The full description of a Branch.  Equivalently,
//                    the event-independent description of an EDProduct.
// This description also applies to every product instance on the branch.
//
// FIXME: A better design would be:
//
//   BranchDescription --owns--> ProductDescription --owns--> TypeDescription
//
// The BranchDescription class is what retains information necessary for
// interactions with ROOT.  The ProductDescription contains information
// that is relevant for core framework processing.
// ========================================================================

#include "canvas/Persistency/Provenance/BranchID.h"
#include "canvas/Persistency/Provenance/BranchType.h"
#include "canvas/Persistency/Provenance/ProcessConfigurationID.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Persistency/Provenance/ProvenanceFwd.h"
#include "canvas/Persistency/Provenance/Transient.h"
#include "canvas/Persistency/Provenance/TypeLabel.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSetID.h"

#include <iosfwd>
#include <set>
#include <string>

// ======================================================================

namespace art {

  class BranchDescription;

  std::ostream& operator<<(std::ostream& os, BranchDescription const& p);

  bool operator<(BranchDescription const& a, BranchDescription const& b);
  bool operator==(BranchDescription const& a, BranchDescription const& b);

  bool combinable(BranchDescription const& a, BranchDescription const& b);

  namespace detail {
    class BranchDescriptionStreamer;
  }
}

// ----------------------------------------------------------------------

class art::BranchDescription {
public:
  static int constexpr invalidSplitLevel{-1};
  static int constexpr invalidBasketSize{0};
  static int constexpr invalidCompression{-1};

  BranchDescription() = default;

  BranchDescription(BranchType const bt,
                    TypeLabel const& tl,
                    ModuleDescription const& modDesc);

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  void write(std::ostream& os) const;

  std::string const&
  moduleLabel() const
  {
    return moduleLabel_;
  }
  std::string const&
  processName() const
  {
    return processName_;
  }
  std::string const&
  producedClassName() const
  {
    return producedClassName_;
  }
  std::string const&
  friendlyClassName() const
  {
    return friendlyClassName_;
  }
  std::string const&
  productInstanceName() const
  {
    return productInstanceName_;
  }

  InputTag
  inputTag() const
  {
    return InputTag{moduleLabel(), productInstanceName(), processName()};
  }

  bool
  produced() const
  {
    return guts().validity_ == Transients::Produced;
  }
  bool
  present() const
  {
    return guts().validity_ == Transients::PresentFromSource;
  }
  bool
  dropped() const
  {
    return guts().validity_ == Transients::Dropped;
  }
  bool
  transient() const
  {
    return guts().transient_;
  }

  int
  splitLevel() const
  {
    return guts().splitLevel_;
  }
  int
  basketSize() const
  {
    return guts().basketSize_;
  }
  int
  compression() const
  {
    return guts().compression_;
  }

  std::set<fhicl::ParameterSetID> const&
  psetIDs() const
  {
    return psetIDs_;
  }

  ProductID
  productID() const
  {
    return productID_;
  }
  BranchType
  branchType() const
  {
    return branchType_;
  }
  bool
  supportsView() const
  {
    return supportsView_;
  }
  std::string const&
  branchName() const
  {
    return guts().branchName_;
  }
  std::string const&
  wrappedName() const
  {
    return guts().wrappedName_;
  }

  void merge(BranchDescription const& other);
  void swap(BranchDescription& other);

  friend bool combinable(BranchDescription const&, BranchDescription const&);
  friend bool operator<(BranchDescription const&, BranchDescription const&);
  friend bool operator==(BranchDescription const&, BranchDescription const&);

  struct Transients {
    Transients() = default;

    enum validity_state { Produced, PresentFromSource, Dropped, Invalid };

    // The branch name, which is currently derivable from the other
    // attributes.
    std::string branchName_{};

    // The wrapped class name, which is currently derivable from the
    // other attributes.
    std::string wrappedName_{};

    // Was this branch produced in this process rather than in a
    // previous process
    validity_state validity_{PresentFromSource};

    // Is the class of the branch marked as transient in the data
    // dictionary
    bool transient_{false};

    // N.B. ROOT-specific transient information will be fluffed by the
    //      BranchDescriptionStreamer::fluffRootTransients function.

    // The split level of the branch, as marked in the data
    // dictionary.
    int splitLevel_{};

    // The basket size of the branch, as marked in the data
    // dictionary.
    int basketSize_{};

    // The compression of the branch, as marked in the data
    // dictionary.
    int compression_{invalidCompression};
  };

  void
  setValidity(Transients::validity_state const state)
  {
    guts().validity_ = state;
  }

private:
  friend class detail::BranchDescriptionStreamer;

  bool
  transientsFluffed_() const
  {
    return !guts().branchName_.empty();
  }
  void initProductID_();
  void fluffTransients_() const;

  fhicl::ParameterSetID const& psetID() const;
  bool
  isPsetIDUnique() const
  {
    return psetIDs().size() == 1;
  }
  std::set<ProcessConfigurationID> const&
  processConfigurationIDs() const
  {
    return processConfigurationIDs_;
  }

  Transients&
  guts()
  {
    return transients_.get();
  }
  Transients const&
  guts() const
  {
    return transients_.get();
  }

  void throwIfInvalid_() const;

  // What tree is the branch in?
  BranchType branchType_{InEvent};

  // A human friendly string that uniquely identifies the EDProducer
  // and becomes part of the identity of a product that it produces
  std::string moduleLabel_{};

  // the physical process that this program was part of (e.g. production)
  std::string processName_{};

  // An ID uniquely identifying the product
  ProductID productID_{};

  // the full name of the type of product this is
  std::string producedClassName_{};

  // a readable name of the type of product this is
  std::string friendlyClassName_{};

  // a user-supplied name to distinguish multiple products of the same type
  // that are produced by the same producer
  std::string productInstanceName_{};

  // Does this product support the concept of a view?
  bool supportsView_{false};

  // ID's of parameter set of the creators of products
  // on this branch
  std::set<fhicl::ParameterSetID> psetIDs_{};

  // ID's of process configurations for products
  // on this branch
  std::set<ProcessConfigurationID> processConfigurationIDs_{};

  mutable Transient<Transients> transients_{};
}; // BranchDescription

namespace art {
  using ProductDescriptions = std::vector<BranchDescription>;
}

#endif /* canvas_Persistency_Provenance_BranchDescription_h */

// Local Variables:
// mode: c++
// End:
