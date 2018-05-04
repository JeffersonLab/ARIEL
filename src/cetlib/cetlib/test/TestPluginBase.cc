#include "TestPluginBase.h"

std::string const cet::PluginTypeDeducer<cettest::TestPluginBase>::value =
  "TestPluginBase";

cettest::TestPluginBase::TestPluginBase(std::string message)
  : message_(std::move(message))
{}

std::string const&
cettest::TestPluginBase::message() const
{
  return message_;
}
