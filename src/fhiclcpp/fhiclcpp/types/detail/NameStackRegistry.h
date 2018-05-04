#ifndef fhiclcpp_types_detail_NameStackRegistry_h
#define fhiclcpp_types_detail_NameStackRegistry_h

/*

  'NameStackRegistry' exists solely because C++ does not have the
  reflection facilities required to return the members of a struct or
  class, which is needed to walk through the members of a table.  For
  example, consider the following struct and associated Table<S>
  object:

    struct S {
       Atom<int>     n { Name("num") };
       Sequence<int> l { Name("list") };
    };
    Table<S> t { Name("t") };

  The keys for this type of configuration are:

     t
     t.num
     t.list

  C++ , however, does not have the ability to do this.  The
  registration system below, however, does do this by taking the Name
  values above ("t", "num" and "list"), and emplacing them into a
  container that is assembled when each fhiclcpp parameter is
  constructed: first 't', then 'n', then 'l'.

  All fhiclcpp parameters register their names/keys via the
  ParameterBase base class, which, itself, has a member of type
  ParameterMetadata that stores the name/key.

  If the C++ reflection facilities improve to the level that a
  struct's or class's members can be returned (either at compile-time,
  or run-time), this registry should be removed.

*/

#include "fhiclcpp/exception.h"
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

namespace fhicl {

  class NameStackRegistry {
  public:
    std::string full_key(std::string const& key);

    bool
    empty() const
    {
      return names_.empty();
    }

    std::string
    current() const
    {
      return names_.back();
    }

    static void
    end_of_ctor()
    {
      instance().names_.pop_back();
    }

    void
    clear()
    {
      names_.clear();
    }

    static NameStackRegistry&
    instance()
    {
      static NameStackRegistry registry;
      return registry;
    }

  private:
    NameStackRegistry() = default;
    std::vector<std::string> names_{};
  };
}

#endif /* fhiclcpp_types_detail_NameStackRegistry_h */

// Local variables:
// mode: c++
// End:
