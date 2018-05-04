#include "fhiclcpp/DatabaseSupport.h"
#include "fhiclcpp/ParameterSetRegistry.h"

void
fhicl::decompose_fhicl(std::string const& filename,
                       std::vector<std::string>& records,
                       std::vector<std::string>& hashes)
{
  // Parse file into a ParameterSet.
  fhicl::ParameterSet top;
  cet::filepath_maker fpm;
  make_ParameterSet(filename, fpm, top);
  decompose_parameterset(top, records, hashes);
}

void
fhicl::decompose_parameterset(fhicl::ParameterSet const& top,
                              std::vector<std::string>& records,
                              std::vector<std::string>& hashes)
{
  assert(records.size() == hashes.size());

  // First handle the top-level ParameterSet
  records.push_back(top.to_compact_string());
  hashes.push_back(top.id().to_string());

  // Recurse through all parameters, dealing with ParameterSets and
  // vectors thereof.
  std::vector<std::string> keys = top.get_names();
  for (auto const& key : keys) {
    if (top.is_key_to_table(key))
      decompose_parameterset(top.get<ParameterSet>(key), records, hashes);
    else if (top.is_key_to_sequence(key)) {
      try {
        auto nestlings = top.get<std::vector<ParameterSet>>(key);
        for (auto const& ps : nestlings)
          decompose_parameterset(ps, records, hashes);
      }
      catch (fhicl::exception const&) {
        // The vector didn't contain ParameterSets, keep going; no
        // corrective action is needed.
      }
    }
  }
}

void
fhicl::parse_file_and_fill_db(std::string const& filename, sqlite3* out)
{
  fhicl::ParameterSet top;
  cet::filepath_maker fpm;
  make_ParameterSet(filename, fpm, top);
  fhicl::ParameterSetRegistry::put(top);
  fhicl::ParameterSetRegistry::exportTo(out);
}
