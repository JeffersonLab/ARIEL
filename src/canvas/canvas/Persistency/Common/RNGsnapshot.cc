// ======================================================================
//
// RNGsnapshot
//
// ======================================================================

#include "canvas/Persistency/Common/RNGsnapshot.h"
#include "cetlib/container_algorithms.h"

using art::RNGsnapshot;

RNGsnapshot::RNGsnapshot(std::string const& ekind,
                         label_t const& lbl,
                         engine_state_t const& est)
{
  saveFrom(ekind, lbl, est);
}

void
RNGsnapshot::saveFrom(std::string const& ekind,
                      label_t const& lbl,
                      engine_state_t const& est)
{
  engine_kind_ = ekind;
  label_ = lbl;
  cet::copy_all(est, std::back_inserter(state_));
}

RNGsnapshot::engine_state_t
RNGsnapshot::restoreState() const
{
  engine_state_t est;
  cet::copy_all(state_, std::back_inserter(est));
  return est;
}

// ======================================================================
