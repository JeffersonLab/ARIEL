#include "canvas_root_io/Utilities/DictionaryChecker.h"
#include "canvas/Utilities/DebugMacros.h"
#include "canvas/Utilities/Exception.h"
#include "canvas_root_io/Utilities/TypeTools.h"
#include "canvas_root_io/Utilities/TypeWithDict.h"
#include "cetlib_except/demangle.h"

#include "TBaseClass.h"
#include "TClass.h"
#include "TClassEdit.h"
#include "TDataMember.h"
#include "TDataType.h"
#include "TDictAttributeMap.h"
#include "TEnum.h"
#include "TList.h"

#include <algorithm>
#include <regex>
#include <sstream>

namespace {

  bool
  match_from_begin(std::string const& test, std::string const& ref)
  {
    return test.size() < ref.size() ? false :
                                      test.compare(0, ref.size(), ref) == 0;
  }

  bool
  match_from_end(std::string const& test, std::string const& ref)
  {
    return test.size() < ref.size() ?
             false :
             test.compare(test.size() - ref.size(), ref.size(), ref) == 0;
  }

  void
  erase_if_match_from_begin(std::string& test, std::string const& ref)
  {
    if (match_from_begin(test, ref)) {
      test.erase(0, ref.size());
    }
  }

  void
  erase_if_match_from_end(std::string& test, std::string const& ref)
  {
    if (match_from_end(test, ref)) {
      test.erase(test.size() - ref.size());
    }
  }
}

