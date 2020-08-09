// ex05.cc
// vim: set sw=2:
//
// Demo of infinity and nan.
//

#include <iostream>
#include <math.h>
#include <vector>

using namespace std;


void
not_called()
{
  vector<double> junk = { 0.0 };
  junk.size();
  junk.capacity();
  junk.max_size();
  junk.data();
  junk[0];
  junk.at(0);
}

int
main()
{
  vector<double> V = { 2.0, 0.0, 9.0 };
  for (auto val : V) {
    double num = 1.0 / val;
    int have_inf = isinf(num);
    int have_nan = isnan(num);
    cout << "-----" << endl;
    cout << "  num: " << num << endl;
    cout << "isinf: " << have_inf << endl;
    if (have_inf) {
      cout << "num == num: " << (num == num) << endl;
    }
    cout << "isnan: " << have_nan << endl;
    if (have_nan) {
      cout << "num == num: " << (num == num) << endl;
    }
  }
  double val = 0.0;
  double num = 0.0 / val;
  int have_inf = isinf(num);
  int have_nan = isnan(num);
  cout << "-----" << endl;
  cout << "  num: " << num << endl;
  cout << "isinf: " << have_inf << endl;
  if (have_inf) {
    cout << "num == num: " << (num == num) << endl;
  }
  cout << "isnan: " << have_nan << endl;
  if (have_nan) {
    cout << "num == num: " << (num == num) << endl;
  }
  return 0;
}

// Local Variables:
// mode: c++
// End:
