#ifndef canvas_Persistency_Provenance_Transient_h
#define canvas_Persistency_Provenance_Transient_h

/**\class Transient Transient.h DataFormats/Provenance/interface/Transient.h

   Description: ROOT safe bool

   Usage:
   We define a template for transients  in order to guarantee that value_
   is always reset when ever a new instance of this class is read from a file.
*/

// forward declarations
namespace art {

  template <typename T>
  class Transient {
  public:
    typedef T value_type;
    Transient(T value = T()) : value_(value) {}
    operator T() const { return value_; }
    Transient&
    operator=(T rh)
    {
      value_ = rh;
      return *this;
    }
    T const&
    get() const
    {
      return value_;
    }
    T&
    get()
    {
      return value_;
    }

  private:
    T value_;
  };
}
#endif /* canvas_Persistency_Provenance_Transient_h */

// Local Variables:
// mode: c++
// End:
