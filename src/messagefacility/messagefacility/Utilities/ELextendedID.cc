#include "messagefacility/Utilities/ELextendedID.h"
// vim: set sw=2 expandtab :

namespace mf {

  ELextendedID::~ELextendedID() {}

  ELextendedID::ELextendedID() {}

  ELextendedID::ELextendedID(std::string const& id,
                             ELseverityLevel severity,
                             std::string const& module,
                             std::string const& subroutine,
                             std::string const& hostname,
                             std::string const& hostaddr,
                             std::string const& application)
    : id_(id)
    , severity_(severity)
    , module_(module)
    , subroutine_(subroutine)
    , hostname_(hostname)
    , hostaddr_(hostaddr)
    , application_(application)
  {}

  std::string const&
  ELextendedID::id() const
  {
    return id_;
  }

  ELseverityLevel
  ELextendedID::severity() const
  {
    return severity_;
  }

  std::string const&
  ELextendedID::module() const
  {
    return module_;
  }

  std::string const&
  ELextendedID::subroutine() const
  {
    return subroutine_;
  }

  std::string const&
  ELextendedID::hostname() const
  {
    return hostname_;
  }

  std::string const&
  ELextendedID::hostaddr() const
  {
    return hostaddr_;
  }

  std::string const&
  ELextendedID::application() const
  {
    return application_;
  }

  long
  ELextendedID::pid() const
  {
    return pid_;
  }

  void
  ELextendedID::setID(std::string const& id)
  {
    id_ = id;
  }

  void
  ELextendedID::setSeverity(ELseverityLevel severity)
  {
    severity_ = severity;
  }

  void
  ELextendedID::setModule(std::string const& module)
  {
    module_ = module;
  }

  void
  ELextendedID::setSubroutine(std::string const& subroutine)
  {
    subroutine_ = subroutine;
  }

  void
  ELextendedID::setHostname(std::string const& hostname)
  {
    hostname_ = hostname;
  }

  void
  ELextendedID::setHostaddr(std::string const& hostaddr)
  {
    hostaddr_ = hostaddr;
  }

  void
  ELextendedID::setApplication(std::string const& application)
  {
    application_ = application;
  }

  void
  ELextendedID::setPID(long pid)
  {
    pid_ = pid;
  }

  bool
  ELextendedID::operator<(ELextendedID const& xid) const
  {
    return (severity_ < xid.severity_) ?
             true :
             (severity_ > xid.severity_) ?
             false :
             (id_ < xid.id_) ?
             true :
             (id_ > xid.id_) ? false :
                               (module_ < xid.module_) ?
                               true :
                               (module_ > xid.module_) ?
                               false :
                               (subroutine_ < xid.subroutine_) ?
                               true :
                               (subroutine_ > xid.subroutine_) ? false : false;
  }

  void
  ELextendedID::clear()
  {
    *this = ELextendedID();
  }

} // end of namespace mf */
