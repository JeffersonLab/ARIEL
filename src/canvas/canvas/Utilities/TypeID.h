#ifndef canvas_Utilities_TypeID_h
#define canvas_Utilities_TypeID_h
// vim: set sw=2 expandtab :

//
// TypeID: A unique identifier for a C++ type.
//
// The identifier is unique within an entire program (not really,
// it is supposed to be, but gcc violates the standard requirement
// that a type_info should have a unique address for performance
// reasons) but cannot be persisted across invocations of the program.
//

#include "hep_concurrency/RecursiveMutex.h"
#include "hep_concurrency/tsan.h"

#include <iosfwd>
#include <map>
#include <string>
#include <typeinfo>
#include <utility>

namespace art {

  class TypeID {

    // STATIC MEMBER FUNCTIONS
  public:
    static void startup();
    static void shutdown();

    // STATIC DATA MEMBERS
  private:
    static hep::concurrency::RecursiveMutex* s_mutex;
    static std::map<size_t, std::string>* s_nameMap;

    // MEMBER FUNCTIONS -- Special Member Functions
  public:
    ~TypeID() noexcept;
    TypeID() noexcept;
    explicit TypeID(std::type_info const&) noexcept;
    explicit TypeID(std::type_info const*) noexcept;
    template <typename T>
    explicit TypeID(T const& val) noexcept;
    TypeID(TypeID const&) noexcept;
    TypeID(TypeID&) noexcept;
    TypeID& operator=(TypeID const&) noexcept;
    TypeID& operator=(TypeID&) noexcept;

    // MEMBER FUNCTIONS -- API for the user
  public:
    std::type_info const& typeInfo() const;
    char const* name() const;
    std::string className() const;
    std::string friendlyClassName() const;
    bool operator<(TypeID const&) const;
    bool operator==(TypeID const&) const;
    explicit operator bool() const;
    void swap(TypeID&);
    void print(std::ostream&) const;

    // MEMBER DATA
  private:
    std::type_info const* ti_{nullptr};
  };

  template <typename T>
  TypeID::TypeID(T const& val) noexcept : ti_{&typeid(val)}
  {}

  inline bool
  is_instantiation_of(std::string const& type_name,
                      std::string const& template_name)
  {
    return type_name.find(template_name + '<') == 0ull;
  }

  inline bool
  is_instantiation_of(TypeID const& tid, std::string const& template_name)
  {
    return is_instantiation_of(tid.className(), template_name);
  }

  inline bool
  is_assns(std::string const& type_name)
  {
    using namespace std::string_literals;
    return is_instantiation_of(type_name, "art::Assns"s);
  }

  inline bool
  is_assns(TypeID const& tid)
  {
    return is_assns(tid.className());
  }

  bool operator>(TypeID const&, TypeID const&);
  bool operator!=(TypeID const&, TypeID const&);
  std::ostream& operator<<(std::ostream&, TypeID const&);
  void swap(TypeID&, TypeID&);
  std::string name_of_template_arg(std::string const& template_instance,
                                   size_t desired_arg);
  std::string name_of_assns_partner(std::string assns_type_name);
  std::string name_of_assns_base(std::string assns_type_name);
  std::string name_of_unwrapped_product(std::string const& wrapped_name);

} // namespace art

// Local Variables:
// mode: c++
// End:
#endif /* canvas_Utilities_TypeID_h */
