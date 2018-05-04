#ifndef messagefacility_Utilities_ErrorObj_h
#define messagefacility_Utilities_ErrorObj_h
// vim: set sw=2 expandtab :

#include "messagefacility/Utilities/ELextendedID.h"
#include "messagefacility/Utilities/ELseverityLevel.h"

#include <list>
#include <sstream>
#include <string>
#include <type_traits>

#include <sys/time.h>

namespace mf {

  class ErrorLog;

  // Note: This class is virtual because it is user-visible and customizable.
  class ErrorObj {

  public:
    virtual ~ErrorObj();
    ErrorObj() = delete;
    ErrorObj(ELseverityLevel const sev,
             std::string const& category,
             bool verbatim = false,
             std::string const& filename = "",
             int lineNumber = 0);
    ErrorObj(ErrorObj const&);

    int serial() const;
    ELextendedID const& xid() const;
    std::string const& idOverflow() const;
    timeval timestamp() const;
    std::list<std::string> const& items() const;
    bool reactedTo() const;

    // Return concatenated items, the full text of the message.
    std::string fullText() const;

    // Note: Obsolete! Remove me when user code migrated.
    std::string const& context() const;
    std::string const& iteration() const;
    bool is_verbatim() const;
    std::string const& filename() const;
    int lineNumber() const;

    // mine
    virtual void setTimestamp(timeval const&);
    // Note: Obsolete! Remove me when user code migrated.
    virtual void setContext(std::string const&);
    virtual void setIteration(std::string const&);

    // part of xid
    virtual void setSeverity(ELseverityLevel const);
    virtual void setID(std::string const&);
    virtual void setModule(std::string const&);
    virtual void setSubroutine(std::string const&);

    // Per-message overrides for the global settings, part of xid
    virtual void setHostName(std::string const&);
    virtual void setHostAddr(std::string const&);
    virtual void setApplication(std::string const&);
    virtual void setPID(long);

    ErrorObj& operator<<(std::ostream& (*f)(std::ostream&));
    ErrorObj& operator<<(std::ios_base& (*f)(std::ios_base&));

    // Explicit function for char const* to avoid unnecessary
    // instantiations of char const[] types.
    ErrorObj& opltlt(char const*);

    template <class T>
    ErrorObj& opltlt(T const&);

    virtual ErrorObj& eo_emit(std::string const&);

    virtual void clear();
    virtual void set(ELseverityLevel const, std::string const& id);
    virtual void setReactedTo(bool);

  private:
    int serial_{};
    ELextendedID xid_{};
    std::string idOverflow_{};
    timeval timestamp_{0, 0};
    std::list<std::string> items_{};
    bool reactedTo_{false};
    std::string iteration_{};
    std::ostringstream oss_{};
    bool verbatim_{false};
    std::string filename_{};
    int lineNumber_{};
  };

  template <class T>
  ErrorObj&
  ErrorObj::opltlt(T const& t)
  {
    oss_.str({});
    oss_ << t;
    if (!oss_.str().empty()) {
      eo_emit(oss_.str());
    }
    return *this;
  }

  //
  ErrorObj& operator<<(ErrorObj& e, char const* s);

  // Force conversion of char arrays to string.
  template <class T>
  ErrorObj&
  operator<<(ErrorObj& e, T const& t)
  {
    return e.opltlt(t);
  }

} // namespace mf

#endif /* messagefacility_Utilities_ErrorObj_h */

// Local variables:
// mode: c++
// End:
