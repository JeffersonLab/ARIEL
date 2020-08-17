// vim: set sw=2 expandtab :
#include "canvas_root_io/Streamers/RefCoreStreamer.h"
#include "canvas/Persistency/Common/PrincipalBase.h"
#include "canvas/Persistency/Common/RefCore.h"
#include "canvas/Persistency/Provenance/ProductID.h"

#include "TBuffer.h"
#include "TClassRef.h"

namespace art {

  RefCoreStreamer::RefCoreStreamer(PrincipalBase const* principal /*=nullptr*/)
  {
    // Must do this way, thread sanitizer does not handle
    // ctor initializer properly.
    principal_ = principal;
  }

  void
  RefCoreStreamer::setPrincipal(PrincipalBase const* principal)
  {
    principal_ = principal;
  }

  TClassStreamer*
  RefCoreStreamer::Generate() const
  {
    return new RefCoreStreamer{principal_.load()};
  }

  void
  RefCoreStreamer::operator()(TBuffer& buf, void* objp)
  {
    static TClassRef cl("art::RefCore");
    if (buf.IsReading()) {
      cl->ReadBuffer(buf, objp);
      auto obj = static_cast<RefCore*>(objp);
      // Note: We do not attempt to fluff an empty RefCore.
      if (principal_ && obj->id().isValid()) {
        // In the case of art, get the Group.
        // In the case of gallery, get the BranchData or AssnsBranchData.
        auto edProductGetter = principal_.load()->getEDProductGetter(obj->id());
        obj->setProductGetter(edProductGetter);
      } else {
        obj->setProductGetter(nullptr);
      }
      obj->setProductPtr(nullptr);
    } else {
      cl->WriteBuffer(buf, objp);
    }
  }

  void
  configureRefCoreStreamer(PrincipalBase const* principal)
  {
    static TClassRef cl("art::RefCore");
    RefCoreStreamer* st = static_cast<RefCoreStreamer*>(cl->GetStreamer());
    if (st == nullptr) {
      cl->AdoptStreamer(new RefCoreStreamer(principal));
    } else {
      st->setPrincipal(principal);
    }
  }

} // namespace art
