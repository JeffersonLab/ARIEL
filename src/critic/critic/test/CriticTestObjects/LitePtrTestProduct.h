#ifndef critic_test_CriticTestObjects_LitePtrTestProduct_h
#define critic_test_CriticTestObjects_LitePtrTestProduct_h

#include "art/test/TestObjects/ToyProducts.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"

namespace critictest {

  class LitePtrTestProduct {
  public:
    LitePtrTestProduct();

    art::Ptr<int> ptrInt1;
    art::Ptr<int> ptrInt2;
    art::Ptr<int> ptrInt3;

    art::Ptr<arttest::Simple> ptrSimple1;
    art::Ptr<arttest::Simple> ptrSimple2;
    art::Ptr<arttest::Simple> ptrSimple3;

    art::Ptr<arttest::SimpleDerived> ptrSimpleDerived1;
    art::Ptr<arttest::SimpleDerived> ptrSimpleDerived2;
    art::Ptr<arttest::SimpleDerived> ptrSimpleDerived3;

    art::PtrVector<int> ptrVectorInt;
    art::PtrVector<arttest::Simple> ptrVectorSimple;
    art::PtrVector<arttest::SimpleDerived> ptrVectorSimpleDerived;

    art::Ptr<int> ptrIntoContainerToBeDropped;
    art::Ptr<int> nullPtr;
    art::Ptr<int> nullDroppedPtr;
    art::Ptr<int> invalidPtr;
  };
} // namespace critictest

#endif /* critic_test_CriticTestObjects_LitePtrTestProduct_h */

// Local Variables:
// mode: c++
// End:
