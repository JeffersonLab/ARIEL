#ifndef canvas_Persistency_Provenance_TypeLabel_h
#define canvas_Persistency_Provenance_TypeLabel_h
// vim: set sw=2 expandtab :

//================================================================
// TypeLabel provides in-memory type information for use in
// constructing BranchDescription objects, etc.
//
// FIXME: A cleanup of the abstractions is required:
//   - Does the 'transient' data mamber belong here?  If so, what
//     values should it have?
//================================================================

#include "canvas/Utilities/TypeID.h"

#include <iosfwd>
#include <memory>
#include <string>

namespace art {

  class TypeLabel {
  public:
    TypeLabel(TypeID const& itemtype,
              std::string const& instanceName,
              bool const supportsView,
              bool const transient);

    TypeLabel(TypeID const& itemtype,
              std::string const& instanceName,
              bool const supportsView,
              std::string emulatedModule);

    auto const&
    typeID() const
    {
      return typeID_;
    }
    std::string
    className() const
    {
      return typeID_.className();
    }
    std::string
    friendlyClassName() const
    {
      return typeID_.friendlyClassName();
    }
    std::string const& emulatedModule() const;
    std::string const&
    productInstanceName() const
    {
      return productInstanceName_;
    }
    bool
    hasEmulatedModule() const
    {
      return static_cast<bool>(emulatedModule_);
    }
    bool
    supportsView() const
    {
      return supportsView_;
    }
    bool
    transient() const
    {
      return transient_;
    }

  private:
    TypeID typeID_;
    std::string productInstanceName_;
    bool supportsView_;
    bool transient_{false};
    std::shared_ptr<std::string> emulatedModule_{
      nullptr}; // shared so TypeLabel is copyable
    friend bool operator<(TypeLabel const& a, TypeLabel const& b);
    friend std::ostream& operator<<(std::ostream& os, TypeLabel const& tl);
  };

  bool operator<(TypeLabel const& a, TypeLabel const& b);
  std::ostream& operator<<(std::ostream& os, TypeLabel const& tl);

} // namespace art

#endif /* canvas_Persistency_Provenance_TypeLabel_h */

// Local Variables:
// mode: c++
// End:
