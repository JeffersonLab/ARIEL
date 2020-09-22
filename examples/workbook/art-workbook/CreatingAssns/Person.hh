#ifndef art_workbook_CreatingAssns_Person_hh
#define art_workbook_CreatingAssns_Person_hh

#include <vector>

namespace awb {
  namespace detail {
    enum Gender { F, M };
  }
  using detail::Gender;
  struct Person {
    unsigned long long id;
    Gender gender;
    unsigned short age;
    unsigned short yearsOfEducation;
    unsigned grossIncome; // K$/y
  };
  typedef std::vector<Person> People;
}

#endif /* art_workbook_CreatingAssns_Person_hh */
