#include "canvas/Utilities/Level.h"

using namespace art;

int
main()
{
  static_assert(is_highest_level(Level::Job));
  static_assert(is_above_most_deeply_nested_level(Level::Run));
  static_assert(is_most_deeply_nested_level(Level::Event));
  static_assert(is_level_contained_by(Level::InputFile, Level::Job));
  static_assert(Level::InputFile == level_down(Level::Job));
  static_assert(level_up(Level::InputFile) == Level::Job);
}
