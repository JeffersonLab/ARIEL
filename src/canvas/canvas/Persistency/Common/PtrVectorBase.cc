#include "canvas/Persistency/Common/PtrVectorBase.h"

#include <utility>

void
art::PtrVectorBase::fillPtrs() const
{
  if (indicies_.size() == 0)
    return; // Empty or already done.
  fill_from_offsets(indicies_);

  using std::swap;
  indices_t tmp;
  swap(indicies_, tmp); // Zero -- finished with these.
}

void
art::PtrVectorBase::updateCore(RefCore const& productToBeInserted)
{
  if (productToBeInserted.isNull()) {
    throw art::Exception(errors::InvalidReference, "Inconsistency")
      << "art::PtrVectorBase::updateCore: Ptr has invalid (zero) product ID,\n"
         "so it cannot be added to a PtrVector. id should be ("
      << id() << ")\n";
  }
  if (isNull()) {
    core_ = productToBeInserted;
    return;
  }
  if (core_.id() != productToBeInserted.id()) {
    throw art::Exception(errors::InvalidReference, "Inconsistency")
      << "art::PtrVectorBase::updateCore: Ptr is inconsistent with\n"
         "PtrVector. id = ("
      << productToBeInserted.id() << "), should be (" << core_.id() << ")\n";
  }
  if (core_.productGetter() == nullptr &&
      productToBeInserted.productGetter() != nullptr) {
    core_.setProductGetter(productToBeInserted.productGetter());
  }
  if (core_.productPtr() == nullptr &&
      productToBeInserted.productPtr() != nullptr) {
    core_.setProductPtr(productToBeInserted.productPtr());
  }
}
