#include "canvas/Persistency/Common/detail/maybeCastObj.h"
// vim: set sw=2:

#include "canvas/Utilities/Exception.h"
#include "cetlib_except/demangle.h"

#include <cxxabi.h>

#ifdef _LIBCPPABI_VERSION
#include "TClass.h"
#include "TClassRef.h"
#endif

#include <iomanip>
#include <iostream>
#include <typeinfo>

using namespace art;
using namespace std;

#ifdef _LIBCPPABI_VERSION

bool
detail::upcastAllowed(type_info const& tid_from, type_info const& tid_to)
{
  if (tid_from == tid_to) {
    return true;
  }
  auto const clFrom = TClass::GetClass(tid_from);
  if (!clFrom) {
    return false;
  }
  auto const clTo = TClass::GetClass(tid_to);
  if (!clTo) {
    return false;
  }
  return clFrom->InheritsFrom(clTo);
}

void const*
art::detail::maybeCastObj(void const* address,
                          std::type_info const& tiFrom,
                          std::type_info const& tiTo)
{
  if (tiFrom == tiTo) {
    // The do nothing case.
    return address;
  }
  auto const clFrom = TClass::GetClass(tiFrom);
  auto const clTo = TClass::GetClass(tiTo);
  if (clFrom && clTo) {
    void const* castAddr(nullptr);
    if (clFrom->InheritsFrom(clTo)) {
      // The upcast case, let ROOT do it.
      castAddr = clFrom->DynamicCast(clTo, const_cast<void*>(address), true);
    } else if (clTo->InheritsFrom(clFrom)) {
      // The downcast case is forbidden.
      throw Exception(errors::TypeConversion)
        << "art::Wrapper<> : unable to convert type "
        << cet::demangle_symbol(tiFrom.name()) << " to "
        << cet::demangle_symbol(tiTo.name()) << ", which is a subclass.\n";
    }
    if (castAddr != nullptr) {
      // ROOT succeeded, done.
      return castAddr;
    }
  }
  // ROOT could not do the upcast, or there was no inheritance
  // relationship between the classes, die.
  throw Exception(errors::TypeConversion)
    << "art::Wrapper<> : unable to convert type "
    << cet::demangle_symbol(tiFrom.name()) << " to "
    << cet::demangle_symbol(tiTo.name()) << "\n";
}

#else // _LIBCPPABI_VERSION

namespace {

  class upcast_result {
  public:
    bool first_call = true;
    bool found = false;
    bool is_ambiguous = false;
    long offset = 0L;

  public:
    void print[[gnu::unused]]() const;
    void reset[[gnu::unused]]();
  };

  void
  upcast_result::print() const
  {
    cout << "       found: " << found << endl;
    cout << "is_ambiguous: " << is_ambiguous << endl;
    cout << "      offset: " << offset << endl;
  }

  void
  upcast_result::reset()
  {
    first_call = true;
    found = false;
    is_ambiguous = false;
    offset = 0L;
  }

  void
  visit_class_for_upcast(abi::__class_type_info const* ci,
                         abi::__class_type_info const* dest,
                         long offset,
                         upcast_result& res)
  {
    if (res.first_call) {
      res.first_call = false;
    } else if (ci == dest) {
      // We found a possible answer.
      if (!res.found) {
        res.found = true;
        res.offset = offset;
      } else {
        res.is_ambiguous = true;
      }
    }
    // Visit bases, if any.
    if (auto si = dynamic_cast<abi::__si_class_type_info const*>(ci)) {
      // Class is part of a public non-virtual single inheritance chain.
      visit_class_for_upcast(si->__base_type, dest, offset, res);
      return;
    }
    if (auto vmi = dynamic_cast<abi::__vmi_class_type_info const*>(ci)) {
      // Class is part of a more complicated inheritance chain.
      for (auto i = 0U; i < vmi->__base_count; ++i) {
        auto const& base = vmi->__base_info[i];
        bool is_public = false;
        if (base.__offset_flags & abi::__base_class_type_info::__public_mask) {
          is_public = true;
        }
        long boff =
          (base.__offset_flags >> abi::__base_class_type_info::__offset_shift);
        if (is_public && (boff > -1)) {
          // Base is public access, and boff is not the offset to the
          // virtual base offset.
          visit_class_for_upcast(base.__base_type, dest, offset + boff, res);
        }
      }
      return;
    }
    // Was a leaf class.
  }

} // unnamed namespace

bool
detail::upcastAllowed(type_info const& tid_from, type_info const& tid_to)
{
  if (tid_from == tid_to) {
    // Trivial, nothing to do.
    return true;
  }
  auto ci_from = dynamic_cast<abi::__class_type_info const*>(&tid_from);
  auto ci_to = dynamic_cast<abi::__class_type_info const*>(&tid_to);
  if (ci_from == nullptr) {
    // Not a class, done.
    return false;
  }
  if (ci_to == nullptr) {
    // Not a class, done.
    return false;
  }
  if (ci_from == ci_to) {
    // Trivial, same types, nothing to do.
    return true;
  }
  upcast_result res;
  visit_class_for_upcast(ci_from, ci_to, 0L, res);
  return res.found && !res.is_ambiguous;
}

void const*
detail::maybeCastObj(void const* ptr,
                     type_info const& tid_from,
                     type_info const& tid_to)
{
  if (tid_from == tid_to) {
    // Trivial, nothing to do.
    return ptr;
  }
  auto ci_from = dynamic_cast<abi::__class_type_info const*>(&tid_from);
  auto ci_to = dynamic_cast<abi::__class_type_info const*>(&tid_to);
  if (ci_from == nullptr) {
    // Not a class, done.
    return ptr;
  }
  if (ci_to == nullptr) {
    // Not a class, done.
    return ptr;
  }
  if (ci_from == ci_to) {
    // Trivial, same types, nothing to do.
    return ptr;
  }
  upcast_result res;
  visit_class_for_upcast(ci_from, ci_to, 0L, res);
  if (!res.found) {
    throw Exception(errors::TypeConversion)
      << "maybeCastObj : unable to convert type: "
      << cet::demangle_symbol(tid_from.name())
      << "\nto: " << cet::demangle_symbol(tid_to.name()) << "\n"
      << "No suitable base found.\n";
  }
  if (res.is_ambiguous) {
    throw Exception(errors::TypeConversion)
      << "MaybeCastObj : unable to convert type: "
      << cet::demangle_symbol(tid_from.name())
      << "\nto: " << cet::demangle_symbol(tid_to.name()) << "\n"
      << "Base class is ambiguous.\n";
  }
  return static_cast<char const*>(ptr) + res.offset;
}

#endif // _LIBCPPABI_VERSION
