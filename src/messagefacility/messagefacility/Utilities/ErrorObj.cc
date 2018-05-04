#include "messagefacility/Utilities/ErrorObj.h"
// vim: set sw=2 expandtab :

#include <atomic>
#include <list>
#include <string>

using namespace std;

namespace {

  unsigned int const maxCategoryLength{200U};

} // unnamed namespace

namespace mf {

  static atomic<int> ourSerial{0};

  ErrorObj::~ErrorObj() {}

  ErrorObj::ErrorObj(ELseverityLevel const sev,
                     string const& category,
                     bool const verbatim,
                     string const& filename,
                     int lineNumber)
    : serial_{}
    , xid_{}
    , idOverflow_{}
    , timestamp_{0, 0}
    , items_{}
    , reactedTo_{false}
    , iteration_{}
    , oss_{}
    , verbatim_{verbatim}
    , filename_{filename}
    , lineNumber_{lineNumber}
  {
    set(sev, category);
  }

  ErrorObj::ErrorObj(ErrorObj const& orig)
    : serial_{orig.serial_}
    , xid_(orig.xid_)
    , idOverflow_{orig.idOverflow_}
    , timestamp_(orig.timestamp_)
    , items_{orig.items_}
    , reactedTo_{orig.reactedTo_}
    , verbatim_{orig.verbatim_}
    , filename_{orig.filename_}
    , lineNumber_{orig.lineNumber_}
  {}

  int
  ErrorObj::serial() const
  {
    return serial_;
  }

  ELextendedID const&
  ErrorObj::xid() const
  {
    return xid_;
  }

  string const&
  ErrorObj::idOverflow() const
  {
    return idOverflow_;
  }

  timeval
  ErrorObj::timestamp() const
  {
    return timestamp_;
  }

  std::list<std::string> const&
  ErrorObj::items() const
  {
    return items_;
  }

  bool
  ErrorObj::reactedTo() const
  {
    return reactedTo_;
  }

  bool
  ErrorObj::is_verbatim() const
  {
    return verbatim_;
  }

  // Note: Obsolete! Remove me when user code migrated.
  string const&
  ErrorObj::context() const
  {
    return iteration_;
  }

  string const&
  ErrorObj::iteration() const
  {
    return iteration_;
  }

  string
  ErrorObj::fullText() const
  {
    string result;
    for (auto const& text : items_) {
      result += text;
    }
    return result;
  }

  string const&
  ErrorObj::filename() const
  {
    return filename_;
  }

  int
  ErrorObj::lineNumber() const
  {
    return lineNumber_;
  }

  void
  ErrorObj::setSeverity(ELseverityLevel const sev)
  {
    xid_.setSeverity(
      (sev <= ELzeroSeverity) ?
        (ELseverityLevel)ELdebug :
        ((sev >= ELhighestSeverity) ? (ELseverityLevel)ELsevere : sev));
  }

  void
  ErrorObj::setID(string const& category)
  {
    xid_.setID(string(category, 0, maxCategoryLength));
    if (category.length() > maxCategoryLength) {
      idOverflow_ = string(
        category, maxCategoryLength, category.length() - maxCategoryLength);
    }
  }

  void
  ErrorObj::setModule(string const& module)
  {
    xid_.setModule(module);
  }

  // Note: Obsolete! Remove me when user code migrated.
  void
  ErrorObj::setContext(string const& s)
  {
    iteration_ = s;
  }

  void
  ErrorObj::setIteration(string const& s)
  {
    iteration_ = s;
  }

  void
  ErrorObj::setSubroutine(string const& subroutine)
  {
    xid_.setSubroutine((subroutine[0] == ' ') ? subroutine.substr(1) :
                                                subroutine);
  }

  void
  ErrorObj::setReactedTo(bool const r)
  {
    reactedTo_ = r;
  }

  void
  ErrorObj::setHostName(string const& hostname)
  {
    xid_.setHostname(hostname);
  }

  void
  ErrorObj::setHostAddr(string const& hostaddr)
  {
    xid_.setHostaddr(hostaddr);
  }

  void
  ErrorObj::setApplication(string const& application)
  {
    xid_.setApplication(application);
  }

  void
  ErrorObj::setPID(long const pid)
  {
    xid_.setPID(pid);
  }

  ErrorObj&
  ErrorObj::eo_emit(string const& s)
  {
    if (s.substr(0, 5) == "@SUB=") {
      setSubroutine(s.substr(5));
    } else {
      items_.push_back(s);
    }
    return *this;
  }

  void
  ErrorObj::clear()
  {
    serial_ = 0;
    xid_ = {};
    idOverflow_.clear();
    timestamp_ = {0, 0};
    items_.clear();
    reactedTo_ = false;
    iteration_.clear();
    oss_.str({});
    verbatim_ = false;
  }

  void
  ErrorObj::set(ELseverityLevel const sev, string const& category)
  {
    gettimeofday(&timestamp_, 0);
    serial_ = ++ourSerial;
    setID(category);
    setSeverity(sev);
  }

  void
  ErrorObj::setTimestamp(timeval const& t)
  {
    timestamp_ = t;
  }

  ErrorObj&
  ErrorObj::operator<<(std::ostream& (*f)(std::ostream&))
  {
    f(oss_);
    oss_.str({});
    return *this;
  }

  ErrorObj&
  ErrorObj::operator<<(std::ios_base& (*f)(std::ios_base&))
  {
    f(oss_);
    return *this;
  }

  // Force conversion of char arrays to string.
  ErrorObj&
  ErrorObj::opltlt(char const* s)
  {
    oss_.str({});
    oss_ << s;
    if (!oss_.str().empty()) {
      eo_emit(oss_.str());
    }
    return *this;
  }

  // Force conversion of char arrays to string.
  ErrorObj&
  operator<<(ErrorObj& e, char const* s)
  {
    return e.opltlt(s);
  }

} // namespace mf
