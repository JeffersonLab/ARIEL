#ifndef canvas_root_io_Utilities_DictionaryChecker_h
#define canvas_root_io_Utilities_DictionaryChecker_h

#include "canvas/Utilities/TypeID.h"

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

private:
  /// Reset missing types list.
  void resetMissingTypes_();
  void checkDictionariesForArg_(std::string const& template_name,
                                std::size_t index,
                                std::size_t level);

  std::set<std::string> checked_names_;
  std::set<std::string> missing_types_;
};

template <typename T>
void
art::root::DictionaryChecker::checkDictionaries(bool const recursive,
                                                std::size_t const level)
{
  checkDictionaries(TypeID{typeid(T)}.className(), recursive, level);
}

inline void
art::root::DictionaryChecker::resetMissingTypes_()
{
  using std::swap;
  std::set<std::string> tmp;
  swap(tmp, missing_types_);
}
#endif /* canvas_root_io_Utilities_DictionaryChecker_h */

// Local Variables:
// mode: c++
// End:
