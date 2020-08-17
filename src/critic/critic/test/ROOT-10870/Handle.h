#ifndef r10870_Handle_h
#define r10870_Handle_h

#include "ProductID.h"
#include "Exception.h"

#include <memory>

namespace r10870 {

  template <typename T>
  class Handle {
  public:
    using element_type = T;
    class HandleTag {};

    Handle() = default;
    explicit Handle(T const*, r10870::ProductID);
    explicit Handle(std::shared_ptr<r10870::Exception const>);
    Handle(Handle const&) = default;
    Handle& operator=(Handle const&) = default;

    // pointer behaviors
    T const& operator*() const noexcept(false);
    T const* operator->() const noexcept(false); // alias for product()
    T const* product() const noexcept(false);

    // inspectors
    bool isValid() const noexcept;
    r10870::ProductID id() const noexcept;
    std::shared_ptr<r10870::Exception const> whyFailed() const noexcept;

  private:
    [[noreturn]] void throwHandleWhyFailed_() const noexcept(false);

    T const* prod_{nullptr};
    r10870::ProductID productID_{r10870::ProductID::invalid()};
    std::shared_ptr<r10870::Exception const> whyFailed_;
  };

  template <class T>
  Handle<T>::Handle(T const* prod, r10870::ProductID const productID)
    : prod_{prod}, productID_{productID}
  {}

  template <class T>
  Handle<T>::Handle(std::shared_ptr<r10870::Exception const> iWhyFailed)
    : whyFailed_{iWhyFailed}
  {}

  template <class T>
  inline T const& Handle<T>::operator*() const noexcept(false)
  {
    return *product();
  }

  template <class T>
  inline T const* Handle<T>::operator->() const noexcept(false)
  {
    return product();
  }

  template <class T>
  inline T const*
  Handle<T>::product() const noexcept(false)
  {
    if (!prod_)
      throwHandleWhyFailed_();
    return prod_;
  }

  template <class T>
  bool
  Handle<T>::isValid() const noexcept
  {
    return prod_ != nullptr;
  }

  template <class T>
  r10870::ProductID
  Handle<T>::id() const noexcept
  {
    return productID_;
  }

  template <class T>
  inline std::shared_ptr<r10870::Exception const>
  Handle<T>::whyFailed() const noexcept
  {
    return whyFailed_;
  }

  template <class T>
  void
  Handle<T>::throwHandleWhyFailed_() const noexcept(false)
  {
    if (whyFailed_) {
      throw *whyFailed_;
    }
    throw r10870::Exception(r10870::errors::LogicError)
      << "Attempt to dereference invalid Handle with no stored exception\n"
      << "Maybe you forgot to call getByLabel before dereferencing\n";
  }
} // namespace r10870
#endif /* r10870_Handle_h */

// Local Variables:
// mode: c++
// End:
