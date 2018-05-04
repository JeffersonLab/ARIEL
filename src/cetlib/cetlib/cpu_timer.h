#ifndef cetlib_cpu_timer_h
#define cetlib_cpu_timer_h

// ======================================================================
//
// cpu_timer: Measure the cpu and wallclock elapsed times
//
// ======================================================================

#include <sys/time.h>

namespace cet {
  class cpu_timer;
}

// ======================================================================

class cet::cpu_timer {
public:
  cpu_timer() = default;

  // Allow move
  cpu_timer(cpu_timer&&) = default;
  cpu_timer& operator=(cpu_timer&&) = default;

  // Disable copy
  cpu_timer(cpu_timer const&) = delete;
  cpu_timer& operator=(cpu_timer const&) = delete;

  // --- accessors:
  bool is_running() const;
  bool is_stopped() const;
  double elapsed_real_time() const;
  double elapsed_cpu_time() const;
  double accumulated_real_time() const;
  double accumulated_cpu_time() const;

  inline double
  realTime() const
  {
    return accumulated_real_time();
  }
  inline double
  cpuTime() const
  {
    return accumulated_cpu_time();
  }

  // --- mutators:
  void start();
  void stop();
  void reset();

private:
  // --- state:
  bool is_running_{false};
  timeval start_real_time_{0, 0};
  timeval start_cpu_time_{0, 0};
  double accumulated_real_time_{};
  double accumulated_cpu_time_{};

}; // cpu_timer

// ======================================================================

inline bool
cet::cpu_timer::is_running() const
{
  return is_running_;
}

inline bool
cet::cpu_timer::is_stopped() const
{
  return !is_running();
}

  // ======================================================================

#endif /* cetlib_cpu_timer_h */

// Local variables:
// mode: c++
// End:
