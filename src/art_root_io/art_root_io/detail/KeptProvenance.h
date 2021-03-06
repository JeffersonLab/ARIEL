#ifndef art_root_io_detail_KeptProvenance_h
#define art_root_io_detail_KeptProvenance_h
// vim: set sw=2 expandtab :

//
// Auxiliary class to handle provenance
// information...so I don't lose my mind in
// RootOutputFile::fillBranches.

#include "art_root_io/DropMetaData.h"
#include "canvas/Persistency/Provenance/ProductID.h"
#include "canvas/Persistency/Provenance/ProductProvenance.h"

#include <set>

namespace art {

  class Principal;

  namespace detail {

    class KeptProvenance {

    public:
      ~KeptProvenance();
      KeptProvenance(DropMetaData dropMetaData,
                     bool dropMetaDataForDroppedData,
                     std::set<ProductID>& branchesWithStoredHistory);

    public:
      ProductProvenance const& insert(ProductProvenance const&);
      ProductProvenance const& emplace(ProductID, ProductStatus);
      void setStatus(ProductProvenance const&, ProductStatus);

      auto
      begin() const
      {
        return provenance_.begin();
      }

      auto
      end() const
      {
        return provenance_.end();
      }

      void insertAncestors(ProductProvenance const& iGetParents,
                           Principal const& principal);

    private:
      DropMetaData const dropMetaData_;
      bool const dropMetaDataForDroppedData_;
      std::set<ProductID>& branchesWithStoredHistory_;
      std::set<ProductProvenance> provenance_{};
    };

  } // namespace detail

} // namespace art

#endif /* art_root_io_detail_KeptProvenance_h */

// Local variables:
// mode: c++
// End:
