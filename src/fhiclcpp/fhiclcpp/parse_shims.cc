#include "fhiclcpp/parse_shims.h"
#include "fhiclcpp/parse_shims_opts.h"

#include <iostream>

namespace shims {

  bool
  isSnippetMode(bool m)
  {
    static const auto _mode = bool{m};
    return _mode;
  }
}