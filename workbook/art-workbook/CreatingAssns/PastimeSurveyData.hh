#ifndef art_workbook_CreatingAssns_PastimeSurveyData_hh
#define art_workbook_CreatingAssns_PastimeSurveyData_hh

#include "art-workbook/CreatingAssns/Pastime.hh"
#include "art-workbook/CreatingAssns/Person.hh"
#include "art-workbook/CreatingAssns/PersonalPastimeData.hh"
#include "canvas/Persistency/Common/Assns.h"

namespace awb {
  typedef art::Assns<Person, Pastime, PersonalPastimeData> PastimeSurveyData;
  // Convenience of ROOT persistency, see classes.h & classes_def.xml.
  typedef art::Assns<Person, Pastime> PastimeSurvey; // Base class.
  typedef art::Assns<Pastime, Person, PersonalPastimeData> RPastimeSurveyData;
  typedef art::Assns<Pastime, Person> RPastimeSurvey; // Base class.
}

#endif /* art_workbook_CreatingAssns_PastimeSurveyData_hh */
