#include "messagefacility/Utilities/ELseverityLevel.h"
// vim: set sw=2 expandtab :

#include <map>
#include <ostream>
#include <string>

using namespace std;

namespace {

  template <mf::ELseverityLevel(F)()>
  void
  setSeverity(map<string const, mf::ELseverityLevel::ELsev_>& m,
              mf::ELslProxy<F> const proxy)
  {
    auto const severity =
      static_cast<mf::ELseverityLevel::ELsev_>(proxy.getLevel());
    m[proxy.getSymbol()] = severity;
    m[proxy.getName()] = severity;
    m[proxy.getInputStr()] = severity;
    m[proxy.getVarName()] = severity;
  }

} // unnamed namespace

namespace mf {

  namespace {

    map<string const, mf::ELseverityLevel::ELsev_> const&
    loadMap()
    {
      static map<string const, mf::ELseverityLevel::ELsev_> m;
      setSeverity(m, ELzeroSeverity);
      setSeverity(m, ELsuccess);
      setSeverity(m, ELdebug);
      setSeverity(m, ELinfo);
      setSeverity(m, ELwarning);
      setSeverity(m, ELerror);
      setSeverity(m, ELunspecified);
      setSeverity(m, ELsevere);
      setSeverity(m, ELhighestSeverity);
      return m;
    }

  } // unnamed namespace

  ELseverityLevel::ELseverityLevel(string const& s)
  {
    static map<string const, mf::ELseverityLevel::ELsev_> const& m = loadMap();
    auto i = m.find(s);
    myLevel = (i == m.end()) ? ELsev_unspecified : i->second;
  }

  int
  ELseverityLevel::cmp(ELseverityLevel const e) const
  {
    return myLevel - e.myLevel;
  }

  int
  ELseverityLevel::getLevel() const
  {
    return myLevel;
  }

  string
  ELseverityLevel::getSymbol() const
  {
    string result;
    switch (myLevel) {
      default:
        result = "0";
        break;
      case ELsev_zeroSeverity:
        result = "--";
        break;
      case ELsev_success:
        result = "-d";
        break;
      case ELsev_info:
        result = "-i";
        break;
      case ELsev_warning:
        result = "-w";
        break;
      case ELsev_error:
        result = "-e";
        break;
      case ELsev_unspecified:
        result = "??";
        break;
      case ELsev_severe:
        result = "-s";
        break;
      case ELsev_highestSeverity:
        result = "!!";
        break;
    }
    return result;
  }

  string
  ELseverityLevel::getName() const
  {
    string result;
    switch (myLevel) {
      default:
        result = "?no value?";
        break;
      case ELsev_zeroSeverity:
        result = "--";
        break;
      case ELsev_success:
        result = "Debug";
        break;
      case ELsev_info:
        result = "Info";
        break;
      case ELsev_warning:
        result = "Warning";
        break;
      case ELsev_error:
        result = "Error";
        break;
      case ELsev_unspecified:
        result = "??";
        break;
      case ELsev_severe:
        result = "System";
        break;
      case ELsev_highestSeverity:
        result = "!!";
        break;
    }
    return result;
  }

  string
  ELseverityLevel::getInputStr() const
  {
    string result;
    switch (myLevel) {
      default:
        result = "?no value?";
        break;
      case ELsev_zeroSeverity:
        result = "ZERO";
        break;
      case ELsev_success:
        result = "DEBUG";
        break;
      case ELsev_info:
        result = "INFO";
        break;
      case ELsev_warning:
        result = "WARNING";
        break;
      case ELsev_error:
        result = "ERROR";
        break;
      case ELsev_unspecified:
        result = "UNSPECIFIED";
        break;
      case ELsev_severe:
        result = "SYSTEM";
        break;
      case ELsev_highestSeverity:
        result = "HIGHEST";
        break;
    }
    return result;
  }

  string
  ELseverityLevel::getVarName() const
  {
    string result;
    switch (myLevel) {
      default:
        result = "?no value?       ";
        break;
      case ELsev_zeroSeverity:
        result = "ELzeroSeverity   ";
        break;
      case ELsev_success:
        result = "ELdebug          ";
        break;
      case ELsev_info:
        result = "ELinfo           ";
        break;
      case ELsev_warning:
        result = "ELwarning        ";
        break;
      case ELsev_error:
        result = "ELerror          ";
        break;
      case ELsev_unspecified:
        result = "ELunspecified    ";
        break;
      case ELsev_severe:
        result = "ELsystem         ";
        break;
      case ELsev_highestSeverity:
        result = "ELhighestSeverity";
        break;
    }
    return result;
  }

  ostream&
  operator<<(ostream& os, ELseverityLevel const sev)
  {
    return os << " -" << sev.getName() << "- ";
  }

} // namespace mf
