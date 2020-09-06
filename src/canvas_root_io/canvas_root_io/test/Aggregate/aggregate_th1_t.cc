#define BOOST_TEST_MODULE (th1Product aggregation Test)
#include "canvas_root_io/test/Aggregate/MockRun.h"
#include "cetlib/container_algorithms.h"
#include "cetlib/quiet_unit_test.hpp"

#include "TH1F.h"
#include "TH2F.h"

#include <random>
#include <vector>

using arttest::MockRun;

namespace {

  double constexpr tolerance{std::numeric_limits<float>::epsilon()};

  struct RandomNumberHandler {
    std::default_random_engine engine_{};
    std::uniform_real_distribution<float> numDist_{0., 1000.};
    float
    operator()()
    {
      return numDist_(engine_);
    }
  };

  struct Error {
    auto
    operator()(float const val)
    {
      return std::sqrt(val);
    }
  };

  struct LinearAdd {
    auto
    operator()(float const val1, float const val2)
    {
      return val1 + val2;
    }
  };

  struct QuadraticAdd {
    auto
    operator()(float const val1, float const val2)
    {
      return std::sqrt(val1 * val1 + val2 * val2);
    }
  };
}

using std::vector;

BOOST_AUTO_TEST_SUITE(aggregate_th1_t)

BOOST_AUTO_TEST_CASE(th1f)
{
  // Use 'int' because that's what ROOT requires
  constexpr int const nBins{10};
  TH1F h1{"title1", "name1", nBins, 0, 10.};
  h1.Sumw2();
  TH1F h2{"title2", "name2", nBins, 0, 10.};
  h2.Sumw2();

  RandomNumberHandler rnh;
  vector<float> h1d;
  vector<float> h2d;
  std::generate_n(std::back_inserter(h1d), nBins, rnh);
  std::generate_n(std::back_inserter(h2d), nBins, rnh);
  vector<float> h3d;
  cet::transform_all(h1d, h2d, std::back_inserter(h3d), LinearAdd{});

  vector<float> h1err;
  vector<float> h2err;
  cet::transform_all(h1d, std::back_inserter(h1err), Error{});
  cet::transform_all(h2d, std::back_inserter(h2err), Error{});
  vector<float> h3err;
  cet::transform_all(h1err, h2err, std::back_inserter(h3err), QuadraticAdd{});

  for (std::size_t i{0}; i != nBins; ++i) {
    h1.SetBinContent(i + 1, h1d[i]);
    h2.SetBinContent(i + 1, h2d[i]);

    h1.SetBinError(i + 1, h1err[i]);
    h2.SetBinError(i + 1, h2err[i]);
  }

  MockRun r;
  r.put<TH1F>(h1);
  r.put<TH1F>(h2);

  // Aggregation happens here
  auto const& h3 = r.get<TH1F>();
  BOOST_REQUIRE(h3d.size() == static_cast<size_t>(h3.GetNbinsX()));

  for (std::size_t i{0}; i != h3d.size(); ++i) {
    BOOST_CHECK_CLOSE_FRACTION(h3.GetBinContent(i + 1), h3d[i], tolerance);
    BOOST_CHECK_CLOSE_FRACTION(h3.GetBinError(i + 1), h3err[i], tolerance);
  }
}

BOOST_AUTO_TEST_CASE(th2f)
{
  // Use 'int' because that's what ROOT requires
  constexpr int const nBinsX{10}, nBinsY{20};
  TH2F h1{"title1", "name1", nBinsX, 0, 10., nBinsY, -5, 15};
  h1.Sumw2();
  TH2F h2{"title2", "name2", nBinsX, 0, 10., nBinsY, -5, 15};
  h2.Sumw2();

  RandomNumberHandler rnh;
  vector<vector<float>> h1d, h1err;
  vector<vector<float>> h2d, h2err;
  vector<vector<float>> h3d, h3err;

  for (int i{0}; i != nBinsX; ++i) {
    vector<float> tmp1d;
    vector<float> tmp2d;
    std::generate_n(std::back_inserter(tmp1d), nBinsY, rnh);
    std::generate_n(std::back_inserter(tmp2d), nBinsY, rnh);
    vector<float> tmp3d;
    cet::transform_all(tmp1d, tmp2d, std::back_inserter(tmp3d), LinearAdd{});

    vector<float> tmp1err;
    vector<float> tmp2err;
    cet::transform_all(tmp1d, std::back_inserter(tmp1err), Error{});
    cet::transform_all(tmp2d, std::back_inserter(tmp2err), Error{});
    vector<float> tmp3err;
    cet::transform_all(
      tmp1err, tmp2err, std::back_inserter(tmp3err), QuadraticAdd{});

    h1d.push_back(std::move(tmp1d));
    h1err.push_back(std::move(tmp1err));
    h2d.push_back(std::move(tmp2d));
    h2err.push_back(std::move(tmp2err));
    h3d.push_back(std::move(tmp3d));
    h3err.push_back(std::move(tmp3err));
  }

  for (std::size_t i{0}; i != nBinsX; ++i) {
    for (std::size_t j{0}; j != nBinsY; ++j) {
      h1.SetBinContent(i + 1, j + 1, h1d[i][j]);
      h2.SetBinContent(i + 1, j + 1, h2d[i][j]);

      h1.SetBinError(i + 1, j + 1, h1err[i][j]);
      h2.SetBinError(i + 1, j + 1, h2err[i][j]);
    }
  }

  MockRun r;
  r.put<TH2F>(h1);
  r.put<TH2F>(h2);

  // Aggregation happens here
  auto const& h3 = r.get<TH2F>();
  BOOST_REQUIRE(nBinsX == h3.GetNbinsX());
  BOOST_REQUIRE(nBinsY == h3.GetNbinsY());

  for (std::size_t i{0}; i != nBinsX; ++i) {
    for (std::size_t j{0}; j != nBinsY; ++j) {
      BOOST_CHECK_CLOSE_FRACTION(
        h3.GetBinContent(i + 1, j + 1), h3d[i][j], tolerance);
      BOOST_CHECK_CLOSE_FRACTION(
        h3.GetBinError(i + 1, j + 1), h3err[i][j], tolerance);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
