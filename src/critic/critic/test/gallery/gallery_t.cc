#include "art/test/TestObjects/ToyProducts.h"
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/FindOne.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/TriggerResults.h"
#include "canvas/Persistency/Provenance/EventAuxiliary.h"
#include "canvas/Utilities/TypeID.h"
#include "critic/test/CriticTestObjects/LiteAssnTestData.h"
#include "critic/test/CriticTestObjects/LitePtrTestProduct.h"
#include "gallery/Event.h"
#include "gallery/TypeLabelInstanceKey.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace std::string_literals;

// Here is a summary of what was written into the input files. It is
// all arbitrary for test purposes. Here we are just testing that we
// have read what was written.
//
// 1 PROD1 runs modules m1 thru m6 writing all products 5 events
// 2 PROD2 same except m4 not run and m6 does not put its products, input from
//         PROD1
// 3 PROD3 same except m3, m4, and m6 not run, m5 not put, m7 added
//         but not put and also ptr1 run, input from PROD2
// 4 PROD3 same as PROD3 except m7 and ptr1 write products also
// 5 MERGE merges the output of PROD3 and PROD4 and drops one of the Ptrs in
//         the LitePtrTestProduct
// 6 PROD2EXTRA input from 2, runs m7, adds an extra process in its
//         output file
// 7 PROD3 reads input from 6, similar to 3 but adds m0 and does not
//         have m7 the purpose of 6 and 7 is to produce a second input
//         file to test multifile input with different BranchIDLists
//         and ProcessHistory

