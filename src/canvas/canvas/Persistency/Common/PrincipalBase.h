#ifndef canvas_Persistency_Common_PrincipalBase_h
#define canvas_Persistency_Common_PrincipalBase_h
// vim: set sw=2 expandtab :

// The RefCoreStreamer uses the interface defined here to find the
// EDProductGetter corresponding to the current event and the
// ProductID. A pointer to the EDProductGetter is then written into
// the RefCore and will get used the first time the Ptr containing the
// RefCore is dereferenced.  This base class breaks the dependence of
// the RefCoreStreamer on the classes that derive from this class. In
// the full framework this will be a base class for the
// art::EventPrincipal and in gallery this will be a base class for
// the gallery::Event.

namespace art {

  class EDProductGetter;
  class ProductID;

  class PrincipalBase {

  public:
    virtual ~PrincipalBase() = 0;

    PrincipalBase();

    // Note: Used only by canvas RefCoreStreamer.cc
    EDProductGetter const* getEDProductGetter(ProductID const& pid) const;

  private:
    // Note: Used only by canvas RefCoreStreamer.cc through
    // PrincipalBase::getEDProductGetter!
    virtual EDProductGetter const* getEDProductGetter_(
      ProductID const&) const = 0;
  };

} // namespace art

#endif /* canvas_Persistency_Common_PrincipalBase_h */

// Local Variables:
// mode: c++
// End:
