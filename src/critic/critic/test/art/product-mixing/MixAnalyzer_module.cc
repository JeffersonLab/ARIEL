////////////////////////////////////////////////////////////////////////
// Class:       MixAnalyzer
// Module Type: analyzer
// File:        MixAnalyzer_module.cc
//
// Generated at Mon May 16 10:45:57 2011 by Chris Green using artmod
// from art v0_06_02.
////////////////////////////////////////////////////////////////////////

#include "cetlib/quiet_unit_test.hpp"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/IO/ProductMix/MixTypes.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/test/TestObjects/ProductWithPtrs.h"
#include "art/test/TestObjects/ToyProducts.h"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace arttest {
  class MixAnalyzer;
}

class arttest::MixAnalyzer : public art::EDAnalyzer {
public:
  explicit MixAnalyzer(fhicl::ParameterSet const& p);

private:
  void analyze(art::Event const& e) override;

  using mv_t = cet::map_vector<unsigned int>;
  using mvv_t = mv_t::value_type;
  using mvm_t = mv_t::mapped_type;

  size_t eventCounter_{};
  size_t nSecondaries_;
  std::string mixFilterLabel_;
};

arttest::MixAnalyzer::MixAnalyzer(fhicl::ParameterSet const& p)
  : art::EDAnalyzer{p}
  , nSecondaries_{p.get<size_t>("numSecondaries", 1)}
  , mixFilterLabel_{p.get<std::string>("mixFilterLabel", "mixFilter")}
{}

void
arttest::MixAnalyzer::analyze(art::Event const& e)
{
  ++eventCounter_;

  // Double
  art::Handle<double> dH;
  e.getByLabel(mixFilterLabel_, "doubleLabel", dH);
  double dExpected =
    ((2 * eventCounter_ - 1) * nSecondaries_ + 1) * nSecondaries_ / 2;
  assert(*dH == dExpected);

  // IntProduct
  art::Handle<IntProduct> ipH;
  BOOST_TEST_REQUIRE(e.getByLabel(mixFilterLabel_, "IntProductLabel", ipH));
  BOOST_TEST_REQUIRE(ipH->value == dExpected + 1000000 * nSecondaries_);

  // String
  art::Handle<std::string> sH;
  BOOST_TEST_REQUIRE(e.getByLabel(mixFilterLabel_, "stringLabel", sH));
  std::ostringstream sExp;
  for (size_t i = 1; i <= nSecondaries_; ++i) {
    sExp << "string value: " << std::setfill('0') << std::setw(7)
         << (eventCounter_ - 1) * nSecondaries_ + i << "\n";
  }
  BOOST_TEST_REQUIRE(*sH == sExp.str());

  // 1. std::vector<double>
  art::Handle<std::vector<double>> vdH;
  BOOST_TEST_REQUIRE(
    e.getByLabel(mixFilterLabel_, "doubleCollectionLabel", vdH));
  BOOST_TEST_REQUIRE(vdH->size() == 10 * nSecondaries_);
  for (size_t i = 0; i < nSecondaries_; ++i) {
    for (size_t j = 1; j < 11; ++j) {
      BOOST_TEST_REQUIRE((*vdH)[i * 10 + j - 1] ==
                         j + 10 * (i + (eventCounter_ - 1) * nSecondaries_));
    }
  }

  // 2. std::vector<art::Ptr<double> >
  // 3. art::PtrVector<double>
  // 4. ProductWithPtrs
  art::Handle<std::vector<art::Ptr<double>>> vpdH;
  BOOST_TEST_REQUIRE(
    e.getByLabel(mixFilterLabel_, "doubleVectorPtrLabel", vpdH)); // 2.
#ifndef ART_NO_MIX_PTRVECTOR
  art::Handle<art::PtrVector<double>> pvdH;
  BOOST_TEST_REQUIRE(
    e.getByLabel(mixFilterLabel_, "doublePtrVectorLabel", pvdH)); // 3.
#endif
  art::Handle<arttest::ProductWithPtrs> pwpH;
  BOOST_TEST_REQUIRE(
    e.getByLabel(mixFilterLabel_, "ProductWithPtrsLabel", pwpH)); // 4.

  for (size_t i = 0; i < nSecondaries_; ++i) {
    BOOST_TEST_REQUIRE(*(*vpdH)[i * 3 + 0] == (*vdH)[(i * 10) + 0]); // 2.
    BOOST_TEST_REQUIRE(*(*vpdH)[i * 3 + 1] == (*vdH)[(i * 10) + 4]); // 2.
    BOOST_TEST_REQUIRE(*(*vpdH)[i * 3 + 2] == (*vdH)[(i * 10) + 8]); // 2.
#ifndef ART_NO_MIX_PTRVECTOR
    BOOST_TEST_REQUIRE(*(*pvdH)[i * 3 + 0] == (*vdH)[(i * 10) + 1]); // 3.
    BOOST_TEST_REQUIRE(*(*pvdH)[i * 3 + 1] == (*vdH)[(i * 10) + 5]); // 3.
    BOOST_TEST_REQUIRE(*(*pvdH)[i * 3 + 2] == (*vdH)[(i * 10) + 9]); // 3.
#endif
    BOOST_TEST_REQUIRE(*(pwpH->vectorPtrDouble())[i * 3 + 0] ==
                       *(*vpdH)[i * 3 + 0]); // 4.
    BOOST_TEST_REQUIRE(*(pwpH->vectorPtrDouble())[i * 3 + 1] ==
                       *(*vpdH)[i * 3 + 1]); // 4.
    BOOST_TEST_REQUIRE(*(pwpH->vectorPtrDouble())[i * 3 + 2] ==
                       *(*vpdH)[i * 3 + 2]); // 4.
#ifndef ART_NO_MIX_PTRVECTOR
    BOOST_TEST_REQUIRE(*(pwpH->ptrVectorDouble())[i * 3 + 0] ==
                       *(*pvdH)[i * 3 + 0]); // 4.
    BOOST_TEST_REQUIRE(*(pwpH->ptrVectorDouble())[i * 3 + 1] ==
                       *(*pvdH)[i * 3 + 1]); // 4.
    BOOST_TEST_REQUIRE(*(pwpH->ptrVectorDouble())[i * 3 + 2] ==
                       *(*pvdH)[i * 3 + 2]); // 4.
#endif
  }

  // map_vector<unsigned int>
  art::Handle<mv_t> mv;
  BOOST_TEST_REQUIRE(e.getByLabel(mixFilterLabel_, "mapVectorLabel", mv));
  BOOST_TEST_REQUIRE(mv->size() == 5 * nSecondaries_);
  {
    auto it = mv->begin();
    size_t delta = 0;
    size_t index = 0;
    for (size_t i = 0; i < nSecondaries_; ++i, delta = index + 1) {
      for (size_t j = 0; j < 5; ++j, ++it) {
        index =
          1 + j * 2 + delta + 10 * (i + nSecondaries_ * (eventCounter_ - 1));
        BOOST_TEST_REQUIRE(it->first ==
                           static_cast<cet::map_vector_key>(index));
        size_t answer = j + 1 + 5 * i + (eventCounter_ - 1) * 5 * nSecondaries_;
        BOOST_TEST_REQUIRE(it->second == answer);
        BOOST_TEST_REQUIRE(*mv->getOrNull(cet::map_vector_key(index)) ==
                           answer);
      }
    }
    std::cerr << "\n";
  }

  // Ptrs into map_vector
  art::Handle<std::vector<art::Ptr<mvv_t>>> mvvp;
  BOOST_TEST_REQUIRE(e.getByLabel(mixFilterLabel_, "intVectorPtrLabel", mvvp));
  BOOST_TEST_REQUIRE(mvvp->size() == 5 * nSecondaries_);
  {
    auto it = mvvp->begin();
    size_t delta = 0;
    size_t index_base = 0;
    for (size_t i = 0; i < nSecondaries_; ++i, delta = index_base + 9, ++it) {
      std::cerr << "delta = " << delta << "\n";
      index_base = delta + 1 + 10 * (i + nSecondaries_ * (eventCounter_ - 1));
      size_t answer_base = (eventCounter_ - 1) * 5 * nSecondaries_ + i * 5 + 1;
      BOOST_TEST_REQUIRE((*it)->first ==
                         static_cast<cet::map_vector_key>(index_base + 6));
      BOOST_TEST_REQUIRE((*it)->second == answer_base + 3);
      ++it;
      BOOST_TEST_REQUIRE((*it)->first ==
                         static_cast<cet::map_vector_key>(index_base + 0));
      BOOST_TEST_REQUIRE((*it)->second == answer_base + 0);
      ++it;
      BOOST_TEST_REQUIRE((*it)->first ==
                         static_cast<cet::map_vector_key>(index_base + 2));
      BOOST_TEST_REQUIRE((*it)->second == answer_base + 1);
      ++it;
      BOOST_TEST_REQUIRE((*it)->first ==
                         static_cast<cet::map_vector_key>(index_base + 8));
      BOOST_TEST_REQUIRE((*it)->second == answer_base + 4);
      ++it;
      BOOST_TEST_REQUIRE((*it)->first ==
                         static_cast<cet::map_vector_key>(index_base + 4));
      BOOST_TEST_REQUIRE((*it)->second == answer_base + 2);
    }
  }

  // Bookkeeping.
  art::Handle<std::string> bsH;
  BOOST_TEST_REQUIRE(e.getByLabel(mixFilterLabel_, bsH));
  BOOST_TEST_REQUIRE((*bsH) == "BlahBlahBlah");

  art::Handle<art::EventIDSequence> eidsH;
  BOOST_TEST_REQUIRE((e.getByLabel(mixFilterLabel_, eidsH)));
  for (size_t i = 0; i < nSecondaries_; ++i) {
    BOOST_TEST_REQUIRE((*eidsH)[i].event() + ((*eidsH)[i].subRun() * 100) +
                         (((*eidsH)[i].run() - 1) * 500) ==
                       (eventCounter_ - 1) * nSecondaries_ + i + 1);
  }
}

DEFINE_ART_MODULE(arttest::MixAnalyzer)
