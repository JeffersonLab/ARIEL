#include "canvas_root_io/Utilities/DictionaryChecker.h"
// vim: set sw=2 expandtab :

#include "canvas/Utilities/DebugMacros.h"
#include "canvas/Utilities/Exception.h"
#include "canvas_root_io/Utilities/TypeTools.h"
#include "canvas_root_io/Utilities/TypeWithDict.h"
#include "cetlib_except/demangle.h"
#include "hep_concurrency/RecursiveMutex.h"

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

using namespace std;

namespace {

  struct DictionaryAction {
    DictionaryAction(string name_stem, vector<size_t> args_to_check={})
      :
      name_stem(move(name_stem)),
      args_to_check(move(args_to_check))
      {
      }
    string name_stem;
    vector<size_t> args_to_check;
  };

  using NameActionTable = vector<DictionaryAction>;

  NameActionTable const noTopLevelDictionaryNames {
    { "art::BranchType"s },
    { "unique_ptr<"s, {0} },
    { "array<"s, {0} },
    { "string"s }
  };

  NameActionTable const noBaseCheckNames {
    { "bitset"s },
    { "pair<"s, {0,1} },
    { "deque<"s, {0} },
    { "forward_list<"s, {0} },
    { "list<"s, {0} },
    { "u16string"s }, // FIXME: Check ROOT's handling of this.
    { "u32string"s }, // FIXME: Check ROOT's handling of this.
    { "wstring"s }, // FIXME: Check ROOT's handling of this.
    { "basic_string<"s, {0} },
    { "vector<"s, {0} }, // FIXME: Should check Allocator.
    { "map<"s, {0,1} }, // FIXME: Should check Compare and Allocator.
    { "multimap<"s, {0,1} }, // FIXME: Should check Compare and Allocator.
      // FIXME: Should check Hash, Pred, and Allocator.
    { "unordered_map<"s, {0,1} },
      // FIXME: Should check Hash, Pred, and Allocator.
    { "unordered_multimap<"s, {0,1} },
    { "set<"s, {0} }, // FIXME: Should check Compare and Allocator.
    { "multiset<"s, {0} }, // FIXME: Should check Compare and Allocator.
      // FIXME: Should check Hash, Pred, and Allocator.
    { "unordered_set<"s, {0} },
      // FIXME: Should check Hash, Pred, and Allocator.
    { "unordered_multiset<"s, {0} },
    { "queue<"s, {0} }, // FIXME: Should check Container.
    { "priority_queue<"s, {0} }, // FIXME: Should check Container and Compare.
    { "stack<"s, {0} } // FIXME: Should check Container.
  };

  bool
  match_from_begin(string const& test, string const& ref)
  {
    return test.size() < ref.size() ? false :
      test.compare(0, ref.size(), ref) == 0;
  }

  bool
  match_from_end(string const& test, string const& ref)
  {
    return test.size() < ref.size() ?
                         false :
      test.compare(test.size() - ref.size(), ref.size(), ref) == 0;
  }

  void
  erase_if_match_from_begin(string& test, string const& ref)
  {
    if (match_from_begin(test, ref)) {
      test.erase(0, ref.size());
    }
  }

  void
  erase_if_match_from_end(string& test, string const& ref)
  {
    if (match_from_end(test, ref)) {
      test.erase(test.size() - ref.size());
    }
  }

  template<typename FUNC>
  bool
  dictionaryActionForName(string const & name,
                          NameActionTable const & actionTable,
                          FUNC dictionaryArgChecker) {
    bool result = false;
    auto const i =
      find_if(cbegin(actionTable),
              cend(actionTable),
              [&name](auto const &nameAction) {
                return match_from_begin(name, nameAction.name_stem);
              });
    if (i != cend(actionTable)) {
      for_each(cbegin(i->args_to_check),
               cend(i->args_to_check),
               dictionaryArgChecker);
      result = true;
    }
    return result;
  }
} // unnamed namespace

void
art::root::DictionaryChecker::checkDictionariesForArg_(string const& name,
                                                       size_t const index,
                                                       size_t const level)
{
  auto const arg = name_of_template_arg(name, index);
  if (arg.empty()) {
    throw Exception(errors::LogicError, "checkDictionaries: ")
      << "Could not get template arg #" << index
      << " from: " << name << '\n';
  }
  checkDictionaries(arg, true, level + 2);
}

void
art::root::DictionaryChecker::checkDictionaries(string const& name_orig,
                                                bool const recursive,
                                                size_t const level)
{
  string name;
  TClassEdit::GetNormalizedName(name, name_orig);
  erase_if_match_from_begin(name, "");       // Strip leading
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
  // Special cases for action on certain names are described in tables
  // in the unnamed namespace. This lambda wraps the appropriate extra
  // boilerplate required to allow the table reader to carry out checks
  // on template arguments if indicated.
  auto const doActionFromTable =
    [this, &name, level](auto const & actionTable) {
    return
    dictionaryActionForName(name,
                            actionTable,
                            [this, &name, level](size_t const index) {
                              checkDictionariesForArg_(name, index, level);
                            });
  };
  // Special cases.
  if (doActionFromTable(noTopLevelDictionaryNames)) {
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
    case TypeWithDict::Category::NONE:
      throw Exception(errors::LogicError, "checkDictionaries: ")
        << "Type category of name is NONE: " << name << '\n';
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
  // Classes that should not have bases checked.
  if (doActionFromTable(noBaseCheckNames)) {
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

vector<string>
art::root::DictionaryChecker::typesMissingDictionaries()
{
  hep::concurrency::RecursiveMutexSentry sentry{mutex_, __func__};
  vector<string> result;
  for (auto const& mt : missing_types_) {
    result.emplace_back(cet::demangle_symbol(mt));
  }
  set<string> tmp;
  swap(tmp, missing_types_);
  sort(result.begin(), result.end());
  return result;
}

void
art::root::DictionaryChecker::reportMissingDictionaries()
{
  hep::concurrency::RecursiveMutexSentry sentry{mutex_, __func__};
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
    << "\nMost likely they were never generated, but it may be that they were "
    "generated in the wrong package.\n\nPlease add (or move) the "
    "specification\n\n     <class name=\"MyClassName\"/>\n\nto the "
    "appropriate classes_def.xml file.\n\nAlso, if this class has any "
    "transient members,\nyou need to specify them in classes_def.xml.";
}
