#ifndef canvas_root_io_Streamers_AssnsStreamer_h
#define canvas_root_io_Streamers_AssnsStreamer_h

// =====================================================================
// AssnsStreamer: Temporary streamer class until ioread rules are
//                working again.
// =====================================================================

#include "canvas/Persistency/Common/AssnsBase.h"
#include "canvas/Utilities/TypeID.h"

#include "TBuffer.h"
#include "TClass.h"
#include "TClassRef.h"
#include "TClassStreamer.h"

namespace art {
  namespace detail {

    class AssnsStreamer : public TClassStreamer {
      std::string className_;

    public:
      explicit AssnsStreamer(std::string const& className)
        : className_{className}
      {}

      static void init_streamer(std::string className);

      TClassStreamer*
      Generate() const override
      {
        return new AssnsStreamer{*this};
      }

      void
      operator()(TBuffer& R_b, void* objp) override
      {
        TClassRef cl{TClass::GetClass(className_.c_str())};
        auto obj = reinterpret_cast<detail::AssnsBase*>(objp);
        if (R_b.IsReading()) {
          cl->ReadBuffer(R_b, objp);
          obj->fill_transients();
        } else {
          obj->fill_from_transients();
          cl->WriteBuffer(R_b, objp);
        }
      }
    };

    inline void
    AssnsStreamer::init_streamer(std::string className)
    {
      // The streamer affects only the members of the Assns<A,B,void>
      // base class.
      auto maybe_base_name = name_of_assns_base(className);
      auto base_name = maybe_base_name.empty() ? className : maybe_base_name;
      TClassRef cl{TClass::GetClass(base_name.c_str())};
      if (cl->GetStreamer() == nullptr) {
        cl->AdoptStreamer(new detail::AssnsStreamer{base_name});
      }
    }
  }
}
#endif /* canvas_root_io_Streamers_AssnsStreamer_h */

// Local Variables:
// mode: c++
// End:
