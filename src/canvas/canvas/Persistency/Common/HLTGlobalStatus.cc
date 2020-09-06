#include "canvas/Persistency/Common/HLTGlobalStatus.h"
// vim: set sw=2 expandtab :

#include "canvas/Persistency/Common/HLTPathStatus.h"
#include "canvas/Persistency/Common/HLTenums.h"

#include <atomic>
#include <cstddef>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;
using namespace literals::string_literals;

namespace art {

  HLTGlobalStatus::~HLTGlobalStatus() {}

  HLTGlobalStatus::HLTGlobalStatus(std::size_t const n /* = 0 */) : paths_(n) {}

  HLTGlobalStatus::HLTGlobalStatus(HLTGlobalStatus const& rhs)
    : paths_(rhs.paths_)
  {}

  HLTGlobalStatus::HLTGlobalStatus(HLTGlobalStatus&& rhs)
    : paths_(move(rhs.paths_))
  {}

  HLTGlobalStatus&
  HLTGlobalStatus::operator=(HLTGlobalStatus const& rhs)
  {
    if (this != &rhs) {
      paths_ = rhs.paths_;
    }
    return *this;
  }

  HLTGlobalStatus&
  HLTGlobalStatus::operator=(HLTGlobalStatus&& rhs)
  {
    paths_ = move(rhs.paths_);
    return *this;
  }

  std::size_t
  HLTGlobalStatus::size() const
  {
    auto ret = paths_.size();
    return ret;
  }

  void
  HLTGlobalStatus::reset()
  {
    unsigned const n = paths_.size();
    for (unsigned i = 0; i != n; ++i) {
      paths_[i].reset();
    }
  }

  bool
  HLTGlobalStatus::wasrun() const
  {
    auto const n = paths_.size();
    auto i = n;
    for (i = 0; i != n; ++i) {
      if (paths_[i].state() != hlt::Ready) {
        return true;
      }
    }
    return false;
  }

  bool
  HLTGlobalStatus::accept() const
  {
    bool at_least_one_ran = false;
    bool at_least_one_accepted = false;
    unsigned const n = paths_.size();
    if (n == 0) {
      return true;
    }
    auto i = n;
    for (i = 0; i != n; ++i) {
      auto const s(paths_[i].state());
      if (s != hlt::Ready) {
        at_least_one_ran = true;
        if (s == hlt::Pass) {
          at_least_one_accepted = true;
        }
      }
    }
    if (!at_least_one_ran) {
      return true;
    }
    return at_least_one_accepted;
  }

  bool
  HLTGlobalStatus::error() const
  {
    auto const n = paths_.size();
    for (unsigned i = 0U; i != n; ++i) {
      if (paths_[i].state() == hlt::Exception) {
        return true;
      }
    }
    return false;
  }

  const HLTPathStatus&
  HLTGlobalStatus::at(unsigned const i) const
  {
    auto const& ret = paths_.at(i);
    return ret;
  }

  HLTPathStatus&
  HLTGlobalStatus::at(unsigned const i)
  {
    auto& ret = paths_.at(i);
    return ret;
  }

  const HLTPathStatus& HLTGlobalStatus::operator[](unsigned const i) const
  {
    auto const& ret = paths_.at(i);
    return ret;
  }

  HLTPathStatus& HLTGlobalStatus::operator[](unsigned const i)
  {
    auto& ret = paths_.at(i);
    return ret;
  }

  bool
  HLTGlobalStatus::wasrun(unsigned const i) const
  {
    auto ret = paths_.at(i).wasrun();
    return ret;
  }

  bool
  HLTGlobalStatus::accept(unsigned const i) const
  {
    auto ret = paths_.at(i).accept();
    return ret;
  }

  bool
  HLTGlobalStatus::error(unsigned const i) const
  {
    auto ret = paths_.at(i).error();
    return ret;
  }

  hlt::HLTState
  HLTGlobalStatus::state(unsigned const i) const
  {
    auto ret = paths_.at(i).state();
    return ret;
  }

  unsigned
  HLTGlobalStatus::index(unsigned const i) const
  {
    auto ret = paths_.at(i).index();
    return ret;
  }

  void
  HLTGlobalStatus::reset(unsigned const i)
  {
    paths_.at(i).reset();
    atomic_thread_fence(memory_order_seq_cst);
  }

  ostream&
  operator<<(ostream& ost, const HLTGlobalStatus& hlt)
  {
    vector<string> text{"n"s, "1"s, "0"s, "e"s};
    unsigned const n(hlt.size());
    for (auto i = 0U; i < n; ++i) {
      ost << text.at(hlt.state(i));
    }
    return ost;
  }

} // namespace art
