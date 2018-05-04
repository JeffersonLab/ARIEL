#ifndef cetlib_simple_stats_h
#define cetlib_simple_stats_h

// ======================================================================
//
// simple_stats: yield simple statistics from individually-presented data
//
// ======================================================================

#include <cstddef> // size_t
#include <iosfwd>  // basic_ostream

namespace cet {

  class simple_stats;

  template <class charT, class traits>
  std::basic_ostream<charT, traits>& operator<<(
    std::basic_ostream<charT, traits>&,
    simple_stats const&);
}

// ======================================================================

class cet::simple_stats {
public:
  // c'tors:
  simple_stats() noexcept;
  simple_stats(double x) noexcept;

  // use compiler-generated copy/move/d'tor

  // statistics observers:
  std::size_t
  size() const noexcept
  {
    return n_;
  }
  double
  max() const noexcept
  {
    return max_;
  }
  double
  min() const noexcept
  {
    return min_;
  }
  double
  small() const noexcept
  {
    return small_;
  }
  double
  sum() const noexcept
  {
    return sum_;
  }
  double
  sumsq() const noexcept
  {
    return sumsq_;
  }

  // statistics calculators:
  double mean() const noexcept;
  double range() const noexcept;
  double err_mean(std::size_t nparams = 1u) const noexcept;
  double rms(std::size_t nparams = 1u) const noexcept;
  double rms0(std::size_t nparams = 0u) const noexcept;
  double err_rms(std::size_t nparams = 1u) const noexcept;

  // mutators:
  void reset() noexcept;
  void sample(double) noexcept;
  template <class Iter>
  void
  sample(Iter begin, Iter end) noexcept
  {
    for (; begin != end; ++begin)
      sample(*begin);
  }

private:
  std::size_t n_;            // counter
  double min_, max_, small_; // extrema
  double sum_, sumsq_;       // accumulations

}; // simple_stats

// ======================================================================

template <class charT, class traits>
std::basic_ostream<charT, traits>&
cet::operator<<(std::basic_ostream<charT, traits>& os,
                simple_stats const& stats)
{
  return os << "( " << stats.size() << ", " << stats.mean() << " +/- "
            << stats.err_mean() << ", " << stats.rms() << " +/- "
            << stats.err_rms() << ", " << stats.min() << " " << stats.max()
            << " )";
}

  // ======================================================================

#endif /* cetlib_simple_stats_h */

// Local Variables:
// mode: c++
// End:
