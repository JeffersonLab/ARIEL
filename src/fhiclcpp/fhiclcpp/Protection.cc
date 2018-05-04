#include "fhiclcpp/Protection.h"

std::string
fhicl::to_string(Protection p)
{
  std::string result;
  switch (p) {
    case Protection::NONE:
      result = "NONE";
      break;
    case Protection::PROTECT_IGNORE:
      result = "PROTECT_IGNORE";
      break;
    case Protection::PROTECT_ERROR:
      result = "PROTECT_ERROR";
      break;
  }
  return result;
}
