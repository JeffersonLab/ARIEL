#include "messagefacility/Utilities/MessageFacilityMsg.h"
// vim: set sw=2 expandtab :

#include "messagefacility/Utilities/ErrorObj.h"
#include "messagefacility/Utilities/formatTime.h"

using namespace std;

namespace mf {

  MessageFacilityMsg::~MessageFacilityMsg() {}

  MessageFacilityMsg::MessageFacilityMsg(ErrorObj const& msg)
    : msg_(new ErrorObj(msg)), empty_(false)
  {}

  MessageFacilityMsg::MessageFacilityMsg()
    : msg_(new ErrorObj(ELseverityLevel("INFO"), "")), empty_(true)
  {}

  void
  MessageFacilityMsg::setTimestamp(timeval const& tv)
  {
    msg_->setTimestamp(tv);
    empty_ = false;
  }

  void
  MessageFacilityMsg::setSeverity(string const& severity)
  {
    msg_->setSeverity(mf::ELseverityLevel(severity));
    empty_ = false;
  }

  void
  MessageFacilityMsg::setCategory(string const& category)
  {
    msg_->setID(category);
    empty_ = false;
  }

  void
  MessageFacilityMsg::setHostname(string const& hostname)
  {
    msg_->setHostName(hostname);
    empty_ = false;
  }

  void
  MessageFacilityMsg::setHostaddr(string const& hostaddr)
  {
    msg_->setHostAddr(hostaddr);
    empty_ = false;
  }

  void
  MessageFacilityMsg::setPid(long const pid)
  {
    msg_->setPID(pid);
    empty_ = false;
  }

  void
  MessageFacilityMsg::setApplication(string const& app)
  {
    msg_->setApplication(app);
    empty_ = false;
  }

  void
  MessageFacilityMsg::setModule(string const& module)
  {
    msg_->setModule(module);
    empty_ = false;
  }

  void
  MessageFacilityMsg::setContext(string const& s)
  {
    msg_->setIteration(s);
    empty_ = false;
  }

  void
  MessageFacilityMsg::setIteration(string const& s)
  {
    msg_->setIteration(s);
    empty_ = false;
  }

  void
  MessageFacilityMsg::setMessage(string const& file,
                                 string const& line,
                                 string const& message)
  {
    *msg_ << " " << file << ":" << line << "\n" << message;
    empty_ = false;
  }

  bool
  MessageFacilityMsg::empty() const
  {
    return empty_;
  }

  ErrorObj
  MessageFacilityMsg::ErrorObject() const
  {
    return *msg_;
  }

  timeval
  MessageFacilityMsg::timestamp() const
  {
    return msg_->timestamp();
  }

  // FIXME: The get_time function is gone!
  string
  MessageFacilityMsg::timestr() const
  {
    return mf::timestamp::Legacy::get_time(msg_->timestamp());
  }

  string
  MessageFacilityMsg::severity() const
  {
    return msg_->xid().severity().getInputStr();
  }

  string
  MessageFacilityMsg::category() const
  {
    return msg_->xid().id();
  }

  string
  MessageFacilityMsg::hostname() const
  {
    return msg_->xid().hostname();
  }

  string
  MessageFacilityMsg::hostaddr() const
  {
    return msg_->xid().hostaddr();
  }

  long
  MessageFacilityMsg::pid() const
  {
    return msg_->xid().pid();
  }

  string
  MessageFacilityMsg::application() const
  {
    return msg_->xid().application();
  }

  string
  MessageFacilityMsg::module() const
  {
    return msg_->xid().module();
  }

  string
  MessageFacilityMsg::context() const
  {
    return msg_->iteration();
  }

  string
  MessageFacilityMsg::iteration() const
  {
    return msg_->iteration();
  }

  // FIXME: We don't put the file in the items any more!
  string
  MessageFacilityMsg::file() const
  {
    int idx = 0;
    for (list<string>::const_iterator it = msg_->items().begin();
         it != msg_->items().end();
         ++it) {
      ++idx;
      if (idx == 2) {
        return *it;
      }
    }
    return "";
  }

  // FIXME: We don't put the line in the items any more!
  long
  MessageFacilityMsg::line() const
  {
    int idx = 0;
    int line = 0;
    for (list<string>::const_iterator it = msg_->items().begin();
         it != msg_->items().end();
         ++it) {
      ++idx;
      if (idx == 4) {
        istringstream ss(*it);
        if (ss >> line) {
          return line;
        }
        return 0;
      }
    }
    return 0;
  }

  // FIXME: We don't put the file, line in the items any more!
  string
  MessageFacilityMsg::message() const
  {
    int idx = 0;
    string msg;
    for (list<string>::const_iterator it = msg_->items().begin();
         it != msg_->items().end();
         ++it) {
      ++idx;
      if (idx > 5) {
        msg += *it;
      }
    }
    return msg;
  }

} // namespace mf
