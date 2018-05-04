#include "cetlib_except/exception.h"

#include <algorithm>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>

using cet::exception;

namespace {
  struct my_exception : public cet::exception {
    my_exception(Category const& category) : cet::exception(category) {}
  }; // my_exception

  template <template <typename> class COMP>
  class ensure_comp {
  public:
    ensure_comp(std::string const& cmp) : cmp_(cmp) {}

    template <typename X>
    bool
    operator()(X const& left, X const& right) const
    {
      if (!COMP<X>()(left, right)) {
        std::ostringstream msg;
        msg << "FAILED comparison: " << left << ' ' << cmp_ << ' ' << right;
        throw std::runtime_error(msg.str());
      }
      return true;
    }

  private:
    std::string cmp_;
  };

  auto const ensure_equal_to = ensure_comp<std::equal_to>("==");
  auto const ensure_not_equal_to = ensure_comp<std::not_equal_to>("!=");
}

int
main()
{
  // simple_category_check
  ensure_equal_to(exception("Test").category(), std::string("Test"));

  // exception_text_check_01
  {
    exception e("Hello");
    e << "I greet you." << 16 << std::endl;
    ensure_equal_to(e.category(), std::string("Hello"));
    ensure_not_equal_to(e.explain_self().find("I greet you.16\n"),
                        std::string::npos);
  }
  // exception_test_check_02
  {
    try {
      throw exception("Bye") << "Nice meeting you" << std::endl;
    }
    catch (exception const& e) {
      ensure_equal_to(e.category(), std::string("Bye"));
      ensure_not_equal_to(e.explain_self().find("Nice meeting you\n"),
                          std::string::npos);
    }
    catch (...) {
      throw std::runtime_error(
        "Caught unexpected exception in exception_test_check_02");
    }
  }
  // exception_subclass_01
  {
    ensure_equal_to(my_exception("Test").category(), std::string("Test"));
  }

  // exception_subclass_02
  {
    my_exception e("Hello");
    ensure_equal_to(e.category(), std::string("Hello"));
  }

  // exception_subclass_03
  {
    try {
      throw my_exception("Bye") << "Nice meeting you" << std::endl;
    }
    catch (my_exception const& e) {
      ensure_not_equal_to(e.explain_self().find("Nice meeting you\n"),
                          std::string::npos);
    }
    catch (...) {
      throw std::runtime_error(
        "Caught unexpected exception in exception_subclass_03");
    }
  }

  // nesting
  {
    exception e3("Innermost");
    e3 << "Tiny";
    exception e2("Inner", "", e3);
    e2 << "Secondary\nmultiline\nmessage.\n";
    exception e1("Outer", "", e2);
    e1 << "Initial\nmultiline\nmessage.\n";
    std::string wanted("---- Outer BEGIN\n  ---- Inner BEGIN\n    ---- "
                       "Innermost BEGIN\n      Tiny\n    ---- Innermost END\n  "
                       "  Secondary\n    multiline\n    message.\n  ---- Inner "
                       "END\n  Initial\n  multiline\n  message.\n---- Outer "
                       "END\n");
    ensure_equal_to(wanted, e1.explain_self());
    auto wanted_categories = {
      std::string("Outer"), std::string("Inner"), std::string("Innermost")};
    ensure_equal_to(wanted_categories.size(), e1.history().size());
    std::equal(wanted_categories.begin(),
               wanted_categories.end(),
               e1.history().begin(),
               ensure_equal_to);
    ensure_equal_to(e1.root_cause(), std::string("Innermost"));
    ensure_equal_to(e2.root_cause(), std::string("Innermost"));
  }
}
