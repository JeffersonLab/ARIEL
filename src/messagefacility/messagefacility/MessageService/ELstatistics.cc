#include "messagefacility/MessageService/ELstatistics.h"
// vim: set sw=2 expandtab :

#include "messagefacility/Utilities/ErrorObj.h"

namespace mf::service {

  ELstatistics::~ELstatistics() = default;

  ELstatistics::ELstatistics(Config const& config, cet::ostream_handle&& osh)
    : ELdestination{config.elDestConfig()}, osh_{std::move(osh)}
  {}

  ELstatistics::ELstatistics(Parameters const& pset, std::ostream& osp)
    : ELstatistics{pset(), cet::ostream_handle{osp}}
  {}

  void
  ELstatistics::log(ErrorObj& msg)
  {
    if (msg.xid().severity() < threshold_) {
      if (outputStatistics_) {
        statsMap_[msg.xid()].add(summarizeContext(msg.context()), false);
        updatedStats_ = true;
      }
      return;
    }
    if (skipMsg(msg.xid())) {
      if (outputStatistics_) {
        statsMap_[msg.xid()].add(summarizeContext(msg.context()), false);
        updatedStats_ = true;
      }
      return;
    }
    msg.setReactedTo(true);
    // The only real difference between a statistics destination
    // and an ordinary one is here, where we make no attempt to
    // output the message.
    if (outputStatistics_) {
      statsMap_[msg.xid()].add(summarizeContext(msg.context()), true);
      updatedStats_ = true;
    }
  }

  // Called only by MessageLoggerScribe::summarize()
  //   Called only by MessageLogger::LogStatistics()
  void
  ELstatistics::summary()
  {
    if (outputStatistics_ && updatedStats_) {
      osh_ << "\n=============================================\n\n"
           << "MessageLogger Summary\n"
           << formSummary();
    }
    updatedStats_ = false;
    if (reset_) {
      resetMsgCounters();
      resetLimiters();
    }
  }

} // namespace mf::service