void
art::root::DictionaryChecker::checkDictionaries(std::string const& name_orig,
                                                bool const recursive,
                                                std::size_t const level)
{
  using namespace std;
  string name;
  TClassEdit::GetNormalizedName(name, name_orig);

  erase_if_match_from_begin(name, "std::");  // Strip leading std::
  erase_if_match_from_begin(name, "const "); // Strip leading const
  erase_if_match_from_end(name, "const ");   // Strip trailing const
  erase_if_match_from_end(name, "&&");       // Strip trailing r-value reference
  erase_if_match_from_end(name, "&");        // Strip trailing l-value reference

  // FIXME: What about volatile & restrict?

  // Strip trailing *.
  {
    auto pos = name.size();
    while ((pos > 0) && (name[pos - 1] == '*')) {
      --pos;
    }
    if (pos == 0) {
      // Name was all '*'.
      return;
    }
    name.erase(pos);
  }

  if (name.empty()) {
    return;
  }

  // Special cases
  // -- enumerations are awkward in ROOT; they do not require
  //    definitions
  if (match_from_begin(name, "art::BranchType")) {
    return;
  }

  // Cases that ROOT transparently supports
  if (match_from_begin(name, "unique_ptr<")) {
    checkDictionariesForArg_(name, 0, level);
    return;
  }
  if (match_from_begin(name, "array<")) {
    checkDictionariesForArg_(name, 0, level);
    return;
  }

  {
    auto I = checked_names_.find(name);
    if (I != checked_names_.end()) {
      // Already checked this name.
      return;
    }
    checked_names_.insert(name);
  }
  if (name == "void") {
    return;
  }

  if (match_from_end(name, "::(anonymous)")) {
    return;
  }

  TypeWithDict ty{name};
  if (ty) {
    switch (ty.category()) {
      case TypeWithDict::Category::NONE: {
        throw Exception(errors::LogicError, "checkDictionaries: ")
          << "Type category of name is NONE: " << name << '\n';
      }
      case TypeWithDict::Category::CLASSTYPE:
        break; // Continue below.
      case TypeWithDict::Category::ENUMTYPE:
        return;
      case TypeWithDict::Category::BASICTYPE:
        return;
    }
  }

  auto cl = TClass::GetClass(name.c_str());
  if (cl == nullptr) {
    missing_types_.insert(name);
    // Note: The rest of the code assumes cl is not nullptr.
    return;
  }
  if (!TClass::HasDictionarySelection(name.c_str())) {
    missing_types_.insert(name);
  }
  {
    auto am = cl->GetAttributeMap();
    if (am && am->HasKey("persistent") &&
        am->GetPropertyAsString("persistent") == string("false")) {
      // Marked transient in the selection xml.
      return;
    }
    if (am && am->HasKey("transient") &&
        am->GetPropertyAsString("transient") == string("true")) {
      // Marked transient in the selection xml.
      return;
    }
  }
  {
    static regex const reNoSplit{"^(art::PtrVector(<|Base$)|art::Assns<)"};
    if (regex_search(name, reNoSplit)) {
      FDEBUG(1) << "Setting NoSplit on class " << name << "\n";
      cl->SetCanSplit(0);
    }
  }
  if (!recursive) {
    return;
  }

  if (match_from_begin(name, "array<")) {
    checkDictionariesForArg_(name, 0, level);
    return;
  }
  if (match_from_begin(name, "deque<")) {
    checkDictionariesForArg_(name, 0, level);
    return;
  }
  if (match_from_begin(name, "forward_list<")) {
    checkDictionariesForArg_(name, 0, level);
    return;
  }
  if (match_from_begin(name, "list<")) {
    checkDictionariesForArg_(name, 0, level);
    return;
  }
  if (match_from_begin(name, "string")) {
    // Ignore, root has special handling for this.
    return;
  }
  if (match_from_begin(name, "u16string")) {
    // Ignore, root has special handling for this.
    // FIXME: It does not!
    return;
  }
  if (match_from_begin(name, "u32string")) {
    // Ignore, root has special handling for this.
    // FIXME: It does not!
    return;
  }
  if (match_from_begin(name, "wstring")) {
    // Ignore, root has special handling for this.
    // FIXME: It does not!
    return;
  }
  if (match_from_begin(name, "basic_string<")) {
    checkDictionariesForArg_(name, 0, level);
    return;
  }
  if (match_from_begin(name, "vector<")) {
    checkDictionariesForArg_(name, 0, level);
    return;
  }
  if (match_from_begin(name, "map<")) {
    checkDictionariesForArg_(name, 0, level);
    checkDictionariesForArg_(name, 1, level);
    // FIXME: Should check Compare, and Allocator too!
    return;
  }
  if (match_from_begin(name, "multimap<")) {
    checkDictionariesForArg_(name, 0, level);
    checkDictionariesForArg_(name, 1, level);
    // FIXME: Should check Compare, and Allocator too!
    return;
  }
  if (match_from_begin(name, "unordered_map<")) {
    checkDictionariesForArg_(name, 0, level);
    checkDictionariesForArg_(name, 1, level);
    // FIXME: Should check Hash, Pred, and Allocator too!
    return;
  }
  if (match_from_begin(name, "unordered_multimap<")) {
    checkDictionariesForArg_(name, 0, level);
    checkDictionariesForArg_(name, 1, level);
    // FIXME: Should check Hash, Pred, and Allocator too!
    return;
  }
  if (match_from_begin(name, "set<")) {
    checkDictionariesForArg_(name, 0, level);
    // FIXME: Should check Compare, and Allocator too!
    return;
  }
  if (match_from_begin(name, "multiset<")) {
    checkDictionariesForArg_(name, 0, level);
    // FIXME: Should check Compare, and Allocator too!
    return;
  }
  if (match_from_begin(name, "unordered_set<")) {
    checkDictionariesForArg_(name, 0, level);
    // FIXME: Should check Hash, Pred, and Allocator too!
    return;
  }
  if (match_from_begin(name, "unordered_multiset<")) {
    checkDictionariesForArg_(name, 0, level);
    // FIXME: Should check Hash, Pred, and Allocator too!
    return;
  }
  if (match_from_begin(name, "queue<")) {
    checkDictionariesForArg_(name, 0, level);
    // FIXME: Should check Container too!
    return;
  }
  if (match_from_begin(name, "priority_queue<")) {
    checkDictionariesForArg_(name, 0, level);
    // FIXME: Should check Container, and Compare too!
    return;
  }
  if (match_from_begin(name, "stack<")) {
    checkDictionariesForArg_(name, 0, level);
    // FIXME: Should check Container too!
    return;
  }

  // Check dictionaries for base classes
  auto* bases = cl->GetListOfBases();
  if (bases == nullptr) {
    throw Exception(errors::LogicError, "checkDictionaries: ")
      << "Retrieving list of base classes for type '" << name
      << "' returned a nullptr.";
  }
  for (auto obj : *bases) {
    auto bc = dynamic_cast<TBaseClass*>(obj);
    if (bc == nullptr) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
        << "A pointer to one of the base classes of a class of type '" << name
        << "'"
        << "could not be cast to a TBaseClass pointer.";
    }
    checkDictionaries(bc->GetName(), true, level + 2);
  }

  // Check dictionaries for data members
  auto* data_members = cl->GetListOfDataMembers();
  if (data_members == nullptr) {
    throw Exception(errors::LogicError, "checkDictionaries: ")
      << "Retrieving list of data members for type '" << name
      << "' returned a nullptr.";
  }

  for (auto obj : *data_members) {
    auto dm = dynamic_cast<TDataMember*>(obj);
    if (dm == nullptr) {
      throw Exception(errors::LogicError, "checkDictionaries: ")
        << "A pointer to one of the data members contained by a class of type '"
        << name << "'"
        << "could not be cast to a TDataMember pointer.";
    }

    if (!dm->IsPersistent()) {
      // The data member comment in the header file starts with '!'.
      continue;
    }
    if (dm->Property() & kIsStatic) {
      // Static data member.
      continue;
    }
    auto am = dm->GetAttributeMap();
    if (am && am->HasKey("persistent") &&
        am->GetPropertyAsString("persistent") == string("false")) {
      // Marked transient in the selection xml.
      continue;
    }
    if (am && am->HasKey("transient") &&
        am->GetPropertyAsString("transient") == string("true")) {
      // Marked transient in the selection xml.
      continue;
    }
    if (am && am->HasKey("comment") &&
        (am->GetPropertyAsString("comment")[0] == '!')) {
      // Marked transient in the selection xml.
      continue;
    }
    checkDictionaries(dm->GetTrueTypeName(), true, level + 2);
  }
}

std::vector<std::string>
art::root::DictionaryChecker::typesMissingDictionaries()
{
  std::vector<std::string> result;
  for (auto const& mt : missing_types_) {
    result.emplace_back(cet::demangle_symbol(mt));
  }
  resetMissingTypes_();
  std::sort(result.begin(), result.end());
  return result;
}

void
art::root::DictionaryChecker::reportMissingDictionaries()
{
  using namespace std;
  if (missing_types_.empty()) {
    return;
  }
  ostringstream ostr;
  for (auto const& mt : typesMissingDictionaries()) {
    ostr << "     " << mt << "\n";
  }
  throw Exception(errors::DictionaryNotFound)
    << "No dictionary found for the following classes:\n\n"
    << ostr.str()
    << "\nMost likely they were never generated, but it may be that they "
       "were generated in the wrong package.\n"
       "\n"
       "Please add (or move) the specification\n"
       "\n"
       "     <class name=\"MyClassName\"/>\n"
       "\n"
       "to the appropriate classes_def.xml file.\n"
       "\n"
       "Also, if this class has any transient members,\n"
       "you need to specify them in classes_def.xml.";
}

void
art::root::DictionaryChecker::checkDictionariesForArg_(std::string const& name,
                                                       std::size_t const index,
                                                       std::size_t const level)
{
  auto const arg = name_of_template_arg(name, index);
  if (arg.empty()) {
    throw Exception(errors::LogicError, "checkDictionaries: ")
      << "Could not get " << (index == 0 ? "first" : "second")
      << " template arg from: " << name << '\n';
  }
  checkDictionaries(arg, true, level + 2);
}
