#ifndef messagefacility_Utilities_ELseverityLevel_h
#define messagefacility_Utilities_ELseverityLevel_h
// vim: set sw=2 expandtab :

#include <string>

namespace mf {

  class ELseverityLevel;

  using ELslGen = ELseverityLevel();

  template <ELslGen ELgen>
  struct ELslProxy {

    operator ELseverityLevel() const;

    int getLevel() const;
    std::string getSymbol() const;
    std::string getName() const;
    std::string getInputStr() const;
    std::string getVarName() const;
  };

  class ELseverityLevel {

  public:
    enum ELsev_ {
      ELsev_noValueAssigned, // 0, not for messages
      ELsev_zeroSeverity,    // 1, threshold use only, not for messages
      ELsev_success,         // 2
      ELsev_info,            // 3
      ELsev_warning,         // 4
      ELsev_error,           // 5
      ELsev_unspecified,     // 6, not for messages
      ELsev_severe,          // 7
      ELsev_highestSeverity, // 8, for threshold use only, not for messages
      nLevels                // 9, not for messages
    };

    constexpr ELseverityLevel(ELsev_ lev = ELsev_unspecified);

    // str may match getSymbol, getName, getInputStr, or getVarName
    // see accessors
    ELseverityLevel(std::string const& str);

    int cmp(ELseverityLevel e) const;

    int getLevel() const;
    std::string getSymbol() const;   // example: "-e"
    std::string getName() const;     // example: "Error"
    std::string getInputStr() const; // example: "ERROR"
    std::string getVarName() const;  // example: "ELerror"

    friend std::ostream& operator<<(std::ostream& os,
                                    ELseverityLevel const sev);

  private:
    int myLevel;
  };

  inline constexpr ELseverityLevel::ELseverityLevel(ELsev_ const level)
    : myLevel{level}
  {}

  inline ELseverityLevel constexpr ELzeroSeverityGen()
  {
    return ELseverityLevel::ELsev_zeroSeverity;
  }

  inline ELseverityLevel constexpr ELsuccessGen()
  {
    return ELseverityLevel::ELsev_success;
  }

  inline ELseverityLevel constexpr ELdebugGen()
  {
    return ELseverityLevel::ELsev_success;
  }

  inline ELseverityLevel constexpr ELinfoGen()
  {
    return ELseverityLevel::ELsev_info;
  }

  inline ELseverityLevel constexpr ELwarningGen()
  {
    return ELseverityLevel::ELsev_warning;
  }

  inline ELseverityLevel constexpr ELerrorGen()
  {
    return ELseverityLevel::ELsev_error;
  }

  inline ELseverityLevel constexpr ELunspecifiedGen()
  {
    return ELseverityLevel::ELsev_unspecified;
  }

  inline ELseverityLevel constexpr ELsevereGen()
  {
    return ELseverityLevel::ELsev_severe;
  }

  inline ELseverityLevel constexpr ELhighestSeverityGen()
  {
    return ELseverityLevel::ELsev_highestSeverity;
  }

  ELslProxy<ELzeroSeverityGen> constexpr ELzeroSeverity{};
  ELslProxy<ELdebugGen> constexpr ELdebug{};
  ELslProxy<ELsuccessGen> constexpr ELsuccess{};
  ELslProxy<ELinfoGen> constexpr ELinfo{};
  ELslProxy<ELwarningGen> constexpr ELwarning{};
  ELslProxy<ELerrorGen> constexpr ELerror{};
  ELslProxy<ELunspecifiedGen> constexpr ELunspecified{};
  ELslProxy<ELsevereGen> constexpr ELsevere{};
  ELslProxy<ELhighestSeverityGen> constexpr ELhighestSeverity{};

  extern bool operator==(ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator!=(ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator<(ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator<=(ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator>(ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator>=(ELseverityLevel e1, ELseverityLevel e2);

  template <ELslGen ELgen>
  inline ELslProxy<ELgen>::operator ELseverityLevel() const
  {
    return ELgen();
  }

  template <ELslGen ELgen>
  inline int
  ELslProxy<ELgen>::getLevel() const
  {
    return ELgen().getLevel();
  }

  template <ELslGen ELgen>
  inline std::string
  ELslProxy<ELgen>::getSymbol() const
  {
    return ELgen().getSymbol();
  }

  template <ELslGen ELgen>
  inline std::string
  ELslProxy<ELgen>::getName() const
  {
    return ELgen().getName();
  }

  template <ELslGen ELgen>
  inline std::string
  ELslProxy<ELgen>::getInputStr() const
  {
    return ELgen().getInputStr();
  }

  template <ELslGen ELgen>
  inline std::string
  ELslProxy<ELgen>::getVarName() const
  {
    return ELgen().getVarName();
  }

  inline bool
  operator==(ELseverityLevel const e1, ELseverityLevel const e2)
  {
    return e1.cmp(e2) == 0;
  }

  inline bool
  operator!=(ELseverityLevel const e1, ELseverityLevel const e2)
  {
    return e1.cmp(e2) != 0;
  }

  inline bool
  operator<(ELseverityLevel const e1, ELseverityLevel const e2)
  {
    return e1.cmp(e2) < 0;
  }

  inline bool
  operator<=(ELseverityLevel const e1, ELseverityLevel const e2)
  {
    return e1.cmp(e2) <= 0;
  }

  inline bool
  operator>(ELseverityLevel const e1, ELseverityLevel const e2)
  {
    return e1.cmp(e2) > 0;
  }

  inline bool
  operator>=(ELseverityLevel const e1, ELseverityLevel const e2)
  {
    return e1.cmp(e2) >= 0;
  }

} // namespace mf

#endif /* messagefacility_Utilities_ELseverityLevel_h */

// Local variables:
// mode: c++
// End:
