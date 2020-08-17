#ifndef canvas_root_io_Utilities_DictionaryChecker_h
#define canvas_root_io_Utilities_DictionaryChecker_h
// vim: set sw=2 expandtab :

#include "canvas/Utilities/TypeID.h"
#include "hep_concurrency/RecursiveMutex.h"

#include <set>
#include <string>
#include <vector>

namespace art {
  namespace root {
    class DictionaryChecker;
  }
}

/// Manage recursive checking of dictionary information for data products.
class art::root::DictionaryChecker {

public:
  /// Check dictionaries (optionally recursively) for named type.
  void checkDictionaries(std::string const& name_orig,
                         bool recursive = false,
                         std::size_t level = 0);
  template <typename T>
  void checkDictionaries(bool recursive = false, std::size_t level = 0);

  /// Return the sequence of (demangled) types missing dictionaries.
  std::vector<std::string> typesMissingDictionaries();

  /// Report (throwing exception) on all missing dictionaries.
  void reportMissingDictionaries();

  // Member Functions -- Implementation details.
private:
  void checkDictionariesForArg_(std::string const& template_name,
                                std::size_t index,
                                std::size_t level);

  // Member Data -- Implementation details.
private:
  // Protects all data members.
  mutable hep::concurrency::RecursiveMutex mutex_{"DictionaryChecker::mutex_"};
  std::set<std::string> checked_names_;
  std::set<std::string> missing_types_;
};

template <typename T>
void
art::root::DictionaryChecker::checkDictionaries(bool const recursive,
                                                std::size_t const level)
{
  hep::concurrency::RecursiveMutexSentry sentry{mutex_, __func__};
  checkDictionaries(TypeID{typeid(T)}.className(), recursive, level);
}

#endif /* canvas_root_io_Utilities_DictionaryChecker_h */

// Local Variables:
// mode: c++
// End:
