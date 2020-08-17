#ifndef critic_test_gallery_TestFindOne_h
#define critic_test_gallery_TestFindOne_h

// Started as a copy of AssnsAnalyzer_module.cc. Modified
// to work with gallery and critic. The checks that are
// made remain about the same. The main difference being
// the checks related to View were deleted.

#include "art/test/TestObjects/ToyProducts.h"
#include "boost/type_traits/has_dereference.hpp"
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/FindMany.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "canvas/Persistency/Common/FindOne.h"
#include "canvas/Persistency/Common/FindOneP.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/detail/is_handle.h"
#include "canvas/Utilities/InputTag.h"
#include "cetlib/maybe_ref.h"
#include "cetlib/quiet_unit_test.hpp"
#include "critic/test/CriticTestObjects/LiteAssnTestData.h"
#include "gallery/Event.h"
#include "gallery/Handle.h"

#include <string>

namespace critictest {

  class TestFindOne {
  public:
    inline TestFindOne(std::string const& inputLabel,
                       bool testAB,
                       bool testBA,
                       bool bCollMissing);

    template <template <typename, typename> class FO>
    void testOne(gallery::Event const& e) const;

    template <template <typename, typename> class FM>
    void testMany(gallery::Event const& e) const;

  private:
    std::string const inputLabel_;
    bool const testAB_;
    bool const testBA_;
    bool const bCollMissing_;
  };
} // namespace critictest

namespace {

  typedef int A_t;
  typedef arttest::StringProduct B_t;
  typedef art::Assns<int, arttest::StringProduct, critictest::LiteAssnTestData>
    AssnsAB_t;
  typedef art::Assns<arttest::StringProduct, int, critictest::LiteAssnTestData>
    AssnsBA_t;
  typedef art::Assns<int, arttest::StringProduct> AssnsABV_t;
  typedef art::Assns<arttest::StringProduct, int> AssnsBAV_t;

  // function template to allow us to dereference both maybe_ref<T>
  // objects and objects that have an operator*.
  template <class R, class W>
  R const& dereference(W const& wrapper);

  // Common case, can dereference (eg Ptr<T>).
  template <typename T, template <typename> class WRAP>
  typename std::enable_if<boost::has_dereference<WRAP<T>>::value,
                          T const&>::type
  dereference(WRAP<T> const& wrapper)
  {
    return *wrapper;
  }

  // maybe_ref<T>.
  template <typename T, template <typename> class WRAP>
  typename std::enable_if<std::is_same<WRAP<T>, cet::maybe_ref<T>>::value,
                          T const&>::type
  dereference(WRAP<T> const& wrapper)
  {
    return wrapper.ref();
  }

  // These are the expected answers; they are global data used in
  // check_get_one_impl.
  char const* const X[] = {"zero", "one", "two"};
  char const* const A[] = {"A", "B", "C"};
  size_t const AI[] = {2, 0, 1}; // Order in Acoll
  size_t const BI[] = {1, 2, 0}; // Order in Bcoll

  // check_get_one_impl is the common implementation for the check_get
  // functions.
  template <typename I, typename D, typename F, typename FV>
  void
  check_get_one_impl(F const& fA, FV const& fAV)
  {
    I item;
    D data;
    for (size_t i = 0; i < 3; ++i) {
      fA.get(i, item, data);
      BOOST_TEST(dereference(item) == static_cast<int>(BI[i]));
      BOOST_TEST(dereference(data).d1 == AI[i]);
      BOOST_TEST(dereference(data).d2 == i);
      fAV.get(i, item);
      BOOST_TEST(dereference(item) == static_cast<int>(BI[i]));
    }
  }

  // check_get tests that both fA and fAV both contain the
  // expected information, as recorded in the global variables.
  // Each looks like:
  //    template <typename T, typename D, template <typename, typename> class F>
  //    void
  //    check_get(F<T,D> const&, F<T,void> const&);
  // with complications using enable_if to control which overload is
  // called in a given case.

  // check_get specialized for FindOne
  template <typename T, typename D, template <typename, typename> class FO>
  typename std::enable_if<
    std::is_same<FO<T, void>, art::FindOne<T, void>>::value>::type
  check_get(FO<T, D> const& fA, FO<T, void> const& fAV)
  {
    typedef cet::maybe_ref<typename FO<T, void>::assoc_t const> item_t;
    typedef cet::maybe_ref<typename FO<T, D>::data_t const> data_t;
    check_get_one_impl<item_t, data_t>(fA, fAV);
  }

  // check_get specialized for FindOneP
  template <typename T, typename D, template <typename, typename> class FO>
  typename std::enable_if<
    std::is_same<FO<T, void>, art::FindOneP<T, void>>::value>::type
  check_get(FO<T, D> const& fA, FO<T, void> const& fAV)
  {
    typedef art::Ptr<typename FO<T, void>::assoc_t> item_t;
    typedef cet::maybe_ref<typename FO<T, D>::data_t const> data_t;
    check_get_one_impl<item_t, data_t>(fA, fAV);
  }

  // check_get specialized for FindMany and FindManyP
  template <typename T, typename D, template <typename, typename> class FM>
  typename std::enable_if<
    std::is_same<FM<T, void>, art::FindMany<T, void>>::value ||
    std::is_same<FM<T, void>, art::FindManyP<T, void>>::value>::type
  check_get(FM<T, D> const& fA, FM<T, void> const& fAV)
  {
    typename FM<T, void>::value_type item;
    typename FM<T, D>::dataColl_t::value_type data;
    BOOST_TEST((fAV.get(0ul, item)) == 1ul);
    BOOST_TEST((fAV.get(1ul, item)) == 2ul);
    BOOST_TEST((fAV.get(2ul, item)) == 1ul);
    BOOST_TEST((fA.get(0ul, item, data)) == 1ul);
    BOOST_TEST((fA.get(1ul, item, data)) == 2ul);
    BOOST_TEST((fA.get(2ul, item, data)) == 1ul);
  }
} // namespace

critictest::TestFindOne::TestFindOne(std::string const& inputLabel,
                                     bool testAB,
                                     bool testBA,
                                     bool bCollMissing)
  : inputLabel_(inputLabel)
  , testAB_(testAB)
  , testBA_(testBA)
  , bCollMissing_(bCollMissing)
{}

