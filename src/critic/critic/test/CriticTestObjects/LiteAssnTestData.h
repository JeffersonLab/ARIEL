#ifndef critic_test_CriticTestObjects_LiteAssnTestData_h
#define critic_test_CriticTestObjects_LiteAssnTestData_h

#include <cstddef>
#include <ostream>
#include <string>

namespace critictest {
  struct LiteAssnTestData;

  std::ostream& operator<<(std::ostream& os, LiteAssnTestData const& item);

  bool operator==(LiteAssnTestData const& left, LiteAssnTestData const& right);
  bool operator!=(LiteAssnTestData const& left, LiteAssnTestData const& right);
} // namespace critictest

struct critictest::LiteAssnTestData {
  LiteAssnTestData() : d1(0), d2(0), label() {}
  LiteAssnTestData(size_t d1, size_t d2, std::string const& label)
    : d1(d1), d2(d2), label(label)
  {}
  size_t d1;
  size_t d2;
  std::string label;
};

inline bool
critictest::operator==(LiteAssnTestData const& left,
                       LiteAssnTestData const& right)
{
  return (left.d1 == right.d1) && (left.d2 == right.d2) &&
         (left.label == right.label);
}

inline bool
critictest::operator!=(LiteAssnTestData const& left,
                       LiteAssnTestData const& right)
{
  return !(left == right);
}
#endif /* critic_test_CriticTestObjects_LiteAssnTestData_h */

// Local Variables:
// mode: c++
// End:
