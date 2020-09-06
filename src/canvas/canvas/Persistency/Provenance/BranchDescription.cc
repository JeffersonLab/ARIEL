#include "canvas/Persistency/Provenance/BranchDescription.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Provenance/canonicalProductName.h"
#include "canvas/Utilities/Exception.h"
#include "canvas/Utilities/FriendlyName.h"
#include "canvas/Utilities/WrappedClassName.h"
#include "fhiclcpp/ParameterSetID.h"

#include <cassert>
#include <cstdlib>
#include <ostream>
#include <sstream>
#include <utility>

using namespace std;

using fhicl::ParameterSetID;

namespace {

  [[noreturn]] void
  throwExceptionWithText(char const* txt)
  {
    throw art::Exception(art::errors::LogicError)
      << "Problem using an incomplete BranchDescription\n"
      << txt << "\nPlease report this error to the ART developers\n";
  }

} // unnamed namespace

namespace art {

  BranchDescription::BranchDescription(
    BranchType const bt,
    TypeLabel const& tl,
    std::string const& moduleLabel,
    ParameterSetID const& modulePSetID,
    ProcessConfiguration const& processConfig)
    : BranchDescription{bt,
                        tl.hasEmulatedModule() ? tl.emulatedModule() :
                                                 moduleLabel,
                        processConfig.processName(),
                        tl.className(),
                        tl.productInstanceName(),
                        modulePSetID,
                        processConfig.id(),
                        tl.hasEmulatedModule() ? Transients::PresentFromSource :
                                                 Transients::Produced,
                        tl.supportsView(),
                        tl.transient()}
  {}

  BranchDescription::BranchDescription(
    BranchType const branchType,
    std::string const& moduleLabel,
    std::string const& processName,
    std::string const& producedClassName,
    std::string const& productInstanceName,
    fhicl::ParameterSetID const& psetID,
    ProcessConfigurationID const& processConfigurationID,
    Transients::validity_state const validity,
    bool const supportsView,
    bool const transient)
    : branchType_{branchType}
    , moduleLabel_{moduleLabel}
    , processName_{processName}
    , producedClassName_{producedClassName}
    , friendlyClassName_{friendlyname::friendlyName(producedClassName_)}
    , productInstanceName_{productInstanceName}
    , supportsView_{supportsView}
  {
    guts().validity_ = validity;
    guts().transient_ = transient;
    psetIDs_.insert(psetID);
    processConfigurationIDs_.insert(processConfigurationID);
    throwIfInvalid_();
    fluffTransients_();
    initProductID_();
  }

  void
  BranchDescription::initProductID_()
  {
    if (!transientsFluffed_()) {
      return;
    }
    if (!productID_.isValid()) {
      productID_.setID(guts().branchName_);
    }
  }

  void
  BranchDescription::fluffTransients_() const
  {
    if (transientsFluffed_()) {
      return;
    }
    transients_.get().branchName_ = canonicalProductName(
      friendlyClassName(), moduleLabel(), productInstanceName(), processName());
    transients_.get().wrappedName_ = wrappedClassName(producedClassName_);
  }

  ParameterSetID const&
  BranchDescription::psetID() const
  {
    assert(!psetIDs().empty());
    if (psetIDs().size() != 1) {
      throw Exception(errors::Configuration, "AmbiguousProvenance")
        << "Your application requires all events on Branch '"
        << guts().branchName_
        << "'\n to have the same provenance. This file has events with "
           "mixed provenance\n"
        << "on this branch.  Use a different input file.\n";
    }
    return *psetIDs().begin();
  }

  void
  BranchDescription::merge(BranchDescription const& other)
  {
    psetIDs_.insert(other.psetIDs().begin(), other.psetIDs().end());
    processConfigurationIDs_.insert(other.processConfigurationIDs().begin(),
                                    other.processConfigurationIDs().end());
    if (guts().splitLevel_ == invalidSplitLevel) {
      guts().splitLevel_ = other.guts().splitLevel_;
    }
    if (guts().basketSize_ == invalidBasketSize) {
      guts().basketSize_ = other.guts().basketSize_;
    }
    // FIXME: This is probably wrong! We are going from defaulted compression
    //        to a possibly different compression, bad.
    if (guts().compression_ == invalidCompression) {
      guts().compression_ = other.guts().compression_;
    }
  }

  void
  BranchDescription::write(ostream& os) const
  {
    os << "Branch Type = " << branchType_ << endl;
    os << "Process Name = " << processName() << endl;
    os << "ModuleLabel = " << moduleLabel() << endl;
    os << "Product ID = " << productID() << '\n';
    os << "Class Name = " << producedClassName() << '\n';
    os << "Friendly Class Name = " << friendlyClassName() << '\n';
    os << "Product Instance Name = " << productInstanceName() << endl;
  }

  void
  BranchDescription::swap(BranchDescription& other)
  {
    using std::swap;
    swap(branchType_, other.branchType_);
    swap(moduleLabel_, other.moduleLabel_);
    swap(processName_, other.processName_);
    swap(productID_, other.productID_);
    swap(producedClassName_, other.producedClassName_);
    swap(friendlyClassName_, other.friendlyClassName_);
    swap(productInstanceName_, other.productInstanceName_);
    swap(supportsView_, other.supportsView_);
    swap(psetIDs_, other.psetIDs_);
    swap(processConfigurationIDs_, other.processConfigurationIDs_);
    swap(transients_, other.transients_);
  }

  // Note: throws
  void
  BranchDescription::throwIfInvalid_() const
  {
    constexpr char underscore('_');
    if (transientsFluffed_()) {
      return;
    }
    if (branchType_ >= NumBranchTypes) {
      throwExceptionWithText("Illegal BranchType detected");
    }
    if (moduleLabel_.empty()) {
      throwExceptionWithText("Module label is not allowed to be empty");
    }
    if (processName_.empty()) {
      throwExceptionWithText("Process name is not allowed to be empty");
    }
    if (producedClassName_.empty()) {
      throwExceptionWithText("Full class name is not allowed to be empty");
    }
    if (friendlyClassName_.empty()) {
      throwExceptionWithText("Friendly class name is not allowed to be empty");
    }
    if (friendlyClassName_.find(underscore) != string::npos) {
      throw Exception(errors::LogicError, "IllegalCharacter")
        << "Class name '" << friendlyClassName()
        << "' contains an underscore ('_'), which is illegal in the "
        << "name of a product.\n";
    }
    if (moduleLabel_.find(underscore) != string::npos) {
      throw Exception(errors::Configuration, "IllegalCharacter")
        << "Module label '" << moduleLabel()
        << "' contains an underscore ('_'), which is illegal in a "
        << "module label.\n";
    }
    if (productInstanceName_.find(underscore) != string::npos) {
      throw Exception(errors::Configuration, "IllegalCharacter")
        << "Product instance name '" << productInstanceName()
        << "' contains an underscore ('_'), which is illegal in a "
        << "product instance name.\n";
    }
    if (processName_.find(underscore) != string::npos) {
      throw Exception(errors::Configuration, "IllegalCharacter")
        << "Process name '" << processName()
        << "' contains an underscore ('_'), which is illegal in a "
        << "process name.\n";
    }
  }

  bool
  BranchDescription::transientsFluffed_() const noexcept
  {
    return !guts().branchName_.empty();
  }

  bool
  BranchDescription::isPsetIDUnique() const noexcept
  {
    return psetIDs().size() == 1;
  }

  set<ProcessConfigurationID> const&
  BranchDescription::processConfigurationIDs() const noexcept
  {
    return processConfigurationIDs_;
  }

  BranchDescription::Transients&
  BranchDescription::guts() noexcept
  {
    return transients_.get();
  }

  BranchDescription::Transients const&
  BranchDescription::guts() const noexcept
  {
    return transients_.get();
  }

  bool
  operator<(BranchDescription const& a, BranchDescription const& b)
  {
    if (a.processName() < b.processName()) {
      return true;
    }
    if (b.processName() < a.processName()) {
      return false;
    }
    if (a.producedClassName() < b.producedClassName()) {
      return true;
    }
    if (b.producedClassName() < a.producedClassName()) {
      return false;
    }
    if (a.friendlyClassName() < b.friendlyClassName()) {
      return true;
    }
    if (b.friendlyClassName() < a.friendlyClassName()) {
      return false;
    }
    if (a.productInstanceName() < b.productInstanceName()) {
      return true;
    }
    if (b.productInstanceName() < a.productInstanceName()) {
      return false;
    }
    if (a.moduleLabel() < b.moduleLabel()) {
      return true;
    }
    if (b.moduleLabel() < a.moduleLabel()) {
      return false;
    }
    if (a.branchType() < b.branchType()) {
      return true;
    }
    if (b.branchType() < a.branchType()) {
      return false;
    }
    if (a.productID() < b.productID()) {
      return true;
    }
    if (b.productID() < a.productID()) {
      return false;
    }
    if (a.psetIDs() < b.psetIDs()) {
      return true;
    }
    if (b.psetIDs() < a.psetIDs()) {
      return false;
    }
    if (a.processConfigurationIDs() < b.processConfigurationIDs()) {
      return true;
    }
    if (b.processConfigurationIDs() < a.processConfigurationIDs()) {
      return false;
    }
    return false;
  }

  bool
  combinable(BranchDescription const& a, BranchDescription const& b)
  {
    return (a.branchType() == b.branchType()) &&
           (a.processName() == b.processName()) &&
           (a.producedClassName() == b.producedClassName()) &&
           (a.friendlyClassName() == b.friendlyClassName()) &&
           (a.productInstanceName() == b.productInstanceName()) &&
           (a.moduleLabel() == b.moduleLabel()) &&
           (a.productID() == b.productID());
  }

  bool
  operator==(BranchDescription const& a, BranchDescription const& b)
  {
    return combinable(a, b) && (a.psetIDs() == b.psetIDs()) &&
           (a.processConfigurationIDs() == b.processConfigurationIDs());
  }

  std::ostream&
  operator<<(std::ostream& os, BranchDescription const& p)
  {
    p.write(os);
    return os;
  }

} // namespace art
