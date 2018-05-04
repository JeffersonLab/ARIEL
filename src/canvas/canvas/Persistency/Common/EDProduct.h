#ifndef canvas_Persistency_Common_EDProduct_h
#define canvas_Persistency_Common_EDProduct_h

// ======================================================================
//
// EDProduct: The base class of each type that will be inserted into the
//            Event.
//
// ======================================================================

#include "canvas/Persistency/Common/fwd.h"

#include <memory>
#include <string>
#include <vector>

namespace art {
  class EDProduct;
}

// ======================================================================

class art::EDProduct {
public:
  virtual ~EDProduct() = default;

  bool
  isPresent() const
  {
    return isPresent_();
  }

  // We use vector<void*> so as to keep the type information out
  // of the EDProduct class.
  virtual void
  fillView(std::vector<void const*>&) const
  { /* should be called only polymorphically */
  }

  virtual std::type_info const*
  typeInfo() const
  {
    return typeInfo_();
  }

  void setPtr(std::type_info const& toType,
              unsigned long index,
              void const*& ptr) const;

  void getElementAddresses(std::type_info const& toType,
                           std::vector<unsigned long> const& indices,
                           std::vector<void const*>& ptr) const;

  virtual std::string
  productSize() const
  {
    return "-";
  }

  std::unique_ptr<EDProduct>
  makePartner(std::type_info const& wanted_type) const
  {
    return do_makePartner(wanted_type);
  }

  unsigned
  getRangeSetID() const
  {
    return do_getRangeSetID();
  }

  void
  setRangeSetID(unsigned const id)
  {
    do_setRangeSetID(id);
  }

  void
  combine(EDProduct* p)
  {
    do_combine(p);
  }

protected:
  virtual std::unique_ptr<EDProduct> do_makePartner(
    std::type_info const& wanted_type) const = 0;

  virtual void do_combine(EDProduct*) = 0;
  virtual void do_setRangeSetID(unsigned) = 0;
  virtual unsigned do_getRangeSetID() const = 0;

  virtual void do_setPtr(std::type_info const& toType,
                         unsigned long index,
                         void const*& ptr) const = 0;

  virtual void do_getElementAddresses(std::type_info const& toType,
                                      std::vector<unsigned long> const& indices,
                                      std::vector<void const*>& ptr) const = 0;

private:
  virtual bool isPresent_() const = 0;
  virtual std::type_info const* typeInfo_() const = 0;

}; // EDProduct

// ======================================================================

#endif /* canvas_Persistency_Common_EDProduct_h */

// Local Variables:
// mode: c++
// End:
