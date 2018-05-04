#ifndef canvas_root_io_Streamers_TransientStreamer_h
#define canvas_root_io_Streamers_TransientStreamer_h
// vim: set sw=2 expandtab :

//
//  We use this custom streamer to make sure that if
//  a ROOT I/O buffer is reused for reading that the
//  data member Transient<T>::value_ gets reinitialized
//  with a default-constructed T.
//

#include "TBuffer.h"
#include "TClassRef.h"
#include "TClassStreamer.h"
#include "canvas/Utilities/TypeID.h"
#include <string>

namespace art {

  void setProvenanceTransientStreamers();

  template <typename T>
  class TransientStreamer : public TClassStreamer {

  public: // TYPES
    typedef T element_type;

  public: // MEMBER FUNCTIONS
    TransientStreamer();

  public: // MEMBER FUNCTIONS
    virtual TClassStreamer* Generate() const override;

    void operator()(TBuffer& R_b, void* objp) override;

  private: // MEMBER DATA
    std::string className_;

    TClassRef cl_;
  };

  template <typename T>
  TransientStreamer<T>::TransientStreamer()
    : className_(TypeID(typeid(T)).className()), cl_(className_.c_str())
  {}

  template <typename T>
  TClassStreamer*
  TransientStreamer<T>::Generate() const
  {
    return new TransientStreamer<T>{*this};
  }

  template <typename T>
  void
  TransientStreamer<T>::operator()(TBuffer& R_b, void* objp)
  {
    if (R_b.IsReading()) {
      cl_->ReadBuffer(R_b, objp);
      T* obj = static_cast<T*>(objp);
      *obj = T();
    } else {
      cl_->WriteBuffer(R_b, objp);
    }
  }

  namespace detail {

    template <typename T>
    void
    SetTransientStreamer()
    {
      TClass* cl = TClass::GetClass(typeid(T));
      // FIXME: We need to test for cl == nullptr here!
      if (cl->GetStreamer() == nullptr) {
        cl->AdoptStreamer(new TransientStreamer<T>());
      }
    }

    template <typename T>
    void
    SetTransientStreamer(T const&)
    {
      TClass* cl = TClass::GetClass(typeid(T));
      // FIXME: We need to test for cl == nullptr here!
      if (cl->GetStreamer() == nullptr) {
        cl->AdoptStreamer(new TransientStreamer<T>());
      }
    }

  } // namespace detail

} // namespace art

#endif /* canvas_root_io_Streamers_TransientStreamer_h */

// Local Variables:
// mode: c++
// End:
