#include "fhiclcpp/detail/printing_helpers.h"
#include "fhiclcpp/exception.h"

#include <cassert>
#include <string>

using namespace fhicl::detail;

namespace {
  bool
  not_sequence_element(std::string const& name) try {
    index_for_sequence_element(name);
    return false;
  }
  catch (fhicl::exception const& e) {
    std::string_view msg{e.what()};
    return e.categoryCode() == fhicl::error::other &&
           msg.find("does not correspond to a sequence element") !=
             std::string::npos;
  }
  catch (...) {
    return false;
  }
}

int
main()
{
  assert(index_for_sequence_element("a[0]") == 0);
  assert(index_for_sequence_element("a[5][7]") == 7);
  assert(index_for_sequence_element("a.b.c[15]") == 15);
  assert(not_sequence_element("a"));
  assert(not_sequence_element("a]19["));
}
