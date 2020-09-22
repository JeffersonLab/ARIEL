// ex08.cc
// vim: set sw=2:
//
// Returning pointer to value on the stack.
// Access to deleted data.
// Dereferencing a nullptr.
//

#include <iomanip>
#include <iostream>

using namespace std;

class DoubleVal {
private:
  double d_;
public:
  DoubleVal(double);
  double get() const;
  double* squared() const;
};

DoubleVal::DoubleVal(double val)
{
  d_ = val;
}

double
DoubleVal::get() const
{
  return d_;
}

double*
DoubleVal::squared() const
{
  auto result = new double(d_ * d_);
  return result;
}

void
printDoubleVal(DoubleVal* val)
{
  cout << "Value: " << val->get() << endl;
}

int
main()
{
  auto dv1 = new DoubleVal(2.0);
  cout << "dv1->get(): " << scientific << dv1->get() << endl;
  cout << "dv1->squared(): " << scientific << *dv1->squared() << endl;
  auto sq1p = dv1->squared();
  auto val1_1st = *sq1p;
  cout << "val1_1st: " << scientific << val1_1st << endl;
  auto val1_2nd = *sq1p;
  cout << "val1_2nd: " << scientific << val1_2nd << endl;
  delete sq1p;
  sq1p = nullptr;
  printDoubleVal(dv1);
  delete dv1;
  dv1 = nullptr;
  return 0;
}