int
main()
{
  art::InputTag const inputTagTriggerResults{"TriggerResults", "", "PROD1"};
  art::InputTag const inputTagIncorrect{
    "IncorrectLabel", "IncorrectInstance", "PROD1"};
  art::InputTag const inputTagEventID{"m1", "eventID", "PROD1"};

  art::InputTag const inputTag111{"m1::PROD1"s};
  art::InputTag const inputTag121{"m1:i2:PROD1"};
  art::InputTag const inputTag131{"m1"s, "i3"s, "PROD1"s};

  art::InputTag const inputTags111{"m1", "", "PROD1"};
  art::InputTag const inputTags121{"m1", "i2", "PROD1"};
  art::InputTag const inputTags131{"m1", "i3", "PROD1"};

  art::InputTag const inputTag211{"m2", "", "PROD1"};
  art::InputTag const inputTag221{"m2", "i2", "PROD1"};

  art::InputTag const inputTag112{"m1", "", "PROD2"};
  art::InputTag const inputTag122{"m1", "i2", "PROD2"};

  art::InputTag const inputTag113{"m1", "", "PROD3"};
  art::InputTag const inputTag123{"m1", "i2", "PROD3"};

  art::InputTag const inputTag11{"m1"};
  art::InputTag const inputTag12{"m1:i2"};

  art::InputTag const inputTags11{"m1", ""};
  art::InputTag const inputTags12{"m1", "i2"};

  art::InputTag const inputTag21{"m2", ""};
  art::InputTag const inputTag22{"m2", "i2"};

  art::InputTag const inputTag31{"m3", ""};
  art::InputTag const inputTag32{"m3", "i2"};

  art::InputTag const inputTag312{"m3", "", "PROD2"};
  art::InputTag const inputTag322{"m3", "i2", "PROD2"};

  art::InputTag const inputTag41{"m4", ""};
  art::InputTag const inputTag42{"m4", "i2"};

  art::InputTag const inputTag411{"m4", "", "PROD1"};
  art::InputTag const inputTag421{"m4", "i2", "PROD1"};

  art::InputTag const inputTag51{"m5", ""};
  art::InputTag const inputTag52{"m5", "i2"};

  art::InputTag const inputTags61{"m6", ""};
  art::InputTag const inputTags62{"m6", "i2"};

  art::InputTag const inputTagFile2{"m0::PROD3"s};

  art::InputTag const inputTagPtrTest{"ptr1"};

  art::InputTag const inputTagAssnTest1{"ptr1"};
  art::InputTag const inputTagAssnTest2{"ptr1::PROD3"};

  // Test InputTag
  assert(inputTag121 != inputTags131);
  assert(inputTag121 != inputTag122);
  assert(inputTag122 != inputTag322);
  art::InputTag const inputTagTest{"m3", "i2", "PROD2"};
  assert(!(inputTag322 != inputTagTest));
  std::cout << inputTagTest << std::endl;

  // Test TypeLabelInstance class
  art::TypeID const typeIntProduct{typeid(arttest::IntProduct)};
  art::TypeID const typeStringProduct{typeid(arttest::StringProduct)};
  gallery::TypeLabelInstanceKey const key1{typeIntProduct, "a", "b"};
  assert(key1.typeID() == typeIntProduct && std::string{key1.label()} == "a" &&
         std::string(key1.instance()) == "b");
  gallery::TypeLabelInstanceKey const key2{typeStringProduct, "a", "b"};
  gallery::TypeLabelInstanceKey const key3{typeIntProduct, "c", "b"};
  gallery::TypeLabelInstanceKey const key4{typeIntProduct, "a", "c"};
  assert(key1 < key2 || key2 < key1);
  assert(key1 < key3 && key1 < key4);

  // Read values from a ROOT file and test that we get the correct
  // products with the arbitrary values we know were put into them.
  std::vector<std::string> const filenames{
    "../gallery_makeInput8.d/test_gallery8.root",
    "../gallery_makeInput5.d/test_gallery5.root",
    "../gallery_makeInput8.d/test_gallery8.root",
    "../gallery_makeInput7.d/test_gallery7.root",
    "../gallery_makeInput7.d/test_gallery7.root",
    "../gallery_makeInput8.d/test_gallery8.root"};

  gallery::Event ev(filenames, true, 1);
  assert(ev.numberOfEventsInFile() == 10);

  unsigned int iEvent = 1;
  unsigned int counter = 0;
  for (; !ev.atEnd(); ev.next(), ++iEvent, ++counter) {

    assert(ev.isValid());
    // First non-empty file has 10 events, other non-empty file has 5 events.
    assert(ev.eventEntry() == (counter < 10 ? counter % 10 : counter % 5));

    // Check that we're on the correct file.
    if (counter < 10) {
      assert(ev.fileEntry() == 1);
    } else if (counter < 15) {
      assert(ev.fileEntry() == 3);
    } else {
      assert(ev.fileEntry() == 4);
    }

    // Check event numbers.
    art::EventAuxiliary const& aux = ev.eventAuxiliary();
    std::cout << aux.id() << "\n";
    if (iEvent == 6)
      iEvent = 1; // Events numbers go from 1 to 5 and repeat 1 to 5
    if (aux.id().event() != iEvent) {
      std::cout << "Unexpected event number read from EventAuxiliary.\n";
      return 1;
    }

    std::cout << "ProcessHistoryID: " << ev.processHistoryID() << std::endl;

    gallery::Handle<art::TriggerResults> triggerResultsHandle;
    assert(!triggerResultsHandle.isValid());
    assert(!triggerResultsHandle.whyFailed());

    assert(ev.getByLabel(inputTagTriggerResults, triggerResultsHandle));
    assert(triggerResultsHandle.isValid());
    assert(!triggerResultsHandle.whyFailed());
    std::string const test1 =
      triggerResultsHandle->parameterSetID().to_string();
    std::string const test2 =
      (*triggerResultsHandle).parameterSetID().to_string();
    assert(!ev.getByLabel(inputTagIncorrect, triggerResultsHandle));
    assert(!triggerResultsHandle.isValid());
    assert(triggerResultsHandle.whyFailed());
    bool exceptionWasThrown{true};
    try {
      triggerResultsHandle->parameterSetID().to_string();
      exceptionWasThrown = false;
    }
    catch (...) {
    }
    assert(exceptionWasThrown);

    auto const triggerResults =
      ev.getValidHandle<art::TriggerResults>(inputTagTriggerResults);
    std::cout << "psetID = " << triggerResults->parameterSetID().to_string()
              << "\n";
    std::string const test3 = triggerResults->parameterSetID().to_string();
    assert(test1 == test3);
    assert(test2 == test3);

    auto const eventIDInt =
      ev.getValidHandle<arttest::IntProduct>(inputTagEventID);
    assert(static_cast<unsigned int>(eventIDInt->value) == aux.id().event());

    auto const intProduct111 =
      ev.getValidHandle<arttest::IntProduct>(inputTag111);
    assert(intProduct111->value == 111);

    auto const intProduct121 =
      ev.getValidHandle<arttest::IntProduct>(inputTag121);
    assert((*intProduct121).value == 121);

    auto const intProduct131 =
      ev.getValidHandle<arttest::IntProduct>(inputTag131);
    assert(intProduct131.product()->value == 131);

    auto const stringProduct111 =
      ev.getValidHandle<arttest::StringProduct>(inputTags111);
    assert(stringProduct111->name_ == "s111");

    auto const stringProduct121 =
      ev.getValidHandle<arttest::StringProduct>(inputTags121);
    assert(stringProduct121->name_ == "s121");

    auto const stringProduct131 =
      ev.getValidHandle<arttest::StringProduct>(inputTags131);
    assert(stringProduct131->name_ == "s131");

    auto const intProduct211 =
      ev.getValidHandle<arttest::IntProduct>(inputTag211);
    assert(intProduct211->value == 211);

    auto const intProduct221 =
      ev.getValidHandle<arttest::IntProduct>(inputTag221);
    assert(intProduct221->value == 221);

    auto const intProduct112 =
      ev.getValidHandle<arttest::IntProduct>(inputTag112);
    assert(intProduct112->value == 112);

    auto const intProduct122 =
      ev.getValidHandle<arttest::IntProduct>(inputTag122);
    assert(intProduct122->value == 122);

    auto const intProduct113 =
      ev.getValidHandle<arttest::IntProduct>(inputTag113);
    assert(intProduct113->value == 113);

    auto const intProduct123 =
      ev.getValidHandle<arttest::IntProduct>(inputTag123);
    assert(intProduct123->value == 123);

    auto const intProduct11 =
      ev.getValidHandle<arttest::IntProduct>(inputTag11);
    assert(intProduct11->value == 113);

    auto const intProduct12 =
      ev.getValidHandle<arttest::IntProduct>(inputTag12);
    assert(intProduct12->value == 123);

    auto const stringProduct11 =
      ev.getValidHandle<arttest::StringProduct>(inputTags11);
    assert(stringProduct11->name_ == "s113");

    auto const stringProduct12 =
      ev.getValidHandle<arttest::StringProduct>(inputTags12);
    assert(stringProduct12->name_ == "s123");

    auto const intProduct21 =
      ev.getValidHandle<arttest::IntProduct>(inputTag21);
    assert(intProduct21->value == 213);

    auto const intProduct22 =
      ev.getValidHandle<arttest::IntProduct>(inputTag22);
    assert(intProduct22->value == 223);

    auto const intProduct31 =
      ev.getValidHandle<arttest::IntProduct>(inputTag31);
    assert(intProduct31->value == 312);

    auto const intProduct32 =
      ev.getValidHandle<arttest::IntProduct>(inputTag32);
    assert(intProduct32->value == 322);

    auto const intProduct312 =
      ev.getValidHandle<arttest::IntProduct>(inputTag312);
    assert(intProduct312->value == 312);

    auto const intProduct322 =
      ev.getValidHandle<arttest::IntProduct>(inputTag322);
    assert(intProduct322->value == 322);

    auto const intProduct41 =
      ev.getValidHandle<arttest::IntProduct>(inputTag41);
    assert(intProduct41->value == 411);

    auto const intProduct42 =
      ev.getValidHandle<arttest::IntProduct>(inputTag42);
    assert(intProduct42->value == 421);

    auto const intProduct411 =
      ev.getValidHandle<arttest::IntProduct>(inputTag411);
    assert(intProduct411->value == 411);

    auto const intProduct421 =
      ev.getValidHandle<arttest::IntProduct>(inputTag421);
    assert(intProduct421->value == 421);

    auto const intProduct51 =
      ev.getValidHandle<arttest::IntProduct>(inputTag51);
    assert(intProduct51->value == 512);

    auto const intProduct52 =
      ev.getValidHandle<arttest::IntProduct>(inputTag52);
    assert(intProduct52->value == 522);

    auto const stringProduct61 =
      ev.getValidHandle<arttest::StringProduct>(inputTags61);
    assert(stringProduct61->name_ == "s611");

    auto const stringProduct62 =
      ev.getValidHandle<arttest::StringProduct>(inputTags62);
    assert(stringProduct62->name_ == "s621");

    // Read all StringProducts from input file.  The first non-empty
    // file is a merged file with 10 events.  The first five events
    // contain 36 StringProduct products.  All other events contain 39
    // StringProduct products.  Note that test_gallery7.root has 48
    // StringProduct branches, but since the presence bit is false for
    // 9 of those branches, 39 products are retrieved.

    std::size_t const expectedNumber = (counter < 5 ? 36 : 39);
    auto const tags = ev.getInputTags<arttest::StringProduct>();
    auto const tokens = ev.getProductTokens<arttest::StringProduct>();

    std::vector<gallery::Handle<arttest::StringProduct>> stringProducts;
    ev.getManyByType(stringProducts);
    assert(expectedNumber == stringProducts.size());
    for (auto const& h : stringProducts) {
      assert(h.isValid());
      assert(!h->name_.empty());
      auto const productID = h.id();
      assert(productID.isValid());
      auto const& pd = ev.getProductDescription(productID);
      assert(pd.friendlyClassName() == "arttest::StringProduct"s);
      assert(!pd.inputTag().empty());
    }

    if (counter > 9) {
      auto const intProductFile2 =
        ev.getValidHandle<arttest::IntProduct>(inputTagFile2);
      assert(intProductFile2->value == 2000);
      auto const& pd = ev.getProductDescription(intProductFile2.id());
      assert(pd.inputTag() == inputTagFile2);
    }

    auto const ptrTestProduct =
      ev.getValidHandle<critictest::LitePtrTestProduct>(inputTagPtrTest);

    int const eventInt = static_cast<int>(aux.id().event());
    auto const& ptrInt1 = ptrTestProduct->ptrInt1;
    assert(*ptrInt1 == 111 + eventInt);
    assert(ptrInt1.isAvailable());

    // Test getProductDescription code
    auto const pid = ptrInt1.id();
    auto const& pd = ev.getProductDescription(pid);
    assert(pd.inputTag() == inputTag111);

    assert(*ptrTestProduct->ptrInt2 == 122);
    assert(*ptrTestProduct->ptrInt3 == 133);

    assert(ptrTestProduct->ptrSimple1->key == 111);
    assert(ptrTestProduct->ptrSimple2->key == 122);
    assert(ptrTestProduct->ptrSimple3->key == 133);
    assert(ptrTestProduct->ptrSimple3->dummy() == 16.25);

    assert(ptrTestProduct->ptrSimpleDerived1->key == 111);
    assert(ptrTestProduct->ptrSimpleDerived2->key == 122);
    assert(ptrTestProduct->ptrSimpleDerived3->key == 133);
    assert(ptrTestProduct->ptrSimpleDerived3->dummy() == 16.25);

    assert(*ptrTestProduct->ptrVectorInt.at(0) == 111 + eventInt);
    assert(ptrTestProduct->ptrVectorInt.isAvailable());
    assert(*ptrTestProduct->ptrVectorInt.at(1) == 131);

    assert(ptrTestProduct->ptrVectorSimple.at(0)->key == 111);
    assert(ptrTestProduct->ptrVectorSimple.at(1)->key == 131);
    assert(ptrTestProduct->ptrVectorSimple.at(1)->dummy() == 16.25);

    assert(ptrTestProduct->ptrVectorSimpleDerived.at(0)->key == 111);
    assert(ptrTestProduct->ptrVectorSimpleDerived.at(1)->key == 131);
    assert(ptrTestProduct->ptrVectorSimpleDerived.at(1)->dummy() == 16.25);

    assert(ptrTestProduct->ptrInt1 && ptrTestProduct->ptrInt1.isAvailable() &&
           !ptrTestProduct->ptrInt1.isNull());

    assert(!ptrTestProduct->ptrIntoContainerToBeDropped &&
           !ptrTestProduct->ptrIntoContainerToBeDropped.isAvailable() &&
           !ptrTestProduct->ptrIntoContainerToBeDropped.isNull());

    bool exceptionThrown{true};
    try {
      auto const x = *ptrTestProduct->ptrIntoContainerToBeDropped;
      std::cout << x << "\n";
      exceptionThrown = false;
    }
    catch (...) {
    }
    assert(exceptionThrown);

    assert(!ptrTestProduct->nullPtr && ptrTestProduct->nullPtr.isAvailable() &&
           ptrTestProduct->nullPtr.isNull());

    // I chose not to test the behavior if a null or invalid Ptr is
    // dereferenced. As of this writing the behavior is this:
    //
    // Ptr::get() will return a null pointer for a null or invalid Ptr
    // Ptr::operator*() will dereference whatever "get" returns (so segfault)
    // Ptr::operator->() throws an exception for invalid Ptrs and
    // I am guessing undefined behavior for null Ptrs
    //
    // This is all in the Ptr class and really has nothing to do with
    // gallery. The same behavior should exist in the full art
    // framework.

    assert(!ptrTestProduct->nullDroppedPtr &&
           !ptrTestProduct->nullDroppedPtr.isAvailable() &&
           ptrTestProduct->nullDroppedPtr.isNull());

    // Note some of the documentation says an invalid Ptr is nonNull,
    // but that is not the way the Ptr class is written.
    assert(!ptrTestProduct->invalidPtr &&
           !ptrTestProduct->invalidPtr.isAvailable() &&
           ptrTestProduct->invalidPtr.isNull());

    using AssnsAB_t =
      art::Assns<arttest::StringProduct, int, critictest::LiteAssnTestData>;
    using AssnsBA_t =
      art::Assns<int, arttest::StringProduct, critictest::LiteAssnTestData>;
    auto const& assnsAB1 = *ev.getValidHandle<AssnsAB_t>(inputTagAssnTest1);
    auto const& assnsAB2 = *ev.getValidHandle<AssnsAB_t>(inputTagAssnTest2);
    auto const& assnsBA3 = *ev.getValidHandle<AssnsBA_t>(inputTagAssnTest1);
    auto const& assnsBA4 = *ev.getValidHandle<AssnsBA_t>(inputTagAssnTest2);

    // 1 and 2 should be the same
    // 3 and 4 should be the same
    // 1 and 3 are also the same except first and second of the pair<Ptr,Ptr>
    // stored in the Assns are reversed.

    if (ev.fileEntry() == 1) {
      assert(assnsAB1[0].first->name_ == "s111"s);
      assert(*assnsAB2[0].second == 121);
      assert(assnsAB1.data(0).label == "A"s);

      assert(assnsAB1[1].first->name_ == "s121"s);
      assert(*assnsAB2[1].second == 131);
      assert(assnsAB1.data(1).label == "B"s);
    } else {
      assert(assnsAB1.at(0).first->name_ == "s131"s);
      assert(*assnsAB2[0].second == 131);
      assert(assnsAB1.data(0).label == "C"s);

      assert(assnsAB1[1].first->name_ == "s111"s);
      assert(*assnsAB2[1].second == 121);
      assert(assnsAB1.data(1).label == "D"s);
    }

    if (ev.fileEntry() == 1) {
      assert(assnsBA3[0].second->name_ == "s111"s);
      assert(*assnsBA4[0].first == 121);
      assert(assnsBA3.data(0).label == "A"s);

      assert(assnsBA3[1].second->name_ == "s121"s);
      assert(*assnsBA4[1].first == 131);
      assert(assnsBA3.data(1).label == "B"s);
    } else {
      assert(assnsBA3.at(0).second->name_ == "s131"s);
      assert(*assnsBA4[0].first == 131);
      assert(assnsBA3.data(0).label == "C"s);

      assert(assnsBA3[1].second->name_ == "s111"s);
      assert(*assnsBA4[1].first == 121);
      assert(assnsBA3.data(1).label == "D"s);
    }

    auto const& hVStringProduct =
      ev.getValidHandle<std::vector<arttest::StringProduct>>(inputTag111);
    art::FindOne<int, critictest::LiteAssnTestData> const findOne{
      hVStringProduct, ev, inputTagAssnTest1};
    assert(findOne.at(0).isValid());
    if (ev.fileEntry() == 1) {
      assert(findOne.at(0).ref() == 121);
      assert(findOne.at(1).ref() == 131);
      assert(findOne.data(0).ref().label == "A"s);
      assert(findOne.data(1).ref().label == "B"s);
    } else {
      assert(findOne.at(0).ref() == 121);
      assert(findOne.at(2).ref() == 131);
      assert(findOne.data(0).ref().label == "D"s);
      assert(findOne.data(2).ref().label == "C"s);
    }

    auto const& hB = ev.getValidHandle<std::vector<int>>(inputTag111);
    art::FindOne<arttest::StringProduct, critictest::LiteAssnTestData> const
      findOneBA{hB, ev, inputTagAssnTest1};
    assert(findOneBA.at(1).isValid());
    if (ev.fileEntry() == 1) {
      assert(findOneBA.at(1).ref() == arttest::StringProduct{"s111"s});
      assert(findOneBA.at(2).ref() == arttest::StringProduct{"s121"s});
      assert(findOneBA.data(1).ref().label == "A"s);
      assert(findOneBA.data(2).ref().label == "B"s);
    } else {
      assert(findOneBA.at(1).ref() == arttest::StringProduct{"s111"s});
      assert(findOneBA.at(2).ref() == arttest::StringProduct{"s131"s});
      assert(findOneBA.data(1).ref().label == "D"s);
      assert(findOneBA.data(2).ref().label == "C"s);
    }
  }
  assert(counter == 20u);
  assert(!ev.isValid());
}
