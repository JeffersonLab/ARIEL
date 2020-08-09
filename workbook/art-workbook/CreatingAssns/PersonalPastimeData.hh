#ifndef art_workbook_CreatingAssns_PersonalPastimeData_hh
#define art_workbook_CreatingAssns_PersonalPastimeData_hh

#include <vector>

namespace awb {
  namespace detail {
    enum Proficiency { NA, BEGINNER, CASUAL, JOURNEYMAN, EXPERT };
  }
  using detail::Proficiency;
  struct PersonalPastimeData {
    unsigned time; // h/m.
    unsigned cost; // $/m.
    Proficiency proficiency; // Self-assessed.
  };
  typedef std::vector<PersonalPastimeData> PersonalPastimeDataSet;
}

#endif /* art_workbook_CreatingAssns_PersonalPastimeData_hh */
