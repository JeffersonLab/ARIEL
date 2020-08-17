#ifndef canvas_Persistency_Provenance_BranchDescription_h
#define canvas_Persistency_Provenance_BranchDescription_h
// vim: set sw=2 expandtab :

// ================================================================================
// BranchDescription: The full description of a data-product branch.
// Equivalently,
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
// ================================================================================

#include "canvas/Persistency/Provenance/BranchType.h"
#include "canvas/Persistency/Provenance/ProcessConfiguration.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Persistency/Provenance/ProvenanceFwd.h"
#include "canvas/Persistency/Provenance/Transient.h"
#include "canvas/Persistency/Provenance/TypeLabel.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSetID.h"

#include <iosfwd>
#include <map>
#include <set>
#include <string>

namespace art {

  namespace detail {
    class BranchDescriptionStreamer;
  }

  class BranchDescription {

    friend bool combinable(BranchDescription const&, BranchDescription const&);
    friend bool operator<(BranchDescription const&, BranchDescription const&);
    friend bool operator==(BranchDescription const&, BranchDescription const&);

    friend class detail::BranchDescriptionStreamer;

  public: // TYPES
    static int constexpr invalidSplitLevel{-1};
    static int constexpr invalidBasketSize{0};
    static int constexpr invalidCompression{-1};

    struct Transients {

      Transients() = default;

      enum validity_state { Produced, PresentFromSource, Dropped, Invalid };

      // The branch name, which is currently derivable from the other
      // attributes.
      std::string branchName_{};

      // The wrapped class name, which is currently derivable from the
      // other attributes.
      std::string wrappedName_{};

      // Is the class of the branch marked as transient in the data
      // dictionary
      bool transient_{false};

      // Was this branch produced in this process rather than in a
      // previous process
      validity_state validity_{PresentFromSource};

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

    BranchDescription() = default;

    BranchDescription(BranchType const bt,
                      TypeLabel const& tl,
                      std::string const& moduleLabel,
                      fhicl::ParameterSetID const& modulePSetID,
                      ProcessConfiguration const& processConfig);

    BranchDescription(BranchType bt,
                      std::string const& moduleLabel,
                      std::string const& processName,
                      std::string const& producedClassName,
                      std::string const& productInstanceName,
                      fhicl::ParameterSetID const& psetID,
                      ProcessConfigurationID const& processConfigurationID,
                      Transients::validity_state validity,
                      bool supportsView,
                      bool transient);

    void write(std::ostream& os) const;

    std::string const&
    moduleLabel() const noexcept
    {
      return moduleLabel_;
    }
    std::string const&
    processName() const noexcept
    {
      return processName_;
    }
    std::string const&
    producedClassName() const noexcept
    {
      return producedClassName_;
    }
    std::string const&
    friendlyClassName() const noexcept
    {
      return friendlyClassName_;
    }
    std::string const&
    productInstanceName() const noexcept
    {
      return productInstanceName_;
    }
    InputTag
    inputTag() const
    {
      return InputTag{moduleLabel(), productInstanceName(), processName()};
    }

    bool
    produced() const noexcept
    {
      return guts().validity_ == Transients::Produced;
    }
    bool
    present() const noexcept
    {
      return guts().validity_ == Transients::PresentFromSource;
    }
    bool
    dropped() const noexcept
    {
      return guts().validity_ == Transients::Dropped;
    }
    bool
    transient() const noexcept
    {
      return guts().transient_;
    }

    int
    splitLevel() const noexcept
    {
      return guts().splitLevel_;
    }
    int
    basketSize() const noexcept
    {
      return guts().basketSize_;
    }
    int
    compression() const noexcept
    {
      return guts().compression_;
    }

    std::set<fhicl::ParameterSetID> const&
    psetIDs() const noexcept
    {
      return psetIDs_;
    }

    ProductID
    productID() const noexcept
    {
      return productID_;
    }
    BranchType
    branchType() const noexcept
    {
      return branchType_;
    }
    bool
    supportsView() const noexcept
    {
      return supportsView_;
    }
    std::string const&
    branchName() const noexcept
    {
      return guts().branchName_;
    }
    std::string const&
    wrappedName() const noexcept
    {
      return guts().wrappedName_;
    }

    void merge(BranchDescription const& other);
    void swap(BranchDescription& other);

    void
    setValidity(Transients::validity_state const state)
    {
      guts().validity_ = state;
    }

  private:
    fhicl::ParameterSetID const& psetID() const;

    void initProductID_();
    void fluffTransients_() const;
    bool transientsFluffed_() const noexcept;
    bool isPsetIDUnique() const noexcept;

    std::set<ProcessConfigurationID> const& processConfigurationIDs() const
      noexcept;

    Transients& guts() noexcept;
    Transients const& guts() const noexcept;

    void throwIfInvalid_() const;

    // What tree is the branch in?
    BranchType branchType_{InEvent};

    // A human-friendly string that uniquely identifies the EDProducer
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

    // The things we do not want saved to disk.
    mutable Transient<Transients> transients_{};
  };

  std::ostream& operator<<(std::ostream&, BranchDescription const&);

  bool operator<(BranchDescription const&, BranchDescription const&);

  bool operator==(BranchDescription const&, BranchDescription const&);

  bool combinable(BranchDescription const&, BranchDescription const&);

  using ProductDescriptions = std::vector<BranchDescription>;
  using ProductDescriptionsByID = std::map<ProductID, BranchDescription>;

} // namespace art

#endif /* canvas_Persistency_Provenance_BranchDescription_h */

// Local Variables:
// mode: c++
// End:
