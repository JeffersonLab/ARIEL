// ======================================================================
//
// simple_stats: yield simple statistics from individually-presented data
//
// ======================================================================

#include "cetlib/simple_stats.h"

#include "cetlib/pow.h" // square

#include <algorithm> // max, min
#include <cmath>     // abs, sqrt
#include <limits>    // numeric_limits

using cet::simple_stats;

// ----------------------------------------------------------------------
// c'tors:

simple_stats::simple_stats() noexcept
  : n_(0u)
  , min_(+std::numeric_limits<double>::infinity())
  , max_(-std::numeric_limits<double>::infinity())
  , small_(+std::numeric_limits<double>::infinity())
  , sum_(0.0)
  , sumsq_(0.0)
{
  ;
}

simple_stats::simple_stats(double x) noexcept
  : n_(0u)
  , min_(+std::numeric_limits<double>::infinity())
  , max_(-std::numeric_limits<double>::infinity())
  , small_(+std::numeric_limits<double>::infinity())
  , sum_(0.0)
  , sumsq_(0.0)
{
  sample(x);
}

// ----------------------------------------------------------------------
// statistics calculators:

double
simple_stats::mean() const noexcept
{
  return n_ == 0u ? std::numeric_limits<double>::quiet_NaN() :
                    sum_ / double(n_);
}

double
simple_stats::range() const noexcept
{
  return max_ - min_;
}

double
simple_stats::err_mean(std::size_t nparams) const noexcept
{
  return n_ == 0u ? std::numeric_limits<double>::quiet_NaN() :
                    rms(nparams) / std::sqrt(double(n_));
}

double
simple_stats::rms(std::size_t nparams) const noexcept
{
  if (n_ <= nparams)
    return std::numeric_limits<double>::quiet_NaN();

  double diff = sumsq_ / double(n_) - square(mean());
  if (diff < 0.0)
    return 0.0;

  double factor = double(n_) / double(n_ - nparams);
  return std::sqrt(factor * diff);
}

double
simple_stats::rms0(std::size_t nparams) const noexcept
{
  if (n_ <= nparams)
    return std::numeric_limits<double>::quiet_NaN();

  double diff = sumsq_ / double(n_);
  if (diff < 0.0)
    return 0.0;

  double factor = double(n_) / double(n_ - nparams);
  return std::sqrt(factor * diff);
}

double
simple_stats::err_rms(std::size_t nparams) const noexcept
{
  return n_ == 0u ? std::numeric_limits<double>::quiet_NaN() :
                    rms(nparams) / std::sqrt(double(2u * n_));
}

// ----------------------------------------------------------------------
// mutators:

void
simple_stats::reset() noexcept
{
  *this = simple_stats();
}

void
simple_stats::sample(double x) noexcept
{
  ++n_;
  min_ = std::min(x, min_);
  max_ = std::max(x, max_);
  small_ = std::min(std::abs(x), small_);
  sum_ += x;
  sumsq_ += square(x);
}

// ======================================================================
