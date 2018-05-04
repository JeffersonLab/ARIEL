// vim: set sw=2:

#include "canvas_root_io/Utilities/TypeTools.h"
#include "boost/algorithm/string.hpp"
#include "boost/thread/tss.hpp"
#include "canvas/Utilities/DebugMacros.h"
#include "canvas/Utilities/Exception.h"
#include "cetlib/container_algorithms.h"
#include "cetlib_except/demangle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "ESTLType.h"
#include "TBaseClass.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TList.h"
#include "TVirtualCollectionProxy.h"

#include <algorithm>
#include <iostream>
#include <memory>

using namespace cet;
using namespace std;
using art::root::TypeWithDict;

TypeWithDict
art::root::find_nested_type_named(string const& nested_type,
                                  TClass* const type_to_search)
{
  if (type_to_search == nullptr) {
    throw Exception(errors::NullPointerError, "find_nested_type_named: ")
      << "Null TClass pointer passed for type_to_search!\n";
  }
  TypeWithDict found_type(string(type_to_search->GetName()) +
                          "::" + nested_type);
  return found_type;
}

TypeWithDict
art::root::value_type_of(TClass* const t)
{
  TypeWithDict found_type;
  if (t == nullptr) {
    throw Exception(errors::NullPointerError, "value_type_of: ")
      << "Null TClass pointer passed for type!\n";
  }
  ROOT::ESTLType stlty = t->GetCollectionType();
  if ((stlty > ROOT::kNotSTL) && (stlty < ROOT::kSTLend)) {
    TVirtualCollectionProxy* vcp = t->GetCollectionProxy();
    TClass* vc = vcp->GetValueClass();
    if (vc != nullptr) {
      found_type = TypeWithDict(*vc->GetTypeInfo());
      return found_type;
    }
    auto vty = static_cast<int>(vcp->GetType());
    found_type = TypeWithDict(getTypeID(vty));
    return found_type;
  }
  // Look for "value_type" attribute in TClass.
  auto am = t->GetAttributeMap();
  if (am && am->HasKey("value_type")) {
    string vt_name(am->GetPropertyAsString("value_type"));
    found_type = TypeWithDict(vt_name);
  }
  return found_type;
}

TypeWithDict
art::root::mapped_type_of(TClass* const t)
{
  TypeWithDict found_type;
  if (t == nullptr) {
    throw Exception(errors::NullPointerError, "mapped_type_of: ")
      << "Null TClass pointer passed for type!\n";
  }
  ROOT::ESTLType stlty = t->GetCollectionType();
  if ((stlty == ROOT::kSTLmap) || (stlty == ROOT::kSTLmultimap) ||
      (stlty == ROOT::kSTLunorderedmap) ||
      (stlty == ROOT::kSTLunorderedmultimap)) {
    TVirtualCollectionProxy* vcp = t->GetCollectionProxy();
    TClass* vc = vcp->GetValueClass();
    if (vc != nullptr) {
      string vcname(vc->GetName());
      string mapped_type_name = name_of_template_arg(vcname, 1);
      found_type = TypeWithDict(mapped_type_name);
      return found_type;
    }
    // This should be impossible.
    throw Exception(errors::LogicError)
      << "ROOT map type did not have a value class!\n";
  }
  // Look for "mapped_type" attribute in TClass.
  auto am = t->GetAttributeMap();
  if (am && am->HasKey("mapped_type")) {
    string mt_name(am->GetPropertyAsString("mapped_type"));
    found_type = TypeWithDict(mt_name);
  }
  return found_type;
}

vector<TClass*>
art::root::public_base_classes(TClass* const cl)
{
  vector<TClass*> result;
  if (cl == nullptr) {
    throw Exception(errors::NullPointerError, "public_base_classes: ")
      << "Null TClass pointer passed!\n";
  }
  for (auto bobj : *cl->GetListOfBases()) {
    auto bb = dynamic_cast<TBaseClass*>(bobj);
    if (bb->Property() & kIsPublic) {
      result.push_back(bb->GetClassPointer());
    }
  }
  return result;
}

TypeWithDict
art::root::type_of_template_arg(TClass* template_instance,
                                size_t const desired_arg)
{
  if (template_instance == nullptr) {
    throw Exception(errors::NullPointerError, "type_of_template_arg: ")
      << "Null TClass pointer passed!\n";
  }
  return TypeWithDict{
    type_of_template_arg(template_instance->GetName(), desired_arg)};
}

bool
art::root::is_instantiation_of(TClass* const cl, string const& template_name)
{
  if (cl == nullptr) {
    throw Exception(errors::NullPointerError, "is_instantiation_of: ")
      << "Null TClass pointer passed!\n";
  }
  return art::is_instantiation_of(cl->GetName(), template_name);
}

void
art::root::throwLateDictionaryError(std::string const& className)
{
  throw Exception(errors::LogicError, "NoDictionary: ")
    << "Could not find dictionary for: " << className
    << "\ndespite passing runtime dictionary checks.\n";
}