template <template <typename, typename = void> class FO>
void
critictest::TestFindOne::testOne(gallery::Event const& e) const
{
  static constexpr bool isFOP =
    std::is_same<typename FO<B_t>::value_type,
                 art::Ptr<typename FO<B_t>::assoc_t>>::value;
  bool const extendedTestsOK = isFOP || (!bCollMissing_);
  gallery::Handle<AssnsAB_t> hAB;
  gallery::Handle<AssnsBA_t> hBA;
  gallery::Handle<AssnsABV_t> hABV;
  gallery::Handle<AssnsBAV_t> hBAV;
  if (testAB_) {
    BOOST_TEST_REQUIRE(e.getByLabel(inputLabel_, hAB));
    BOOST_TEST_REQUIRE(hAB->size() == 3ul);
    BOOST_TEST_REQUIRE(e.getByLabel(inputLabel_, hABV));
    BOOST_TEST_REQUIRE(hABV->size() == 3ul);
  }
  if (testBA_) {
    BOOST_TEST_REQUIRE(e.getByLabel(inputLabel_, hBA));
    BOOST_TEST_REQUIRE(hBA->size() == 3ul);
    BOOST_TEST_REQUIRE(e.getByLabel(inputLabel_, hBAV));
    BOOST_TEST_REQUIRE(hBAV->size() == 3ul);
  }
  // Construct a FO using a handle to a collection.
  gallery::Handle<std::vector<int>> hAcoll;
  BOOST_TEST_REQUIRE(e.getByLabel(inputLabel_, hAcoll));
  auto vhAcoll = e.getValidHandle<std::vector<int>>(inputLabel_);
  // First, check we can make an FO on a non-existent label without
  // barfing immediately.
  FO<arttest::StringProduct, critictest::LiteAssnTestData> foDead(
    hAcoll, e, "noModule");
  BOOST_TEST_REQUIRE(!foDead.isValid());
  BOOST_REQUIRE_EXCEPTION(
    foDead.size(), art::Exception, [](art::Exception const& e) {
      return e.categoryCode() == art::errors::LogicError &&
             e.history().back() == "ProductNotFound";
    });
  BOOST_REQUIRE_EXCEPTION(
    foDead.data(0), art::Exception, [](art::Exception const& e) {
      return e.categoryCode() == art::errors::LogicError &&
             e.history().back() == "ProductNotFound";
    });

  // Now do our normal checks.
  gallery::Handle<std::vector<arttest::StringProduct>> hBcoll;
  std::unique_ptr<FO<int, critictest::LiteAssnTestData>> foA;
  std::unique_ptr<FO<int, void>> foAV;
  if (extendedTestsOK) {
    if (!bCollMissing_) {
      BOOST_TEST_REQUIRE(e.getByLabel(inputLabel_, hBcoll));
      if (testBA_) {
        foA.reset(
          new FO<int, critictest::LiteAssnTestData>(hBcoll, e, inputLabel_));
        foAV.reset(new FO<int, void>(hBcoll, e, inputLabel_));
      }
    }
    FO<arttest::StringProduct, critictest::LiteAssnTestData> foB(
      hAcoll, e, inputLabel_);
    FO<arttest::StringProduct, critictest::LiteAssnTestData> foB2(
      vhAcoll, e, inputLabel_);
    FO<arttest::StringProduct, void> foBV(hAcoll, e, inputLabel_);
    std::vector<art::Ptr<int>> vp;
    vp.reserve(3);
    for (size_t i = 0; i < 3; ++i) {
      vp.emplace_back(art::ProductID(), &hAcoll.product()->at(i), i);
      if (testAB_) {
        BOOST_TEST(*(*hAB)[i].first == static_cast<int>(i));
        if (!bCollMissing_) {
          BOOST_TEST(*(*hAB)[i].second == B_t(std::string(X[i])));
        }
        BOOST_TEST((*hAB).data(i).d1 == (*hAB)[i].first.key());
        BOOST_TEST((*hAB).data(i).d2 == (*hAB)[i].second.key());
        BOOST_TEST((*hAB).data(i).label == std::string(A[i]));
        BOOST_TEST(*(*hABV)[i].first == static_cast<int>(i));
        if (!bCollMissing_) {
          BOOST_TEST(*(*hABV)[i].second == B_t(std::string(X[i])));
        }
      }
      if (testBA_) {
        if (!bCollMissing_) {
          BOOST_TEST(*(*hBA)[i].first == B_t(std::string(X[i])));
        }
        BOOST_TEST(*(*hBA)[i].second == static_cast<int>(i));
        BOOST_TEST((*hBA).data(i).d2 == (*hBA)[i].first.key());
        BOOST_TEST((*hBA).data(i).d1 == (*hBA)[i].second.key());
        BOOST_TEST((*hBA).data(i).label == std::string(A[i]));
        if (!bCollMissing_) {
          BOOST_TEST(*(*hBAV)[i].first == B_t(std::string(X[i])));
        }
        BOOST_TEST(*(*hBAV)[i].second == static_cast<int>(i));
      }
      if (bCollMissing_) {
        BOOST_TEST(!foBV.at(i));
      } else {
        BOOST_TEST(dereference(foBV.at(i)) == B_t(std::string(X[AI[i]])));
        if (testBA_) {
          BOOST_TEST(dereference(foA->at(i)) == static_cast<int>(BI[i]));
          BOOST_TEST(dereference(foA->data(i)).d1 == AI[i]);
          BOOST_TEST(dereference(foA->data(i)).d2 == i);
          BOOST_TEST(dereference(foAV->at(i)) == static_cast<int>(BI[i]));
          BOOST_CHECK_NO_THROW(check_get(*foA, *foAV));
        }
      }
      for (auto const& f : {foB, foB2}) {
        if (!bCollMissing_) {
          BOOST_TEST(dereference(f.at(i)) == B_t(std::string(X[AI[i]])));
        }
        BOOST_TEST(dereference(f.data(i)).d1 == i);
        BOOST_TEST(dereference(f.data(i)).d2 == BI[i]);
      }
    }

    // Check FindOne looking into a map_vector.
    BOOST_TEST_REQUIRE(hAcoll.isValid());
    art::InputTag tag(inputLabel_, "mapvec");
    FO<B_t, critictest::LiteAssnTestData> foBmv(hAcoll, e, tag);
    if (!bCollMissing_) {
      BOOST_TEST(dereference(foBmv.at(0)) == dereference(foB.at(0)));
      BOOST_TEST(dereference(foBmv.data(0)).label ==
                 dereference(foB.data(0)).label);
      BOOST_TEST(dereference(foBmv.at(1)) == dereference(foB.at(1)));
      BOOST_TEST(dereference(foBmv.data(1)).label ==
                 dereference(foB.data(1)).label);
      BOOST_TEST(dereference(foBmv.at(2)) == dereference(foB.at(2)));
      BOOST_TEST(dereference(foBmv.data(2)).label ==
                 dereference(foB.data(2)).label);
    }
  }

  // Check alternative accessors and range checking for Assns.
  if (testAB_) {
    BOOST_CHECK_THROW((*hAB).at(3), std::out_of_range);
    BOOST_TEST(&(*hAB).data(0) == &(*hAB).data((*hAB).begin()));
    BOOST_CHECK_THROW((*hAB).data(3), std::out_of_range);
  }
  if (testBA_) {
    BOOST_CHECK_THROW((*hBA).at(3), std::out_of_range);
    BOOST_TEST(&(*hBA).data(0) == &(*hBA).data((*hBA).begin()));
    BOOST_CHECK_THROW((*hBA).data(3), std::out_of_range);
  }
}

template <template <typename, typename> class FM>
void
critictest::TestFindOne::testMany(gallery::Event const& e) const
{
  gallery::Handle<std::vector<int>> hAcoll;
  BOOST_TEST_REQUIRE(e.getByLabel(inputLabel_, hAcoll));

  // First, check we can make an FM on a non-existent label without
  // barfing immediately.
  FM<B_t, critictest::LiteAssnTestData> fmDead(hAcoll, e, "noModule");
  BOOST_TEST_REQUIRE(!fmDead.isValid());
  BOOST_REQUIRE_EXCEPTION(
    fmDead.size(), art::Exception, [](art::Exception const& e) {
      return e.categoryCode() == art::errors::LogicError &&
             e.history().back() == "ProductNotFound";
    });
  BOOST_REQUIRE_EXCEPTION(
    fmDead.data(0), art::Exception, [](art::Exception const& e) {
      return e.categoryCode() == art::errors::LogicError &&
             e.history().back() == "ProductNotFound";
    });

  // Now do our normal checks.
  // Check FindMany.
  FM<B_t, critictest::LiteAssnTestData> fmB(
    hAcoll, e, art::InputTag(inputLabel_, "many"));
  art::Ptr<int> larry(art::ProductID(), &hAcoll.product()->at(0), 0),
    curly(art::ProductID(), &hAcoll.product()->at(1), 1),
    mo(art::ProductID(), &hAcoll.product()->at(2), 2);
  FM<B_t, critictest::LiteAssnTestData> fmB2(
    {larry, curly, mo}, e, art::InputTag(inputLabel_, "many"));
  for (auto const& f : {fmB, fmB2}) {
    BOOST_TEST_REQUIRE(f.size() == 3ul);
    BOOST_TEST(f.at(0).size() == 1ul);
    BOOST_TEST(f.at(1).size() == 2ul);
    BOOST_TEST(f.at(2).size() == 1ul);
    BOOST_TEST(f.data(0).size() == 1ul);
    BOOST_TEST(f.data(1).size() == 2ul);
    BOOST_TEST(f.data(2).size() == 1ul);
  }
  FM<B_t, void> fmBV(hAcoll, e, art::InputTag(inputLabel_, "many"));
  BOOST_TEST(fmBV.at(0).size() == 1ul);
  BOOST_TEST(fmBV.at(1).size() == 2ul);
  BOOST_TEST(fmBV.at(2).size() == 1ul);
  BOOST_CHECK_NO_THROW(check_get(fmB, fmBV));

  // Check FindMany on map_vector
  FM<B_t, void> fmvBV(hAcoll, e, art::InputTag(inputLabel_, "manymapvec"));
  BOOST_TEST(fmvBV.at(0).size() == 1ul);
  BOOST_TEST(fmvBV.at(1).size() == 2ul);
  BOOST_TEST(fmvBV.at(2).size() == 1ul);
  BOOST_CHECK_NO_THROW(check_get(fmB, fmBV));
}

#endif /* critic_test_gallery_TestFindOne_h */

// Local Variables:
// mode: c++
// End:
