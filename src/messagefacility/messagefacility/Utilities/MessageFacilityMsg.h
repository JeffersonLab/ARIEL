#ifndef messagefacility_Utilities_MessageFacilityMsg_h
#define messagefacility_Utilities_MessageFacilityMsg_h
// vim: set sw=2 expandtab :

#include <memory>
#include <string>
#include <sys/time.h>

namespace mf {

  class ErrorObj;

  class MessageFacilityMsg {

  public:
    ~MessageFacilityMsg();
    MessageFacilityMsg(ErrorObj const& errorobj);
    MessageFacilityMsg();

  public:
    bool empty() const;
    ErrorObj ErrorObject() const;
    timeval timestamp() const;
    std::string timestr() const;
    std::string severity() const;
    std::string category() const;
    std::string hostname() const;
    std::string hostaddr() const;
    long pid() const;
    std::string application() const;
    std::string module() const;
    std::string context() const;
    std::string iteration() const;
    std::string file() const;
    long line() const;
    std::string message() const;

    void setTimestamp(timeval const&);
    void setSeverity(std::string const&);
    void setCategory(std::string const&);
    void setHostname(std::string const&);
    void setHostaddr(std::string const&);
    void setPid(long);
    void setApplication(std::string const&);
    void setModule(std::string const&);
    void setContext(std::string const&);
    void setIteration(std::string const&);
    void setMessage(std::string const& file,
                    std::string const& line,
                    std::string const& message);

  private:
    std::unique_ptr<ErrorObj> msg_;
    bool empty_;
  };

} // namespace mf

#endif /* messagefacility_Utilities_MessageFacilityMsg_h */

// Local Variables:
// mode: c++
// End:
