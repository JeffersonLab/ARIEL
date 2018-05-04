#include "canvas/Utilities/Level.h"

using namespace art;

int
main()
{
  static_assert(is_highest_level(Level::Job),
                "is_highest_level static check failed");
  static_assert(is_above_most_deeply_nested_level(Level::Run),
                "is_above_most_deeply_nested_level static check failed");
  static_assert(is_most_deeply_nested_level(Level::Event),
                "is_most_deeply_nested_level static check failed");
  static_assert(is_level_contained_by(Level::InputFile, Level::Job),
                "is_level_contained_by static check failed");
  static_assert(Level::InputFile == level_down(Level::Job),
                "level_down check failed.");
  static_assert(level_up(Level::InputFile) == Level::Job,
                "level_up check failed.");
}
