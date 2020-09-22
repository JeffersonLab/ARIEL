// ex08.cc
// vim: set sw=2:

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
  auto result = d_ * d_;
  auto resultp = &result;
  return resultp;
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
  delete dv1;
  printDoubleVal(dv1);
  dv1 = nullptr;
  printDoubleVal(dv1);
  return 0;
}

