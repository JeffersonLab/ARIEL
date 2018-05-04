#include "canvas/Persistency/Provenance/TypeLabel.h"
#include "canvas/Utilities/Exception.h"
// vim: set sw=2 expandtab :

#include <ostream>
#include <tuple>

namespace art {

  TypeLabel::TypeLabel(TypeID const& itemtype,
                       std::string const& instanceName,
                       bool const supportsView,
                       bool const transient)
    : typeID_{itemtype}
    , productInstanceName_{instanceName}
    , supportsView_{supportsView}
    , transient_{transient}
  {}

  TypeLabel::TypeLabel(TypeID const& itemtype,
                       std::string const& instanceName,
                       bool const supportsView,
                       std::string emulatedModule)
    : typeID_{itemtype}
    , productInstanceName_{instanceName}
    , supportsView_{supportsView}
    , emulatedModule_{std::make_unique<std::string>(std::move(emulatedModule))}
  {}

  std::string const&
  TypeLabel::emulatedModule() const
  {
    if (!emulatedModule_) {
      throw art::Exception(art::errors::NullPointerError,
                           "TypeLabel::emulatedModule\n")
        << "Attempt to retrieve an emulated module name when one does not "
           "exist.\n";
    }
    return *emulatedModule_;
  }

  // Types with the same friendlyClassName are in the same equivalence
  // class for the purposes of this comparison.
  bool
  operator<(TypeLabel const& a, TypeLabel const& b)
  {
    auto const& a_class_name = a.className();
    auto const& b_class_name = b.className();
    auto const& a_emulated_module =
      a.hasEmulatedModule() ? *a.emulatedModule_ : "<none>";
    auto const& b_emulated_module =
      b.hasEmulatedModule() ? *b.emulatedModule_ : "<none>";
    return std::tie(a_emulated_module, a.productInstanceName_, a_class_name) <
           std::tie(b_emulated_module, b.productInstanceName_, b_class_name);
  }

  std::ostream&
  operator<<(std::ostream& os, TypeLabel const& tl)
  {
    os << "Emulated module:       '"
       << (tl.hasEmulatedModule() ? tl.emulatedModule() : "<none>") << "'\n"
       << "Product instance name: '" << tl.productInstanceName() << "'\n"
       << "Class name:            '" << tl.className() << "'\n"
       << "Supports views:        '" << std::boolalpha << tl.supportsView()
       << '\n'
       << "Transient:             '" << tl.transient();
    return os;
  }
}
