// ex11.cc
// vim: set sw=2:
//
// Corrupt backtrace.
//
// Note that Valgrind will report:
// ==36890== Jump to the invalid address stated on the next line
// ==36890==    at 0x4050000000000000: ???
// ==36890==    by 0x404FFFFFFFFFFFFF: ???
// ==36890==    by 0x404FFFFFFFFFFFFF: ???
// ==36890==    by 0x404FFFFFFFFFFFFF: ???
// ==36890==    by 0x404FFFFFFFFFFFFF: ???
// ==36890==    by 0x404FFFFFFFFFFFFF: ???
// ==36890==    by 0x404FFFFFFFFFFFFF: ???
// ==36890==    by 0x404FFFFFFFFFFFFF: ???
// ==36890==    by 0x404FFFFFFFFFFFFF: ???
// ==36890==    by 0x404FFFFFFFFFFFFF: ???
// ==36890==    by 0x404FFFFFFFFFFFFF: ???
// ==36890==    by 0x404FFFFFFFFFFFFF: ???
// ==36890==  Address 0x4050000000000000 is not stack'd, malloc'd or (recently) free'd
// ==36890== 
// ==36890== 
// ==36890== Process terminating with default action of signal 11 (SIGSEGV)
// ==36890==  Bad permissions for mapped region at address 0x4050000000000000
//
//

#include <iomanip>
#include <iostream>

using namespace std;

int I = 0;

void
f()
{
  double V[3] = { 2.0, 0.0, 9.0 };
  double W[3] = { 12.0, 13.0, 14.0 };
  for (I = 0; I < 33; ++I) {
    W[I] = 64.0;
  }
  cout << "W[0]: " << scientific << W[0] << endl;
  cout << "V[0]: " << scientific << V[0] << endl;
}

int
main()
{
  f();
  return 0;
}

