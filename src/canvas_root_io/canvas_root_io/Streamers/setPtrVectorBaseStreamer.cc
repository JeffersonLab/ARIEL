#include "canvas_root_io/Streamers/setPtrVectorBaseStreamer.h"
#include "canvas/Persistency/Common/PtrVectorBase.h"
#include "canvas/Utilities/TypeID.h"

#include "TBuffer.h"
#include "TClass.h"

#include <cassert>

TClassStreamer*
art::detail::PtrVectorBaseStreamer::Generate() const
{
  return new PtrVectorBaseStreamer{*this};
}

void
art::detail::PtrVectorBaseStreamer::operator()(TBuffer& R_b, void* objp)
{
  static TClassRef cl("art::PtrVectorBase");
  PtrVectorBase* obj = reinterpret_cast<PtrVectorBase*>(objp);
  if (R_b.IsReading()) {
    obj->zeroTransients(); // Clear transient rep.
    cl->ReadBuffer(R_b, objp);
    obj->fillPtrs(); // Fill transient rep.
  } else {
    obj->fill_offsets(obj->indicies_); // Fill persistent rep.
    cl->WriteBuffer(R_b, objp);
    PtrVectorBase::indices_t tmp;
    tmp.swap(obj->indicies_); // Clear, no longer needed.
  }
}

void
art::detail::setPtrVectorBaseStreamer()
{
  TClass* cl = TClass::GetClass(typeid(PtrVectorBase));
  if (cl->GetStreamer() == 0) {
    cl->AdoptStreamer(new PtrVectorBaseStreamer);
    auto const canSplit [[gnu::unused]] = cl->CanSplit();
    assert(!canSplit && "PtrVectorBase should not be splittable!");
  }
}
