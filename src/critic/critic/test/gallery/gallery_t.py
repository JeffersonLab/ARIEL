from __future__ import print_function

import os
import ROOT

filenames = ROOT.vector(ROOT.string)()
filenames.push_back("../gallery_makeInput5.d/test_gallery5.root")
filenames.push_back("../gallery_makeInput7.d/test_gallery7.root")
ev = ROOT.gallery.Event(filenames)

inputTagEventID = ROOT.art.InputTag("m1", "eventID", "PROD1");
inputTagTriggerResults = ROOT.art.InputTag("TriggerResults", "", "PROD1");
inputTagIncorrect = ROOT.art.InputTag("IncorrectLabel", "IncorrectInstance", "PROD1");
inputTags62 = ROOT.art.InputTag("m6", "i2");
inputTagPtrTest = ROOT.art.InputTag("ptr1");

inputTag111 = ROOT.art.InputTag("m1", "", "PROD1");
inputTagAssnTest1 = ROOT.art.InputTag("ptr1");
inputTagAssnTest2 = ROOT.art.InputTag("ptr1", "", "PROD3");

iEvent = 1
ev.toBegin()
while ( not ev.atEnd()) :
  aux = ev.eventAuxiliary()
  print('Event number = {}"'.format(aux.id().event()))
  if iEvent == 6 :
    iEvent = 1; # Events numbers go from 1 to 5 and repeat 1 to 5
  assert(iEvent == aux.id().event()), "Unexpected event number in EventAuxiliary"

  eventIDInt = ev.getValidHandle(ROOT.arttest.IntProduct)(inputTagEventID);
  assert(eventIDInt.value == aux.id().event());

  triggerResultsHandle = ROOT.gallery.Handle(ROOT.art.TriggerResults)();
  assert( not triggerResultsHandle.isValid() );
  assert( not triggerResultsHandle.whyFailed() );

  assert(ev.getByLabel(inputTagTriggerResults, triggerResultsHandle));
  assert(triggerResultsHandle.isValid());
  assert( not triggerResultsHandle.whyFailed() );
  test1 = triggerResultsHandle.parameterSetID().to_string();
  test2 = (triggerResultsHandle).parameterSetID().to_string();
  assert( not ev.getByLabel(inputTagIncorrect, triggerResultsHandle) );
  assert( not triggerResultsHandle.isValid());
  assert(triggerResultsHandle.whyFailed());

  # This will throw
  #test4 = triggerResultsHandle.parameterSetID().to_string();

  triggerResults = ev.getValidHandle(ROOT.art.TriggerResults)(inputTagTriggerResults);
  print(triggerResults.parameterSetID().to_string())

  test3 = triggerResults.parameterSetID().to_string();
  assert(test1 == test3);
  assert(test2 == test3);

  stringProduct62 = ev.getValidHandle(ROOT.arttest.StringProduct)(inputTags62);
  assert(stringProduct62.name_ == "s621");

  ptrTestProduct = ev.getValidHandle(ROOT.critictest.LitePtrTestProduct)(inputTagPtrTest);

  eventInt = aux.id().event();
  value = ptrTestProduct.ptrInt1.get()[0]
  print(value)
  assert(value == 111 + eventInt);

  assert(ptrTestProduct.ptrInt1.isAvailable());
  assert(ptrTestProduct.ptrInt2.get()[0] == 122);
  assert(ptrTestProduct.ptrInt3.get()[0] == 133);

  assert(ptrTestProduct.ptrSimple1.get().key == 111);
  assert(ptrTestProduct.ptrSimple2.get().key == 122);
  print(ptrTestProduct.ptrSimple2.get().key)
  assert(ptrTestProduct.ptrSimple3.get().key == 133);
  assert(ptrTestProduct.ptrSimple3.get().dummy() == 16.25);

  assert(ptrTestProduct.ptrSimpleDerived1.get().key == 111);
  assert(ptrTestProduct.ptrSimpleDerived2.get().key == 122);
  assert(ptrTestProduct.ptrSimpleDerived3.get().key == 133);
  assert(ptrTestProduct.ptrSimpleDerived3.get().dummy() == 16.25);

  assert(ptrTestProduct.ptrVectorInt.at(0).get()[0] == 111 + eventInt);
  assert(ptrTestProduct.ptrVectorInt.isAvailable());
  assert(ptrTestProduct.ptrVectorInt.at(1).get()[0] == 131);

  assert(ptrTestProduct.ptrVectorSimple.at(0).get().key == 111);
  assert(ptrTestProduct.ptrVectorSimple.at(1).get().key == 131);
  assert(ptrTestProduct.ptrVectorSimple.at(1).get().dummy() == 16.25);

  assert(ptrTestProduct.ptrVectorSimpleDerived.at(0).get().key == 111);
  assert(ptrTestProduct.ptrVectorSimpleDerived.at(1).get().key == 131);
  assert(ptrTestProduct.ptrVectorSimpleDerived.at(1).get().dummy() == 16.25);

  assert(ptrTestProduct.ptrInt1 and
         ptrTestProduct.ptrInt1.isAvailable() and
         not ptrTestProduct.ptrInt1.isNull());

  assert(not ptrTestProduct.ptrIntoContainerToBeDropped and
         not ptrTestProduct.ptrIntoContainerToBeDropped.isAvailable() and
         not ptrTestProduct.ptrIntoContainerToBeDropped.isNull());

  # This will throw because the product was dropped.
  # x = ptrTestProduct.ptrIntoContainerToBeDropped.get();

  assert(not ptrTestProduct.nullPtr and
         ptrTestProduct.nullPtr.isAvailable() and
         ptrTestProduct.nullPtr.isNull());

  assert(not ptrTestProduct.nullDroppedPtr and
         not ptrTestProduct.nullDroppedPtr.isAvailable() and
         ptrTestProduct.nullDroppedPtr.isNull());

  assert(not ptrTestProduct.invalidPtr and
         not ptrTestProduct.invalidPtr.isAvailable() and
         ptrTestProduct.invalidPtr.isNull());

  assnsABHandle1 = ROOT.gallery.Handle(ROOT.art.Assns(ROOT.arttest.StringProduct, int, ROOT.critictest.LiteAssnTestData))();
  ev.getByLabel(inputTagAssnTest1, assnsABHandle1);

  assnsABHandle2 = ROOT.gallery.Handle(ROOT.art.Assns(ROOT.arttest.StringProduct, int, ROOT.critictest.LiteAssnTestData))();
  ev.getByLabel(inputTagAssnTest2, assnsABHandle2);

  assnsBAHandle3 = ROOT.gallery.Handle(ROOT.art.Assns(int, ROOT.arttest.StringProduct, ROOT.critictest.LiteAssnTestData))();
  ev.getByLabel(inputTagAssnTest1, assnsBAHandle3);

  assnsBAHandle4 = ROOT.gallery.Handle(ROOT.art.Assns(int, ROOT.arttest.StringProduct, ROOT.critictest.LiteAssnTestData))();
  ev.getByLabel(inputTagAssnTest2, assnsBAHandle4);

  assert(assnsABHandle1.isValid() and
         assnsABHandle2.isValid() and
         assnsBAHandle3.isValid() and
         assnsBAHandle4.isValid());

  # handles 1 and 2 should be the same
  # handles 3 and 4 should be the same
  # 1 and 3 are also the same except first and second of the pair<Ptr,Ptr>
  # stored in the Assns are reversed.

  if (ev.fileEntry() == 0) :
    assert(assnsABHandle1.at(0).first.name_ == "s111");
    assert(assnsABHandle2.at(0).second.get()[0] == 121);
    assert(assnsABHandle1.data(0).label == "A");

    assert(assnsABHandle1.at(1).first.name_ == "s121");
    assert(assnsABHandle2.at(1).second.get()[0] == 131);
    assert(assnsABHandle1.data(1).label == "B");
  else :
    assert(assnsABHandle1.at(0).first.name_ == "s131");
    assert(assnsABHandle2.at(0).second.get()[0] == 131);
    assert(assnsABHandle1.data(0).label == "C");

    assert(assnsABHandle1.at(1).first.name_ == "s111");
    assert(assnsABHandle2.at(1).second.get()[0] == 121);
    assert(assnsABHandle1.data(1).label == "D");

  if (ev.fileEntry() == 0) :
    assert(assnsBAHandle3.at(0).second.name_ == "s111");
    assert(assnsBAHandle4.at(0).first.get()[0] == 121);
    assert(assnsBAHandle3.data(0).label == "A");

    assert(assnsBAHandle3.at(1).second.name_ == "s121");
    assert(assnsBAHandle4.at(1).first.get()[0] == 131);
    assert(assnsBAHandle3.data(1).label == "B");
  else :
    assert(assnsBAHandle3.at(0).second.name_ == "s131");
    assert(assnsBAHandle4.at(0).first.get()[0] == 131);
    assert(assnsBAHandle3.data(0).label == "C");

    assert(assnsBAHandle3.at(1).second.name_ == "s111");
    assert(assnsBAHandle4.at(1).first.get()[0] == 121);
    assert(assnsBAHandle3.data(1).label == "D");

  hVStringProduct = ROOT.gallery.Handle(ROOT.std.vector(ROOT.arttest.StringProduct))();
  ev.getByLabel(inputTag111, hVStringProduct);
  assert(hVStringProduct.isValid());

  findMaker = ROOT.gallery.FindMaker();

  findOne = findMaker.makeFindOne(int, ROOT.critictest.LiteAssnTestData, ROOT.gallery.Handle(ROOT.std.vector(ROOT.arttest.StringProduct)))(hVStringProduct, ev, inputTagAssnTest1);
  assert(findOne.at(0).isValid());
  if (ev.fileEntry() == 0) :
    assert(findOne.at(0).ref() == 121);
    assert(findOne.at(1).ref() == 131);
    assert(findOne.data(0).ref().label == "A");
    assert(findOne.data(1).ref().label == "B");
  else :
    assert(findOne.at(0).ref() == 121);
    assert(findOne.at(2).ref() == 131);
    assert(findOne.data(0).ref().label == "D");
    assert(findOne.data(2).ref().label == "C");

  hB = ROOT.gallery.Handle(ROOT.std.vector(int))();
  ev.getByLabel(inputTag111, hB);
  assert(hB.isValid());

  findOneBA = findMaker.makeFindOne(ROOT.arttest.StringProduct, ROOT.critictest.LiteAssnTestData, ROOT.gallery.Handle(ROOT.std.vector(int)))(hB, ev, inputTagAssnTest1);
  assert(findOneBA.at(1).isValid());
  if (ev.fileEntry() == 0) :
    assert(findOneBA.at(1).ref() == ROOT.arttest.StringProduct(ROOT.std.string("s111")));
    assert(findOneBA.at(2).ref() == ROOT.arttest.StringProduct(ROOT.std.string("s121")));
    assert(findOneBA.data(1).ref().label == "A");
    assert(findOneBA.data(2).ref().label == "B");
  else :
    assert(findOneBA.at(1).ref() == ROOT.arttest.StringProduct(ROOT.std.string("s111")));
    assert(findOneBA.at(2).ref() == ROOT.arttest.StringProduct(ROOT.std.string("s131")));
    assert(findOneBA.data(1).ref().label == "D");
    assert(findOneBA.data(2).ref().label == "C");

  findOneP = findMaker.makeFindOneP(int, ROOT.critictest.LiteAssnTestData, ROOT.gallery.Handle(ROOT.std.vector(ROOT.arttest.StringProduct)))(hVStringProduct, ev, inputTagAssnTest1);
  assert(findOneP.at(0).isNonnull());
  assert(findOneP.at(0).isAvailable());
  if (ev.fileEntry() == 0) :
    assert(findOneP.at(0).get()[0] == 121);
    assert(findOneP.at(1).get()[0] == 131);
    assert(findOneP.data(0).ref().label == "A");
    assert(findOneP.data(1).ref().label == "B");
  else :
    assert(findOneP.at(0).get()[0] == 121);
    assert(findOneP.at(2).get()[0] == 131);
    assert(findOneP.data(0).ref().label == "D");
    assert(findOneP.data(2).ref().label == "C");

  findManyBA = findMaker.makeFindMany(ROOT.arttest.StringProduct, ROOT.critictest.LiteAssnTestData, ROOT.gallery.Handle(ROOT.std.vector(int)))(hB, ev, inputTagAssnTest1);
  assert(findManyBA.at(1).size() == 1);
  if (ev.fileEntry() == 0) :
    assert(findManyBA.at(1).at(0) == ROOT.arttest.StringProduct(ROOT.std.string("s111")));
    assert(findManyBA.at(2).at(0) == ROOT.arttest.StringProduct(ROOT.std.string("s121")));
    assert(findManyBA.data(1).at(0).label == "A");
    assert(findManyBA.data(2).at(0).label == "B");
  else :
    assert(findManyBA.at(1).at(0) == ROOT.arttest.StringProduct(ROOT.std.string("s111")));
    assert(findManyBA.at(2).at(0) == ROOT.arttest.StringProduct(ROOT.std.string("s131")));
    assert(findManyBA.data(1).at(0).label == "D");
    assert(findManyBA.data(2).at(0).label == "C");

  findManyP = findMaker.makeFindManyP(int, ROOT.critictest.LiteAssnTestData, ROOT.gallery.Handle(ROOT.std.vector(ROOT.arttest.StringProduct)))(hVStringProduct, ev, inputTagAssnTest1);
  assert(findManyP.at(0).size() == 1);
  if (ev.fileEntry() == 0) :
    assert(findManyP.at(0).at(0).get()[0] == 121);
    assert(findManyP.at(1).at(0).get()[0] == 131);
    assert(findManyP.data(0).at(0).label == "A");
    assert(findManyP.data(1).at(0).label == "B");
  else :
    assert(findManyP.at(0).at(0).get()[0] == 121);
    assert(findManyP.at(2).at(0).get()[0] == 131);
    assert(findManyP.data(0).at(0).label == "D");
    assert(findManyP.data(2).at(0).label == "C");

  ev.next()
  iEvent += 1
