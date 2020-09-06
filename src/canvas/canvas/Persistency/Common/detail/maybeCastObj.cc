#include "canvas/Persistency/Common/detail/maybeCastObj.h"
// vim: set sw=2:

#include "canvas/Utilities/Exception.h"
#include "cetlib_except/demangle.h"

#include <cxxabi.h>

#include <iomanip>
#include <iostream>
#include <typeinfo>

using namespace art;
using namespace std;

#ifdef _LIBCPPABI_VERSION
// When using libc++, under the assumption that it uses libc++abi,
// libc++abi implements the __class_type_info classes, but does
// not expose them in cxxabi.h. However, seem to be able to
// provide the declarations, link to c++abi, and use them.
// TODO:
// 1. Note sure about providing *definition* of constructors
//    vs just declarations of destructors
// 2. Configure time check on availability of __class_type_info?
// 3. Configure time check on which c++ ABI library is used?
//
namespace __cxxabiv1 {
  // Type information for a class
  class __class_type_info : public std::type_info {
  public:
    explicit __class_type_info(char const* n) : type_info(n) {}
    virtual ~__class_type_info() override;
  };

  // Type information for a class with a single non-virtual base
  class __si_class_type_info : public __class_type_info {
  public:
    __class_type_info const* __base_type;
    explicit __si_class_type_info(char const* n, __class_type_info const* base)
      : __class_type_info(n), __base_type(base)
    {}
    virtual ~__si_class_type_info() override;
  };

  // Helper class for __vmi_class_type.
  struct __base_class_type_info {
    __class_type_info const* __base_type;
#if defined _GLIBCXX_LLP64
    long long __offset_flags;
#else
    long __offset_flags;
#endif
    enum __offset_flags_masks {
      __virtual_mask = 0x1,
      __public_mask = 0x2,
      __offset_shift = 8
    };
  };

  // Type information for a class with multiple and/or virtual bases.
  class __vmi_class_type_info : public __class_type_info {
  public:
    unsigned int __flags;
    unsigned int __base_count;
    __base_class_type_info __base_info[1];

    enum __flags_masks {
      __non_diamond_repeat_mask = 0x1,
      __diamond_shaped_mask = 0x2,
      __flags_unknown_mask = 0x10
    };

    explicit __vmi_class_type_info(char const* n, int flags)
      : __class_type_info(n), __flags(flags), __base_count(0)
    {}
    virtual ~__vmi_class_type_info() override;
  };
}
#endif // _LIBCPPABI_VERSION

namespace {

  class upcast_result {
  public:
    bool first_call = true;
    bool found = false;
    bool is_ambiguous = false;
    long offset = 0L;

  public:
    void print [[maybe_unused]] () const;
    void reset [[maybe_unused]] ();
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
