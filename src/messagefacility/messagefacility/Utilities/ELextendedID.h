#ifndef messagefacility_Utilities_ELextendedID_h
#define messagefacility_Utilities_ELextendedID_h
// vim: set sw=2 expandtab :

#include "messagefacility/Utilities/ELseverityLevel.h"

namespace mf {

  class ELextendedID {

  public:
    ~ELextendedID();
    ELextendedID();
    ELextendedID(std::string const& id,
                 ELseverityLevel severity,
                 std::string const& module,
                 std::string const& subroutine,
                 std::string const& hostname,
                 std::string const& hostaddr,
                 std::string const& application);

    std::string const& id() const;
    ELseverityLevel severity() const;
    std::string const& module() const;
    std::string const& subroutine() const;
    std::string const& hostname() const;
    std::string const& hostaddr() const;
    std::string const& application() const;
    long pid() const;

    void setID(std::string const& id);
    void setSeverity(ELseverityLevel severity);
    void setModule(std::string const& module);
    void setSubroutine(std::string const& subroutine);
    void setHostname(std::string const& hostname);
    void setHostaddr(std::string const& hostaddr);
    void setApplication(std::string const& application);
    void setPID(long pid);

    bool operator<(ELextendedID const& xid) const;
    void clear();

  private:
    std::string id_{};
    ELseverityLevel severity_{};
    std::string module_{};
    std::string subroutine_{};
    std::string hostname_{};
    std::string hostaddr_{};
    std::string application_{};
    long pid_{0};
  };

} // namespace mf

#endif /* messagefacility_Utilities_ELextendedID_h */

// Local variables:
// mode: c++
// End:
