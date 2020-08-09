//
// Tell the event display where to look for authorization to
// continue to the event, either input from the keyboard
// or from the mouse.
//

#include "toyExperiment/Analyzers/WaitFor.h"

std::string const&
tex::WaitForDetail::typeName() {
  static std::string type("WaitFor");
  return type;
}

std::map<tex::WaitForDetail::enum_type,std::string> const&
tex::WaitForDetail::names(){

  static const std::map<enum_type,std::string> nam =
    { { unknown,  "unknown"},
      { keyboard, "keyboard"},
      { mouse,    "mouse"}
    };

  return nam;
}

