#ifndef canvas_Persistency_Provenance_rootNames_h
#define canvas_Persistency_Provenance_rootNames_h

#include "canvas/Persistency/Provenance/BranchChildren.h"
#include "canvas/Persistency/Provenance/BranchType.h"
#include "canvas/Persistency/Provenance/Compatibility/BranchIDList.h"
#include "canvas/Persistency/Provenance/FileIndex.h"
#include "canvas/Persistency/Provenance/ParameterSetMap.h"
#include "canvas/Persistency/Provenance/ProcessHistory.h"
#include "canvas/Persistency/Provenance/ProductRegistry.h"
#include "canvas/Utilities/Exception.h"
#include "canvas/Utilities/TypeID.h"

#include <string>
#include <typeinfo>

namespace art {
  struct FileFormatVersion;
  class History;

  namespace rootNames {

    //------------------------------------------------------------------
    // Parentage Tree
    std::string const& parentageTreeName();

    // Branches on parentage tree
    std::string const& parentageIDBranchName();
    std::string const& parentageBranchName();

    //------------------------------------------------------------------
    // MetaData Tree (1 entry per file)
    std::string const& metaDataTreeName();

    // FileIndex Tree
    std::string const& fileIndexTreeName();

    // Event History Tree
    std::string const& eventHistoryTreeName();

    // Branches on EventHistory Tree
    std::string const& eventHistoryBranchName();

    //------------------------------------------------------------------
    // Other tree names
    std::string const& eventTreeName();
    std::string const& dataTreeName(BranchType bt);
    std::string const& eventMetaDataTreeName();

#define ART_ROOTNAME(T, N)                                                     \
  template <>                                                                  \
  inline char const* metaBranchRootName<T>()                                   \
  {                                                                            \
    return N;                                                                  \
  }

#define ART_ROOTNAME_SIMPLE(T) ART_ROOTNAME(T, #T)

    template <typename T>
    char const*
    metaBranchRootName()
    {
      throw Exception(errors::LogicError)
        << "art::metaBranchRootName requires a specialization for type "
        << TypeID(typeid(T)).className() << "\n";
    }

    ART_ROOTNAME_SIMPLE(FileFormatVersion)
    ART_ROOTNAME_SIMPLE(FileIndex) // supporting backwards compatibility
    ART_ROOTNAME(FileIndex::Element, "Element")
    ART_ROOTNAME_SIMPLE(ProductRegistry)
    ART_ROOTNAME_SIMPLE(ParameterSetMap)
    ART_ROOTNAME_SIMPLE(ProcessHistoryMap)
    ART_ROOTNAME_SIMPLE(BranchIDLists)
    ART_ROOTNAME(BranchChildren, "ProductDependencies")
    ART_ROOTNAME(History, "EventHistory")

#undef ART_ROOTNAME_SIMPLE
#undef ART_ROOTNAME

  } // rootNames
}
#endif /* canvas_Persistency_Provenance_rootNames_h */

// Local Variables:
// mode: c++
// End:
