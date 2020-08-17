//
//
// Read a .fcl file and form a parameter set object.
//

#include "toyExperiment/Utilities/ParameterSetFromFile.h"

#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/parse.h"

#include "cetlib/filepath_maker.h"

#include <iostream>

tex::ParameterSetFromFile::ParameterSetFromFile(std::string const& fileName)
  : _fileName(fileName)
{

  cet::filepath_lookup policy("FHICL_FILE_PATH");
  fhicl::intermediate_table tbl;
  fhicl::parse_document(_fileName, policy, tbl);
  fhicl::make_ParameterSet(tbl, _pSet);
}

void
tex::ParameterSetFromFile::printNames(std::ostream& out) const
{

  std::vector<std::string> const& names = _pSet.get_names();
  out << "\nParameter set read from file: " << _fileName << std::endl;
  out << "Number of names: " << names.size() << std::endl;
  for (std::vector<std::string>::const_iterator i = names.begin();
       i != names.end();
       ++i) {
    out << " " << i - names.begin() << " " << *i << std::endl;
  }
}
