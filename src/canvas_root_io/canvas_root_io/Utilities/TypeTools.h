#ifndef canvas_root_io_Utilities_TypeTools_h
#define canvas_root_io_Utilities_TypeTools_h
// vim: set sw=2:

// ==============================================================
// TypeTools provides a small number of Reflex-based tools
// ==============================================================

#include "TClass.h"
#include "canvas/Utilities/WrappedClassName.h"
#include "canvas_root_io/Utilities/TypeWithDict.h"
#include <ostream>
#include <string>
#include <vector>

namespace art {
  namespace root {

    // Expensive: will almost certainly cause a ROOT autoparse.
    TypeWithDict find_nested_type_named(std::string const& nested_type,
                                        TClass* const type_to_search);

    TypeWithDict value_type_of(TClass* t);
    TypeWithDict mapped_type_of(TClass* t);

    std::vector<TClass*> public_base_classes(TClass* cl);

    TypeWithDict type_of_template_arg(TClass* template_instance,
                                      size_t desired_arg);
    TypeWithDict type_of_template_arg(std::string const& template_instance,
                                      size_t desired_arg);

    TypeWithDict type_of_assns_partner(std::string const& assns_type_name);
    TypeWithDict type_of_assns_base(std::string const& assns_type_name);

    bool is_instantiation_of(TClass* cl, std::string const& template_name);

    [[noreturn]] void throwLateDictionaryError(std::string const& className);

  } // namespace root
} // namespace art

inline auto
art::root::type_of_template_arg(std::string const& template_instance,
                                size_t const desired_arg) -> TypeWithDict
{
  return TypeWithDict{name_of_template_arg(template_instance, desired_arg)};
}

inline auto
art::root::type_of_assns_partner(std::string const& assns_type_name)
  -> TypeWithDict
{
  return TypeWithDict{name_of_assns_partner(assns_type_name)};
}

inline auto
art::root::type_of_assns_base(std::string const& assns_type_name)
  -> TypeWithDict
{
  return TypeWithDict{name_of_assns_base(assns_type_name)};
}

#endif /* canvas_root_io_Utilities_TypeTools_h */

// Local Variables:
// mode: c++
// End:
