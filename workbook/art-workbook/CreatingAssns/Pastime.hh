#ifndef art_workbook_CreatingAssns_Pastime_hh
#define art_workbook_CreatingAssns_Pastime_hh

#include <string>
#include <vector>

namespace awb {
  struct Pastime {
    std::string name;
    bool competitive;
    bool requiresSpecialistEquipment;
    bool indoor;
    bool team;
  };
  typedef std::vector<Pastime> Pastimes;
}
#endif /* art_workbook_CreatingAssns_Pastime_hh */

