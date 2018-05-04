#include "cetlib/test/TestPluginBase.h"

#include "cetlib/ProvideMakePluginMacros.h"
#include "cetlib/compiler_macros.h"

#include <memory>

namespace cettest {
  class TestPlugin;
}

class cettest::TestPlugin : public TestPluginBase {
public:
  TestPlugin(std::string message);
};

cettest::TestPlugin::TestPlugin(std::string message)
  : TestPluginBase(std::move(message))
{}

MAKE_PLUGIN_START(std::unique_ptr<cettest::TestPluginBase>, std::string message)
{
  return std::make_unique<cettest::TestPlugin>(std::move(message));
}
MAKE_PLUGIN_END

DEFINE_BASIC_PLUGINTYPE_FUNC(cettest::TestPluginBase)
